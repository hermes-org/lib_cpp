#include "stdafx.h"

#include "MessageDispatcher.h"

namespace
{
    const std::string cHERMES = "Hermes";
    const std::size_t cHERMES_SIZE = 6U;
    const std::string cHERMES_END_TAG = "</Hermes";
}

namespace Hermes
{
    StringSpan TakeMessage_(StringSpan& buffer)
    {
        // find "<Hermes" removing everything that is encountered until then:
        for (;;)
        {
            std::size_t startIndex = buffer.find('<');
            if (startIndex == std::string::npos)
            {
                buffer = StringSpan{};
                return{};
            }
            if (startIndex != 0)
            {
                buffer = StringSpan{buffer.data() + startIndex, buffer.size() - startIndex};
            }

            if (buffer.size() < 1U + cHERMES_SIZE)
                return{};

            if (buffer.compare(1U, cHERMES_SIZE, cHERMES) == 0)
                break;

            buffer = buffer.substr(1U);
        }

        // find the ending "</Hermes"
        std::size_t endTagIndex = buffer.find(cHERMES_END_TAG);
        if (endTagIndex == std::string::npos)
            return{};

        // ... and the final '>'
        std::size_t endIndex = buffer.find('>', endTagIndex + cHERMES_END_TAG.size());
        if (endIndex == std::string::npos)
            return{};

        auto msgBuffer = buffer.substr(0U, endIndex + 1);
        buffer = buffer.substr(endIndex + 1);

        return msgBuffer;
    }

    Error ParseXmlMessage_(StringSpan message, pugi::xml_document* pDoc, pugi::xml_node* pNode)
    {
        auto parseResult = pDoc->load_buffer_inplace(message.data(), message.size(), pugi::parse_default, pugi::encoding_utf8);
        if (!parseResult)
            return Error{EErrorCode::eCLIENT_ERROR,
            std::string(parseResult.description()) + " at offset " + std::to_string(parseResult.offset)};
        pugi::xml_node envelopeNode = pDoc->first_child();
        *pNode = envelopeNode.first_child();
        if (!*pNode)
            return Error{EErrorCode::eCLIENT_ERROR, std::string("Missing message type node")};

        return{};
    }

    void MessageDispatcher::Add(StringView tag, Callback&& callback)
    {
        m_map.emplace(tag, std::move(callback));
    }

    Error MessageDispatcher::Dispatch(StringSpan xmlData)
    {
        if (!m_buffer.empty())
        {
            m_buffer.append(xmlData.data(), xmlData.size());
            xmlData = m_buffer;
        }

        for (StringSpan xmlMessage = TakeMessage_(xmlData); !xmlMessage.empty(); xmlMessage = TakeMessage_(xmlData))
        {
            pugi::xml_document xmlDocument;
            pugi::xml_node dataNode;
            if (auto error = ParseXmlMessage_(xmlMessage, &xmlDocument, &dataNode))
                return error;

            StringView tag = dataNode.name();
            auto itFound = m_map.find(tag.data());
            if (itFound == m_map.end())
            {
                m_service.Warn(m_sessionId, "Unexpected message ", tag);
                continue;
            }
            if (auto error = (itFound->second)(dataNode))
                return error;
        }

        if (xmlData.size() > cMAX_MESSAGE_SIZE)
            return{EErrorCode::ePEER_ERROR, "Maximum message size exceeded"};

        // do the housekeeping for the buffer and the view into it:
        if (xmlData.empty())
        {
            m_buffer.clear();
        }
        else if (m_buffer.empty())
        {
            m_buffer.assign(xmlData.data(), xmlData.size());
        }
        else // view and buffer overlap
        {
            m_buffer.erase(0U, xmlData.data() - m_buffer.data());
        }
        return{};
    }

}

