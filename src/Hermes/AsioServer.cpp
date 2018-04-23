/***********************************************************************
Copyright 2018 ASM Assembly Systems GmbH & Co. KG

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
************************************************************************/

#include "stdafx.h"
#include "AsioSocket.h"
#include "IService.h"
#include "MessageSerialization.h"
#include "StringBuilder.h"

#include <HermesData.hpp>

#include <boost/asio.hpp>

#include <mutex>

namespace asio = boost::asio;

namespace Hermes
{
    using AsioSocketSp = std::shared_ptr<AsioSocket>;

    struct ServerSocket : IServerSocket
    {
        AsioSocketSp m_spSocket;

        ServerSocket(AsioSocketSp&& spSocket)
            : m_spSocket(std::move(spSocket))
        {}

        ~ServerSocket()
        {
            m_spSocket->Close();
        }

        // implementation of IServerSocket:
        unsigned SessionId() const
        {
            return m_spSocket->m_sessionId;
        }

        const ConnectionInfo& GetConnectionInfo() const override
        {
            return m_spSocket->m_connectionInfo;
        }

        const NetworkConfiguration& GetConfiguration() const override
        {
            return m_spSocket->m_configuration;
        }

        void Connect(std::weak_ptr<void> wpOwner, ISocketCallback& callback) override
        {
            m_spSocket->m_wpOwner = std::move(wpOwner);
            m_spSocket->m_pCallback = &callback;
            m_spSocket->m_asioService.post([spSocket = m_spSocket]()
            {
                if (spSocket->Closed())
                    return;
                spSocket->m_pCallback->OnConnected(spSocket->m_connectionInfo);
            });
            m_spSocket->StartReceiving();
        }

        void Send(StringView message) override
        {
            m_spSocket->Send(message);
        }

        void Close() override
        {
            m_spSocket->Close();
        }
    };

    struct AcceptorResources
    {
        asio::deadline_timer m_timer;
        asio::ip::tcp::acceptor m_acceptor;
        bool m_closed = false;

        AcceptorResources(asio::io_service& asioService) :
            m_timer(asioService),
            m_acceptor(asioService)
        {}
    };

    struct AsioAcceptor : IAcceptor
    {
        unsigned m_sessionId = 1U;
        IAsioService& m_service;
        asio::io_service& m_asioService{m_service.GetUnderlyingService()};
        Optional<NetworkConfiguration> m_optionalConfiguration;
        IAcceptorCallback& m_callback;
        std::shared_ptr<AcceptorResources> m_spResources{std::make_shared<AcceptorResources>(m_asioService)};

        AsioAcceptor(IAsioService& asioService, IAcceptorCallback& callback) :
            m_service(asioService),
            m_callback(callback)
        {}

        ~AsioAcceptor()
        {
            Close_();
        }

        // implementation of IAcceptor:
        void StartListening(const NetworkConfiguration& configuration) override
        {
            m_service.Log(m_sessionId, "Start Listening(", configuration, "), currentConfig=", m_optionalConfiguration);

            if (m_optionalConfiguration && *m_optionalConfiguration == configuration)
                return;

            m_optionalConfiguration = configuration;
            boost::system::error_code ecDummy;
            m_spResources->m_acceptor.cancel(ecDummy);
            m_spResources->m_acceptor.close(ecDummy);
            Listen_();
        }

        void StopListening() override
        {
            m_optionalConfiguration.reset();
            boost::system::error_code ecDummy;
            m_spResources->m_acceptor.close(ecDummy);
        }

        // internals
        void Listen_()
        {
            if (m_spResources->m_closed)
                return;

            if (!m_optionalConfiguration)
                return;

            auto& configuration = *m_optionalConfiguration;
            m_service.Log(m_sessionId, "Listen_ on ", configuration.m_hostName,
                ", port=", configuration.m_port);

            if (!m_optionalConfiguration->m_hostName.empty())
            {
                asio::ip::tcp::resolver resolver(m_asioService);
                asio::ip::tcp::resolver::query query(asio::ip::tcp::v4(), configuration.m_hostName, "");
                boost::system::error_code ec;
                resolver.resolve(query, ec);
                if (ec)
                {
                    Alarm(ec, "Unable to resolve ", query.host_name());
                    RetryLater_();
                    return;
                }
            }

            asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), configuration.m_port);

            boost::system::error_code ec;
            m_spResources->m_acceptor.open(asio::ip::tcp::v4(), ec);
            if (ec)
            {
                Alarm(ec, "Unable to open accept port ", configuration.m_port);
                RetryLater_();
                return;
            }

            // it appears to be robuster not to re-use, but instead to retry later
            //m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true), ec);
            //if (ec)
            //{
            //    m_asioHelper.Alarm(ec, "Unable to bind to re-use accepting port");
            //    Retry_();
            //    return;
            //}

            m_spResources->m_acceptor.bind(endpoint, ec);
            if (ec)
            {
                Alarm(ec, "Unable to bind to accept port ", configuration.m_port);
                RetryLater_();
                return;
            }

            m_spResources->m_acceptor.listen(asio::socket_base::max_connections, ec);
            if (ec)
            {
                Alarm(ec, "Unable to listen on accept port ", configuration.m_port);
                RetryLater_();
                return;
            }

            AsyncAccept_();
        }

        void AsyncAccept_()
        {
            if (m_spResources->m_closed)
                return;

            if (!m_optionalConfiguration)
                return;

            auto& configuration = *m_optionalConfiguration;

            m_service.Log(m_sessionId, "AsyncAccept_ on ", configuration.m_hostName,
                ", port=", configuration.m_port);

            auto spSocket = std::make_shared<AsioSocket>(m_sessionId, configuration, m_service);
            spSocket->m_connectionInfo.m_port = configuration.m_port;
            auto& asioSocket = spSocket->m_socket;
            m_spResources->m_acceptor.async_accept(asioSocket,
                [this, spSocket = std::move(spSocket), spResources = m_spResources](const boost::system::error_code& ec) mutable
            {
                if (spResources->m_closed)
                    return;

                OnAccepted_(std::move(spSocket), ec);
            });
        }

        void OnAccepted_(AsioSocketSp&& spSocket, const boost::system::error_code& ec)
        {
            if (ec)
            {
                if (ec == asio::error::operation_aborted)
                    return;
                if (!m_optionalConfiguration)
                    return;

                Alarm(ec, "Accept error on port ", m_optionalConfiguration->m_port);
                RetryLater_();
                return;
            }

            // we have accepted, so increment the session id:
            m_sessionId = m_sessionId == std::numeric_limits<unsigned>::max() ? 1U : m_sessionId + 1U;

            // does the socket's configuration match the current one
            if (!m_optionalConfiguration || spSocket->m_configuration != *m_optionalConfiguration)
            {
                m_service.Warn(spSocket->m_sessionId, "Configuration of accepted socket no longer matches the current configuration");
                NotificationData notification(ENotificationCode::eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION,
                    ESeverity::eINFO, "ConfigurationChanged");
                const std::string& xmlString = Serialize(notification);
                spSocket->Send(xmlString);
                AsyncAccept_();
                return;
            }
            auto& configuration = *m_optionalConfiguration;
            
            asio::ip::tcp::resolver resolver(m_asioService);
            boost::system::error_code ecResolve;
            const auto& endpoint = spSocket->m_socket.remote_endpoint();
            const auto& remoteAddress = endpoint.address();
            spSocket->m_connectionInfo.m_address = remoteAddress.to_string();

            // try and resolve the remote address to a name:
            auto itResolved = resolver.resolve(endpoint, ecResolve);
            if (ec)
            {
                spSocket->Alarm(ec, "Unable to resolve ip address ", endpoint);
            }
            else
            {
                spSocket->m_connectionInfo.m_hostName = itResolved->host_name();
            }

            if (!configuration.m_hostName.empty())
            {
                asio::ip::tcp::resolver::query query(asio::ip::tcp::v4(), configuration.m_hostName, "");
                auto itAllowedEndpoint = resolver.resolve(query, ecResolve);
                if (ecResolve)
                {
                    spSocket->Alarm(ec, "Unable to resolve ", query.host_name());
                    NotificationData notification(ENotificationCode::eCONFIGURATION_ERROR, ESeverity::eERROR,
                        "Connection only allowed from a hostname which cannot be resolved: " + configuration.m_hostName);
                    const std::string& xmlString = Serialize(notification);
                    spSocket->Send(xmlString);
                    spSocket->Close();
                    RetryLater_();
                    return;
                }

                const auto& allowedEndpoint = itAllowedEndpoint->endpoint();
                const auto& allowedAddress = allowedEndpoint.address();
                if (remoteAddress != allowedAddress)
                {
                    std::ostringstream oss;
                    oss << "Remote host does not match allowed host " << configuration.m_hostName
                        << ",\nAllowed resolved hostname=" << itAllowedEndpoint->host_name()
                        << ",\nAllowed address=" << allowedAddress.to_string()
                        << ",\nRemote resolved hostname=" << spSocket->m_connectionInfo.m_hostName
                        << ",\nRemote address=" << spSocket->m_connectionInfo.m_address;
                    const std::string& text = oss.str();
                    m_service.Warn(m_sessionId, text);

                    NotificationData notification(ENotificationCode::eCONFIGURATION_ERROR, ESeverity::eWARNING, text);
                    const std::string& xmlString = Serialize(notification);
                    spSocket->Send(xmlString);
                    spSocket->Close();
                    AsyncAccept_();
                    return;
                }
            }

            m_callback.OnAccepted(std::make_unique<ServerSocket>(std::move(spSocket)));
            AsyncAccept_();
        }

        void RetryLater_()
        {
            if (m_spResources->m_closed)
                return;

            if (!m_optionalConfiguration)
                return;

            auto& configuration = *m_optionalConfiguration;

            boost::system::error_code ecDummy;
            m_spResources->m_acceptor.close(ecDummy);

            m_spResources->m_timer.expires_from_now(boost::posix_time::milliseconds(static_cast<int>(1000.0 * configuration.m_retryDelayInSeconds)));
            m_spResources->m_timer.async_wait([this, spResources = m_spResources](const boost::system::error_code& ec)
            {
                if (spResources->m_closed)
                    return;

                if (ec) // we were canccelled
                    return;

                Listen_();
            });
        }

        void Close_()
        {
            if (m_spResources->m_closed)
                return;

            m_spResources->m_closed = true;
            m_service.Log(m_sessionId, "Close Acceptor");

            boost::system::error_code ecDummy;
            m_spResources->m_acceptor.close(ecDummy);
            m_spResources->m_timer.cancel(ecDummy);
        }

        template<class... Ts>
        Error Alarm(const boost::system::error_code& ec, const Ts&... trace)
        {
            return Hermes::Alarm(m_service, m_sessionId, ec, trace...);
        }

    };
}
std::unique_ptr<Hermes::IAcceptor> Hermes::CreateAcceptor(IAsioService& asioService, IAcceptorCallback& callback)
{
    return std::make_unique<AsioAcceptor>(asioService, callback);
}
