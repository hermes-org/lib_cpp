#pragma once

#include <HermesStringView.hpp>

#ifdef _WINDOWS
#include "pugixml/pugixml.hpp"
#else
#include "pugixml.hpp"
#endif

namespace Hermes
{
    class SenderEnvelope
    {
    public:
        SenderEnvelope(StringView tag);
        SenderEnvelope(const SenderEnvelope&) = delete;
        SenderEnvelope& operator=(const SenderEnvelope&) = delete;
        ~SenderEnvelope() = default;

        std::string ToXmlString() const;
        pugi::xml_node& DataNode() { return m_dataNode; }

    private:
        pugi::xml_document m_domDocument;
        pugi::xml_node m_dataNode;
    };


 }
