// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include "HermesDataConversion.hpp"

#include <functional>
#include <memory>


// These are lightweight C++ wrappers around the Hermes C Api:
namespace Hermes
{
    //======================= Downstream interface =====================================
    struct IDownstreamCallback;
    class Downstream
    {
    public:
        explicit Downstream(unsigned laneId, IDownstreamCallback&);
        Downstream(const Downstream&) = delete;
        Downstream& operator=(const Downstream&) = delete;
        ~Downstream() { ::DeleteHermesDownstream(m_pImpl); }

        void Run();
        template<class F> void Post(F&&);
        void Enable(const DownstreamSettings&);

        void Signal(unsigned sessionId, const ServiceDescription&);
        void Signal(unsigned sessionId, const BoardAvailableData&);
        void Signal(unsigned sessionId, const RevokeBoardAvailableData&);
        void Signal(unsigned sessionId, const TransportFinishedData&);
        void Signal(unsigned sessionId, const NotificationData&);
        void Signal(unsigned sessionId, const CheckAliveData&);

        void Reset(const NotificationData&);
        void Disable(const NotificationData&);
        void Stop();

    private:
        HermesDownstream* m_pImpl = nullptr;
    };

    struct IDownstreamCallback
    {
        IDownstreamCallback() = default;

        // callbacks must not be moved or copied!
        IDownstreamCallback(const IDownstreamCallback&) = delete;
        IDownstreamCallback& operator=(const IDownstreamCallback&) = delete;

        virtual void OnConnected(unsigned sessionId, EState, const ConnectionInfo&) = 0;
        virtual void On(unsigned sessionId, const NotificationData&) = 0;
        virtual void On(unsigned /*sessionId*/, const CheckAliveData&) {} // not necessarily interesting, hence not abstract
        virtual void OnState(unsigned sessionId, EState) = 0;
        virtual void OnDisconnected(unsigned sessionId, EState, const Error&) = 0;

        virtual void On(unsigned sessionId, EState, const ServiceDescription&) = 0;
        virtual void On(unsigned sessionId, EState, const MachineReadyData&) = 0;
        virtual void On(unsigned sessionId, EState, const RevokeMachineReadyData&) = 0;
        virtual void On(unsigned sessionId, EState, const StartTransportData&) = 0;
        virtual void On(unsigned sessionId, EState, const StopTransportData&) = 0;

        virtual void OnTrace(unsigned sessionId, ETraceType, StringView trace) = 0;

    protected:
        virtual ~IDownstreamCallback() = default;
    };

    //======================= Upstream interface =====================================
    struct IUpstreamCallback;
    class Upstream
    {
    public:
        explicit Upstream(unsigned laneId, IUpstreamCallback&);
        Upstream(const Upstream&) = delete;
        Upstream& operator=(const Upstream&) = delete;
        ~Upstream() { ::DeleteHermesUpstream(m_pImpl); }

        void Run();
        template<class F> void Post(F&&);
        void Enable(const UpstreamSettings&);

        void Signal(unsigned sessionId, const ServiceDescription&);
        void Signal(unsigned sessionId, const MachineReadyData&);
        void Signal(unsigned sessionId, const RevokeMachineReadyData&);
        void Signal(unsigned sessionId, const StartTransportData&);
        void Signal(unsigned sessionId, const StopTransportData&);
        void Signal(unsigned sessionId, const NotificationData&);
        void Signal(unsigned sessionId, const CheckAliveData&);

        void Reset(const NotificationData&);
        void Disable(const NotificationData&);
        void Stop();

    private:
        HermesUpstream* m_pImpl = nullptr;
    };

    struct IUpstreamCallback
    {
        IUpstreamCallback() = default;

        // callbacks must not be moved or copied!
        IUpstreamCallback(const IUpstreamCallback&) = delete;
        IUpstreamCallback& operator=(const IUpstreamCallback&) = delete;

        virtual void OnConnected(unsigned sessionId, EState, const ConnectionInfo&) = 0;
        virtual void On(unsigned sessionId, const NotificationData&) = 0;
        virtual void On(unsigned /*sessionId*/, const CheckAliveData&) {} // not necessarily interesting, hence not abstract
        virtual void OnState(unsigned sessionId, EState) = 0;
        virtual void OnDisconnected(unsigned sessionId, EState, const Error&) = 0;

        virtual void On(unsigned sessionId, EState, const ServiceDescription&) = 0;
        virtual void On(unsigned sessionId, EState, const BoardAvailableData&) = 0;
        virtual void On(unsigned sessionId, EState, const RevokeBoardAvailableData&) = 0;
        virtual void On(unsigned sessionId, EState, const TransportFinishedData&) = 0;

        virtual void OnTrace(unsigned sessionId, ETraceType, StringView trace) = 0;

    protected:
        virtual ~IUpstreamCallback() = default;
    };

    //======================= ConfigurationService interface =====================================
    struct IConfigurationServiceCallback;
    class ConfigurationService
    {
    public:
        explicit ConfigurationService(IConfigurationServiceCallback& callback);
        ConfigurationService(const ConfigurationService&) = delete;
        ConfigurationService& operator=(const ConfigurationService&) = delete;
        ~ConfigurationService() { ::DeleteHermesConfigurationService(m_pImpl); }

        void Run();
        template<class F> void Post(F&&);
        void Enable(const ConfigurationServiceSettings&);
        void Disable(const NotificationData&);
        void Stop();

    private:
        HermesConfigurationService* m_pImpl = nullptr;
        IConfigurationServiceCallback& m_callback;
    };

    struct IConfigurationServiceCallback
    {
        IConfigurationServiceCallback() = default;

        // callbacks must not be moved or copied!
        IConfigurationServiceCallback(const IConfigurationServiceCallback&) = delete;
        IConfigurationServiceCallback& operator=(const IConfigurationServiceCallback&) = delete;

        virtual void OnConnected(unsigned sessionId, const ConnectionInfo&) = 0;
        virtual Error OnSetConfiguration(unsigned sessionId, const ConnectionInfo&, const SetConfigurationData&) = 0;
        virtual CurrentConfigurationData OnGetConfiguration(unsigned sessionId, const ConnectionInfo&) = 0;
        virtual void OnDisconnected(unsigned sessionId, const Error&) = 0;

        virtual void OnTrace(unsigned sessionId, ETraceType, StringView trace) = 0;

    protected:
        virtual ~IConfigurationServiceCallback() = default;
    };

    //======================= ConfigurationClient interface =====================================
    struct ITraceCallback
    {
        virtual void OnTrace(unsigned sessionId, ETraceType, StringView trace) = 0;

    protected:
        ~ITraceCallback() {}
    };

    std::pair<CurrentConfigurationData, Error> GetConfiguration(StringView hostName, // host (e.g. ip address) to connect with
        unsigned timeoutInSeconds, // timeout to connect
        ITraceCallback* // optional
    );

    Error SetConfiguration(StringView hostName, // host (e.g. ip address) to connect with
        const Hermes::SetConfigurationData&, // configuration to set
        unsigned timeoutInSeconds, // timeout to connect
        Hermes::CurrentConfigurationData*, // optional out: resulting configuration
        std::vector<NotificationData>*, // optional out: notification data
        ITraceCallback* // optional
    );

    // Convenience interface for situations in which the same class implements IUpstreamCallback and IDownstreamCallback.
    // Because some function names clash, this adds a level of indirection to different function names, eg.
    // On(unsigned, const ServiceDescription&)
    // becomes:
    // OnUpstream(unsigned, const ServiceDescription&)
    // and
    // OnDownstream(unsigned, const ServiceDescription&)
    //
    // Deriving from UpstreamCallbackHelper and DownstreamCallbackHelper gives unique names for all callback methods.
    // Unambiguous methods, eg. On(unsigned, EState, const BoardAvailableData&) remain unchanged!
    struct UpstreamCallbackHelper : IUpstreamCallback
    {
        virtual void OnUpstreamConnected(unsigned sessionId, EState, const ConnectionInfo&) = 0;
        virtual void OnUpstream(unsigned sessionId, EState, const ServiceDescription&) = 0;
        virtual void OnUpstream(unsigned sessionId, const NotificationData& data) = 0;
        virtual void OnUpstream(unsigned sessionId, const CheckAliveData& data) = 0;
        virtual void OnUpstreamState(unsigned sessionId, EState) = 0;
        virtual void OnUpstreamDisconnected(unsigned sessionId, EState, const Error&) = 0;
        virtual void OnUpstreamTrace(unsigned sessionId, ETraceType, StringView trace) = 0;

        void OnConnected(unsigned sessionId, EState state, const ConnectionInfo& data) override { OnUpstreamConnected(sessionId, state, data); }
        void On(unsigned sessionId, EState state, const ServiceDescription& data) override { OnUpstream(sessionId, state, data); }
        void On(unsigned sessionId, const NotificationData& data) override { OnUpstream(sessionId, data); }
        void On(unsigned sessionId, const CheckAliveData& data) override { OnUpstream(sessionId, data); }
        void OnState(unsigned sessionId, EState state) override { OnUpstreamState(sessionId, state); }
        void OnDisconnected(unsigned sessionId, EState state, const Error& data) override { OnUpstreamDisconnected(sessionId, state, data); }
        void OnTrace(unsigned sessionId, ETraceType type, StringView data) override { OnUpstreamTrace(sessionId, type, data); }
    };

    struct DownstreamCallbackHelper : IDownstreamCallback
    {
        virtual void OnDownstreamConnected(unsigned sessionId, EState, const ConnectionInfo&) = 0;
        virtual void OnDownstream(unsigned sessionId, EState, const ServiceDescription&) = 0;
        virtual void OnDownstream(unsigned sessionId, const NotificationData& data) = 0;
        virtual void OnDownstream(unsigned sessionId, const CheckAliveData& data) = 0;
        virtual void OnDownstreamState(unsigned sessionId, EState) = 0;
        virtual void OnDownstreamDisconnected(unsigned sessionId, EState, const Error&) = 0;
        virtual void OnDownstreamTrace(unsigned sessionId, ETraceType, StringView trace) = 0;

        void OnConnected(unsigned sessionId, EState state, const ConnectionInfo& data) override { OnDownstreamConnected(sessionId, state, data); }
        void On(unsigned sessionId, EState state, const ServiceDescription& data) override { OnDownstream(sessionId, state, data); }
        void On(unsigned sessionId, const NotificationData& data) override { OnDownstream(sessionId, data); }
        void On(unsigned sessionId, const CheckAliveData& data) override { OnDownstream(sessionId, data); }
        void OnState(unsigned sessionId, EState state) override { OnDownstreamState(sessionId, state); }
        void OnDisconnected(unsigned sessionId, EState state, const Error& data) override { OnDownstreamDisconnected(sessionId, state, data); }
        void OnTrace(unsigned sessionId, ETraceType type, StringView data) override { OnDownstreamTrace(sessionId, type, data); }
    };


    //======================== Downstream implementation =================================
    inline Downstream::Downstream(unsigned laneId, IDownstreamCallback& callback)
    {
        HermesDownstreamCallbacks callbacks{};

        callbacks.m_connectedCallback.m_pData = &callback;
        callbacks.m_connectedCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesConnectionInfo* pConnectionInfo)
        {
            static_cast<IDownstreamCallback*>(pCallback)->OnConnected(sessionId, ToCpp(state), 
                ToCpp(*pConnectionInfo));
        };

        callbacks.m_serviceDescriptionCallback.m_pData = &callback;
        callbacks.m_serviceDescriptionCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesServiceDescription* pServiceDescription)
        {
            static_cast<IDownstreamCallback*>(pCallback)->On(sessionId, ToCpp(state), ToCpp(*pServiceDescription));
        };

        callbacks.m_machineReadyCallback.m_pData = &callback;
        callbacks.m_machineReadyCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesMachineReadyData* pData)
        {
            static_cast<IDownstreamCallback*>(pCallback)->On(sessionId, ToCpp(state), ToCpp(*pData));
        };

        callbacks.m_revokeMachineReadyCallback.m_pData = &callback;
        callbacks.m_revokeMachineReadyCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesRevokeMachineReadyData* pData)
        {
            static_cast<IDownstreamCallback*>(pCallback)->On(sessionId, ToCpp(state), ToCpp(*pData));
        };

        callbacks.m_startTransportCallback.m_pData = &callback;
        callbacks.m_startTransportCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesStartTransportData* pData)
        {
            static_cast<IDownstreamCallback*>(pCallback)->On(sessionId, ToCpp(state), ToCpp(*pData));
        };

        callbacks.m_stopTransportCallback.m_pData = &callback;
        callbacks.m_stopTransportCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesStopTransportData* pData)
        {
            static_cast<IDownstreamCallback*>(pCallback)->On(sessionId, ToCpp(state), ToCpp(*pData));
        };

        callbacks.m_notificationCallback.m_pData = &callback;
        callbacks.m_notificationCallback.m_pCall = [](void* pCallback, uint32_t sessionId,
            const HermesNotificationData* pData)
        {
            static_cast<IDownstreamCallback*>(pCallback)->On(sessionId, ToCpp(*pData));
        };

        callbacks.m_checkAliveCallback.m_pData = &callback;
        callbacks.m_checkAliveCallback.m_pCall = [](void* pCallback, uint32_t sessionId,
            const HermesCheckAliveData* pData)
        {
            static_cast<IDownstreamCallback*>(pCallback)->On(sessionId, ToCpp(*pData));
        };

        callbacks.m_stateCallback.m_pData = &callback;
        callbacks.m_stateCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state)
        {
            static_cast<IDownstreamCallback*>(pCallback)->OnState(sessionId, ToCpp(state));
        };

        callbacks.m_disconnectedCallback.m_pData = &callback;
        callbacks.m_disconnectedCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesError* pError)
        {
            static_cast<IDownstreamCallback*>(pCallback)->OnDisconnected(sessionId, ToCpp(state), ToCpp(*pError));
        };

        callbacks.m_traceCallback.m_pData = &callback;
        callbacks.m_traceCallback.m_pCall = [](void* pCallback, unsigned sessionId, EHermesTraceType type,
            HermesStringView trace)
        {
            static_cast<IDownstreamCallback*>(pCallback)->OnTrace(sessionId, ToCpp(type), ToCpp(trace));
        };

        m_pImpl = ::CreateHermesDownstream(laneId, &callbacks);
    }

    inline void Downstream::Run()
    {
        ::RunHermesDownstream(m_pImpl);
    }

    template<class F> void Downstream::Post(F&& f)
    {
        HermesVoidCallback callback;
        callback.m_pData = std::make_unique<F>(std::forward<F>(f)).release();
        callback.m_pCall = [](void* pData)
        {
            auto upF = std::unique_ptr<F>(static_cast<F*>(pData));
            (*upF)();
        };
        ::PostHermesDownstream(m_pImpl, callback);
    }

    inline void Downstream::Enable(const DownstreamSettings& data)
    {
        auto apiData = ToC(data);
        ::EnableHermesDownstream(m_pImpl, &apiData);
    }

    inline void Downstream::Signal(unsigned sessionId, const ServiceDescription& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesDownstreamServiceDescription(m_pImpl, sessionId, &apiData);
    }

    inline void Downstream::Signal(unsigned sessionId, const BoardAvailableData& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesBoardAvailable(m_pImpl, sessionId, &apiData);
    }

    inline void Downstream::Signal(unsigned sessionId, const RevokeBoardAvailableData& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesRevokeBoardAvailable(m_pImpl, sessionId, &apiData);
    }

    inline void Downstream::Signal(unsigned sessionId, const TransportFinishedData& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesTransportFinished(m_pImpl, sessionId, &apiData);
    }

    inline void Downstream::Signal(unsigned sessionId, const NotificationData& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesDownstreamNotification(m_pImpl, sessionId, &apiData);
    }

    inline void Downstream::Signal(unsigned sessionId, const CheckAliveData& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesDownstreamCheckAlive(m_pImpl, sessionId, &apiData);
    }

    inline void Downstream::Reset(const NotificationData& data)
    {
        auto apiData = ToC(data);
        ::ResetHermesDownstream(m_pImpl, &apiData);
    }

    inline void Downstream::Disable(const NotificationData& data)
    {
        auto apiData = ToC(data);
        ::DisableHermesDownstream(m_pImpl, &apiData);
    }

    inline void Downstream::Stop()
    {
        ::StopHermesDownstream(m_pImpl);
    }

    //======================== Upstream implementation =================================
    inline Upstream::Upstream(unsigned laneId, IUpstreamCallback& callback)
    {
        HermesUpstreamCallbacks callbacks{};

        callbacks.m_connectedCallback.m_pData = &callback;
        callbacks.m_connectedCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesConnectionInfo* pConnectionInfo)
        {
            static_cast<IUpstreamCallback*>(pCallback)->OnConnected(sessionId, ToCpp(state),
                ToCpp(*pConnectionInfo));
        };

        callbacks.m_serviceDescriptionCallback.m_pData = &callback;
        callbacks.m_serviceDescriptionCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesServiceDescription* pServiceDescription)
        {
            static_cast<IUpstreamCallback*>(pCallback)->On(sessionId, ToCpp(state), ToCpp(*pServiceDescription));
        };

        callbacks.m_boardAvailableCallback.m_pData = &callback;
        callbacks.m_boardAvailableCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesBoardAvailableData* pData)
        {
            static_cast<IUpstreamCallback*>(pCallback)->On(sessionId, ToCpp(state), ToCpp(*pData));
        };

        callbacks.m_revokeBoardAvailableCallback.m_pData = &callback;
        callbacks.m_revokeBoardAvailableCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesRevokeBoardAvailableData* pData)
        {
            static_cast<IUpstreamCallback*>(pCallback)->On(sessionId, ToCpp(state), ToCpp(*pData));
        };

        callbacks.m_transportFinishedCallback.m_pData = &callback;
        callbacks.m_transportFinishedCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesTransportFinishedData* pData)
        {
            static_cast<IUpstreamCallback*>(pCallback)->On(sessionId, ToCpp(state), ToCpp(*pData));
        };

        callbacks.m_notificationCallback.m_pData = &callback;
        callbacks.m_notificationCallback.m_pCall = [](void* pCallback, uint32_t sessionId,
            const HermesNotificationData* pData)
        {
            static_cast<IUpstreamCallback*>(pCallback)->On(sessionId, ToCpp(*pData));
        };

        callbacks.m_checkAliveCallback.m_pData = &callback;
        callbacks.m_checkAliveCallback.m_pCall = [](void* pCallback, uint32_t sessionId,
            const HermesCheckAliveData* pData)
        {
            static_cast<IUpstreamCallback*>(pCallback)->On(sessionId, ToCpp(*pData));
        };

        callbacks.m_stateCallback.m_pData = &callback;
        callbacks.m_stateCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state)
        {
            static_cast<IUpstreamCallback*>(pCallback)->OnState(sessionId, ToCpp(state));
        };

        callbacks.m_disconnectedCallback.m_pData = &callback;
        callbacks.m_disconnectedCallback.m_pCall = [](void* pCallback, uint32_t sessionId, EHermesState state,
            const HermesError* pError)
        {
            static_cast<IUpstreamCallback*>(pCallback)->OnDisconnected(sessionId, ToCpp(state), ToCpp(*pError));
        };

        callbacks.m_traceCallback.m_pData = &callback;
        callbacks.m_traceCallback.m_pCall = [](void* pCallback, unsigned sessionId, EHermesTraceType type,
            HermesStringView trace)
        {
            static_cast<IUpstreamCallback*>(pCallback)->OnTrace(sessionId, ToCpp(type), ToCpp(trace));
        };

        m_pImpl = ::CreateHermesUpstream(laneId, &callbacks);
    }

    inline void Upstream::Run()
    {
        ::RunHermesUpstream(m_pImpl);
    }

    inline void Upstream::Enable(const UpstreamSettings& data)
    {
        auto apiData = ToC(data);
        ::EnableHermesUpstream(m_pImpl, &apiData);
    }

    template<class F> void Upstream::Post(F&& f)
    {
        HermesVoidCallback callback;
        callback.m_pData = std::make_unique<F>(std::forward<F>(f)).release();
        callback.m_pCall = [](void* pData)
        {
            auto upF = std::unique_ptr<F>(static_cast<F*>(pData));
            (*upF)();
        };
        ::PostHermesUpstream(m_pImpl, callback);
    }

    inline void Upstream::Signal(unsigned sessionId, const ServiceDescription& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesUpstreamServiceDescription(m_pImpl, sessionId, &apiData);
    }

    inline void Upstream::Signal(unsigned sessionId, const MachineReadyData& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesMachineReady(m_pImpl, sessionId, &apiData);
    }

    inline void Upstream::Signal(unsigned sessionId, const RevokeMachineReadyData& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesRevokeMachineReady(m_pImpl, sessionId, &apiData);
    }

    inline void Upstream::Signal(unsigned sessionId, const StartTransportData& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesStartTransport(m_pImpl, sessionId, &apiData);
    }

    inline void Upstream::Signal(unsigned sessionId, const StopTransportData& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesStopTransport(m_pImpl, sessionId, &apiData);
    }

    inline void Upstream::Signal(unsigned sessionId, const NotificationData& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesUpstreamNotification(m_pImpl, sessionId, &apiData);
    }

    inline void Upstream::Reset(const NotificationData& data)
    {
        auto apiData = ToC(data);
        ::ResetHermesUpstream(m_pImpl, &apiData);
    }

    inline void Upstream::Signal(unsigned sessionId, const CheckAliveData& data)
    {
        auto apiData = ToC(data);
        ::SignalHermesUpstreamCheckAlive(m_pImpl, sessionId, &apiData);
    }

    inline void Upstream::Disable(const NotificationData& data)
    {
        auto apiData = ToC(data);
        ::DisableHermesUpstream(m_pImpl, &apiData);
    }

    inline void Upstream::Stop()
    {
        ::StopHermesUpstream(m_pImpl);
    }

    //======================== ConfigurationService implementation =================================
    inline ConfigurationService::ConfigurationService(IConfigurationServiceCallback& callback):
        m_callback(callback)
    {
        HermesConfigurationServiceCallbacks callbacks;
        callbacks.m_connectedCallback.m_pData = this;
        callbacks.m_connectedCallback.m_pCall = [](void* pVoid, uint32_t sessionId, EHermesState, const HermesConnectionInfo* pInfo) 
        {
            static_cast<ConfigurationService*>(pVoid)->m_callback.OnConnected(sessionId, ToCpp(*pInfo));
        };

        callbacks.m_getConfigurationCallback.m_pData = this;
        callbacks.m_getConfigurationCallback.m_pCall = [](void* pVoid, uint32_t sessionId, 
            const HermesConnectionInfo* pConnectionInfo, const HermesGetConfigurationData*)
        {
            auto pThis = static_cast<ConfigurationService*>(pVoid);
            auto configuration = pThis->m_callback.OnGetConfiguration(sessionId, ToCpp(*pConnectionInfo));
            auto apiConfiguration = ToC(configuration);
            ::SignalHermesCurrentConfiguration(pThis->m_pImpl, sessionId, &apiConfiguration);
        };

        callbacks.m_setConfigurationCallback.m_pData = this;
        callbacks.m_setConfigurationCallback.m_pCall = [](void* pVoid, uint32_t sessionId,
            const HermesConnectionInfo* pConnectionInfo, const HermesSetConfigurationData* pConfiguration)
        {
            auto pThis = static_cast<ConfigurationService*>(pVoid);
            auto error = pThis->m_callback.OnSetConfiguration(sessionId, ToCpp(*pConnectionInfo), ToCpp(*pConfiguration));
            if (!error)
                return;

            NotificationData notification(ENotificationCode::eCONFIGURATION_ERROR, ESeverity::eERROR, error.m_text);
            auto apiNotification = ToC(notification);
            ::SignalHermesConfigurationNotification(pThis->m_pImpl, sessionId, &apiNotification);
        };

        callbacks.m_disconnectedCallback.m_pData = this;
        callbacks.m_disconnectedCallback.m_pCall = [](void* pVoid, uint32_t sessionId, EHermesState, const HermesError* pError)
        {
            static_cast<ConfigurationService*>(pVoid)->m_callback.OnDisconnected(sessionId, ToCpp(*pError));
        };

        callbacks.m_traceCallback.m_pData = this;
        callbacks.m_traceCallback.m_pCall = [](void* pVoid, unsigned sessionId, EHermesTraceType type,
            HermesStringView trace)
        {
            static_cast<ConfigurationService*>(pVoid)->m_callback.OnTrace(sessionId, ToCpp(type), ToCpp(trace));
        };

        m_pImpl = ::CreateHermesConfigurationService(&callbacks);
    }

    inline void ConfigurationService::Run()
    {
        ::RunHermesConfigurationService(m_pImpl);
    }

    template<class F> void ConfigurationService::Post(F&& f)
    {
        HermesVoidCallback callback;
        callback.m_pData = std::make_unique<F>(std::forward<F>(f)).release();
        callback.m_pCall = [](void* pData)
        {
            auto upF = std::unique_ptr<F>(static_cast<F*>(pData));
            (*upF)();
        };
        ::PostHermesConfigurationService(m_pImpl, callback);
    }

    inline void ConfigurationService::Enable(const ConfigurationServiceSettings& data)
    {
        auto apiData = ToC(data);
        ::EnableHermesConfigurationService(m_pImpl, &apiData);
    }

    inline void ConfigurationService::Disable(const NotificationData& data)
    {
        auto apiData = ToC(data);
        ::DisableHermesConfigurationService(m_pImpl, &apiData);
    }

    inline void ConfigurationService::Stop()
    {
        ::StopHermesConfigurationService(m_pImpl);
    }

    //================= Configuration client implementation =======================
    inline std::pair<CurrentConfigurationData, Error> GetConfiguration(StringView hostName, unsigned timeoutInSeconds,
        ITraceCallback* pTraceCallback)
    {
        HermesGetConfigurationCallbacks callbacks{};

        callbacks.m_traceCallback.m_pData = pTraceCallback;
        callbacks.m_traceCallback.m_pCall = [](void* pTraceCallback, unsigned sessionId, EHermesTraceType type,
            HermesStringView trace)
        {
            if (!pTraceCallback)
                return;
            static_cast<ITraceCallback*>(pTraceCallback)->OnTrace(sessionId, ToCpp(type), ToCpp(trace));
        };

        CurrentConfigurationData configuration;
        callbacks.m_currentConfigurationCallback.m_pData = &configuration;
        callbacks.m_currentConfigurationCallback.m_pCall = [](void* pData, uint32_t /*sessionId*/,
            const HermesCurrentConfigurationData* pConfiguration)
        {
            *static_cast<CurrentConfigurationData*>(pData) = ToCpp(*pConfiguration);
        };

        Error error;
        callbacks.m_errorCallback.m_pData = &error;
        callbacks.m_errorCallback.m_pCall = [](void* pData, const HermesError* pError)
        {
            if (!pData)
                return;
            *static_cast<Error*>(pData) = ToCpp(*pError);
        };

        ::GetHermesConfiguration(ToC(hostName), timeoutInSeconds, &callbacks);
        return{configuration, error};
    }

    inline Error Hermes::SetConfiguration(StringView hostName, const SetConfigurationData& configuration,
        unsigned timeoutInSeconds,
        Hermes::CurrentConfigurationData* out_pConfiguration, // resulting configuration
        std::vector<NotificationData>* out_pNotifications, // out: notification data
        ITraceCallback* pTraceCallback)
    {
        // reset the output by default
        if (out_pConfiguration)
        {
            *out_pConfiguration = Hermes::CurrentConfigurationData();
        }
        if (out_pNotifications)
        {
            out_pNotifications->clear();
        }

        HermesSetConfigurationCallbacks callbacks{};

        callbacks.m_traceCallback.m_pData = pTraceCallback;
        callbacks.m_traceCallback.m_pCall = [](void* pTraceCallback, unsigned sessionId, EHermesTraceType type,
            HermesStringView trace)
        {
            if (!pTraceCallback)
                return;
            static_cast<ITraceCallback*>(pTraceCallback)->OnTrace(sessionId, ToCpp(type), ToCpp(trace));
        };

        callbacks.m_notificationCallback.m_pData = out_pNotifications;
        callbacks.m_notificationCallback.m_pCall = [](void* pData, uint32_t /*sessionId*/, const HermesNotificationData* pNotification)
        {
            if (!pData)
                return;
            static_cast<std::vector<NotificationData>*>(pData)->emplace_back(ToCpp(*pNotification));
        };

        Error error;
        callbacks.m_errorCallback.m_pData = &error;
        callbacks.m_errorCallback.m_pCall = [](void* pData, const HermesError* pError)
        {
            *static_cast<Error*>(pData) = ToCpp(*pError);
        };

        callbacks.m_currentConfigurationCallback.m_pData = out_pConfiguration;
        callbacks.m_currentConfigurationCallback.m_pCall = [](void* pData, uint32_t /*sessionId*/,
            const HermesCurrentConfigurationData* pConfiguration)
        {
            if (!pData)
                return;
            *static_cast<CurrentConfigurationData*>(pData) = ToCpp(*pConfiguration);
        };

        auto apiConfiguration = ToC(configuration);
        ::SetHermesConfiguration(ToC(hostName), &apiConfiguration, timeoutInSeconds, &callbacks);
        return error;
    }


}



