#pragma once

#include "IService.h"
#include "MessageSerialization.h"
#include "StringSpan.h"

#ifdef _WINDOWS
#include "pugixml/pugixml.hpp"
#else
#include "pugixml.hpp"
#endif

#include <functional>
#include <map>

namespace Hermes
{
    struct Error;

    class MessageDispatcher
    {
    public:

        explicit MessageDispatcher(unsigned sessionId, IAsioService& asioService) :
            m_sessionId(sessionId),
            m_service(asioService)
        {}

        MessageDispatcher(const MessageDispatcher&) = delete;
        MessageDispatcher& operator=(const MessageDispatcher&) = delete;

        ~MessageDispatcher() = default;

        using Callback = std::function<Error(pugi::xml_node)>;

        void Add(StringView tag, Callback&& callback);
        Error Dispatch(StringSpan input);

        template<class DataT, class CallbackT>
        void Add(CallbackT&& callback)
        {
            Add(SerializationTraits<DataT>::cTAG_VIEW,
                [this, callback = std::forward<CallbackT>(callback)](pugi::xml_node xmlNode)->Error
            {
                DataT data;
                auto error = Deserialize(xmlNode, data);
                if (error)
                    return error;
                m_service.Log(m_sessionId, SerializationTraits<DataT>::cTAG_VIEW, ':', data);
                callback(data);
                return{};
            });
        }

    private:

        std::string m_buffer;
        std::map<std::string, std::function<Error(pugi::xml_node)>, std::less<>> m_map;
        unsigned m_sessionId;
        IAsioService& m_service;
    };

}
