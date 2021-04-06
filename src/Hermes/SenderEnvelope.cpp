#include "stdafx.h"

#include "SenderEnvelope.h"

#include <chrono>
#include <iomanip>
#include <time.h>
#include <sstream>

namespace Hermes
{
    namespace
    {
        std::string GenerateTimestamp_()
        {
            auto now = std::chrono::system_clock::now();
            auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
            auto fraction = now - seconds;
            time_t cnow = std::chrono::system_clock::to_time_t(now);
            tm local_tm;
#ifdef _WINDOWS
            localtime_s(&local_tm, &cnow);
#else
            localtime_r(&cnow, &local_tm);
#endif
            std::ostringstream oss;
            oss << std::put_time(&local_tm, "%Y-%m-%dT%H:%M:%S.");
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);
            oss << std::setw(3) << std::setfill('0') << milliseconds.count();
            return oss.str();
        }
    }
    SenderEnvelope::SenderEnvelope(StringView tag)
    {
        auto root = m_domDocument.append_child("Hermes");
        root.append_attribute("Timestamp").set_value(GenerateTimestamp_().c_str());
        m_dataNode = root.append_child(tag.data());
    }

    std::string SenderEnvelope::ToXmlString() const
    {
        std::ostringstream ss;
        m_domDocument.save(ss, PUGIXML_TEXT(" "), pugi::format_indent | pugi::format_no_declaration, pugi::encoding_utf8);
        return ss.str();
    }
}


