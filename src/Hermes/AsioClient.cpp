#include "stdafx.h"
#include "Network.h"

#include "AsioSocket.h"

#include <HermesData.hpp>
#include "IService.h"

#include <boost/asio.hpp>

#include <memory>

namespace asio = boost::asio;

namespace Hermes
{
    struct ClientSocket : IClientSocket
    {
        AsioSocket m_socket;

        ClientSocket(unsigned sessionId, const NetworkConfiguration& configuration, 
            IAsioService& asioService) :
            m_socket(sessionId, configuration, asioService)
        {}

        ~ClientSocket()
        {
            m_socket.Close();
        }

        //========================= implementation of IClientSocket ============================
        unsigned SessionId() const
        {
            return m_socket.m_sessionId;
        }

        const ConnectionInfo& GetConnectionInfo() const override
        {
            return m_socket.m_connectionInfo;
        }

        const NetworkConfiguration& GetConfiguration() const override
        {
            return m_socket.m_configuration;
        }
        
        void Connect(std::weak_ptr<void> wpOwner, ISocketCallback& callback) override
        {
            assert(!m_socket.m_pCallback);
            m_socket.m_wpOwner = wpOwner;
            m_socket.m_pCallback = &callback;
            AsyncConnect_();
        }

        void Send(StringView message) override 
        { 
            m_socket.Send(message); 
        }

        void Close() override 
        { 
            m_socket.Close(); 
        }

        //================= implementation ===============================
        std::shared_ptr<ClientSocket> shared_from_this()
        {
            return std::shared_ptr<ClientSocket>(m_socket.m_wpOwner.lock(), this);
        }

        void AsyncConnect_()
        {
            if (m_socket.Closed())
            {
                m_socket.m_service.Log(m_socket.m_sessionId, "AsyncConnect_, but already closed");
                return;
            }

            m_socket.m_service.Log(m_socket.m_sessionId, "AsyncConnect_ to host=", 
                m_socket.m_configuration.m_hostName, " on port=", m_socket.m_configuration.m_port);

            asio::ip::tcp::resolver resolver(m_socket.m_asioService);
            asio::ip::tcp::resolver::query query(asio::ip::tcp::v4(), m_socket.m_configuration.m_hostName, "");

            boost::system::error_code ec;
            auto itEndpoint = resolver.resolve(query, ec);
            if (ec)
            {
                m_socket.Alarm(ec, "Unable to resolve ", query.host_name());
                RetryLater_();
                return;
            }
                
            asio::ip::tcp::endpoint endpoint(*itEndpoint);
            endpoint.port(m_socket.m_configuration.m_port);

            m_socket.m_connectionInfo.m_address = endpoint.address().to_string();
            m_socket.m_connectionInfo.m_port = endpoint.port();
            m_socket.m_connectionInfo.m_hostName = itEndpoint->host_name();

            m_socket.m_service.Inform(m_socket.m_sessionId, "Connecting to ", m_socket.m_connectionInfo, " ...");
            m_socket.m_socket.async_connect(endpoint, 
                [spThis = shared_from_this()](const boost::system::error_code& ec)
            {
                if (spThis->m_socket.Closed())
                    return;

                spThis->OnConnected_(ec);
            });
        }

        void OnConnected_(const boost::system::error_code& ec)
        {
            if (ec)
            {
                m_socket.Alarm(ec, "Unable to connect to ", m_socket.m_connectionInfo);
                RetryLater_();
                return;
            }

            assert(m_socket.m_pCallback);
            if (!m_socket.m_pCallback)
            {
                m_socket.m_service.Alarm(m_socket.m_sessionId, Hermes::EErrorCode::eIMPLEMENTATION_ERROR, 
                    "Connected, but no callback");
                return;
            }

            m_socket.m_pCallback->OnConnected(m_socket.m_connectionInfo);
            m_socket.StartReceiving();
        }

        void RetryLater_()
        {
            if (m_socket.Closed())
                return;

            m_socket.m_timer.expires_from_now(
                boost::posix_time::seconds(m_socket.m_configuration.m_retryDelayInSeconds));
            m_socket.m_timer.async_wait([spThis = shared_from_this()](const boost::system::error_code& ec)
            {
                if (spThis->m_socket.Closed())
                    return;

                if (ec) // we were canccelled
                    return;

                spThis->AsyncConnect_();
            });
        }
    };
}

std::unique_ptr<Hermes::IClientSocket> Hermes::CreateClientSocket(unsigned sessionId,
    const NetworkConfiguration& configuration, IAsioService& asioService)
{
    return std::make_unique<ClientSocket>(sessionId, configuration, asioService);
}
