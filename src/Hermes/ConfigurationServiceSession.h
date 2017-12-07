#pragma once

#include <HermesData.hpp>

#include <memory>

namespace Hermes
{
    struct IAsioService;
    struct IServerSocket;

    struct IConfigurationServiceSessionCallback;

    class ConfigurationServiceSession
    {
    public:
        ConfigurationServiceSession(std::unique_ptr<IServerSocket>&&, IAsioService&, const ConfigurationServiceSettings&);
        ConfigurationServiceSession(const ConfigurationServiceSession&) = default;
        ConfigurationServiceSession& operator=(const ConfigurationServiceSession&) = default;
        ConfigurationServiceSession(ConfigurationServiceSession&&) = default;
        ConfigurationServiceSession& operator=(ConfigurationServiceSession&&) = default;
        ~ConfigurationServiceSession();

        unsigned Id() const;
        const ConnectionInfo& PeerConnectionInfo() const;

        void Connect(IConfigurationServiceSessionCallback&);
        void Disconnect(const NotificationData&);

    private:
        struct Impl;
        std::shared_ptr<Impl> m_spImpl;
    };

    struct IGetConfigurationResponse
    {
        virtual unsigned Id() const = 0; 
        virtual void Signal(const CurrentConfigurationData&) = 0;

    };

    struct ISetConfigurationResponse
    {
        virtual unsigned Id() const = 0;
        virtual void Signal(const NotificationData&) = 0;
    };

    struct IConfigurationServiceSessionCallback
    {
        virtual void OnSocketConnected(unsigned sessionId, const ConnectionInfo&) = 0;
        virtual void OnGet(unsigned sessionId, const ConnectionInfo&, const GetConfigurationData&, 
            IGetConfigurationResponse&) = 0;
        virtual void OnSet(unsigned sessionId, const ConnectionInfo&, const SetConfigurationData&,
            ISetConfigurationResponse&) = 0;
        virtual void OnDisconnected(unsigned sessionId, const Error&) = 0;
    };

}
