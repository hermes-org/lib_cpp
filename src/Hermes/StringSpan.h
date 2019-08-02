// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include <HermesStringView.hpp>
// while we have not got std::string_view at our disposal, we make our own:

namespace Hermes
{
    class StringSpan
    {
    public:
        constexpr StringSpan() = default;
        constexpr StringSpan(char* pData, std::size_t size) : m_pData(pData), m_size(size) {}
        StringSpan(std::string& rhs) : m_pData(const_cast<char*>(rhs.data())), m_size(rhs.size()) {}

//        operator std::string() const { return std::string(m_pData, m_size); }
        operator StringView() const { return{m_pData, m_size}; }

        constexpr char* data() const { return m_pData; }
        constexpr std::size_t size() const { return m_size; }
        constexpr std::size_t length() const { return m_size; }
        constexpr bool empty() const { return m_size == 0U; }
        constexpr StringSpan substr(std::size_t pos, std::size_t count = std::string::npos) const { return{m_pData + pos, std::min(count, m_size - pos)}; }

        std::size_t find(char c, std::size_t pos = 0U) const { return StringView(*this).find(c, pos); }
        std::size_t find(StringView v) const { return StringView(*this).find(v); }

        int compare(StringView rhs) { return StringView(*this).compare(rhs); }
        int compare(std::size_t pos, std::size_t count, StringView rhs) { return StringView(*this).compare(pos, count, rhs); }

        friend std::ostream& operator<<(std::ostream& os, StringSpan sv)
        {
            os.write(sv.m_pData, sv.m_size);
            return os;
        }

    private:
        char* m_pData = nullptr;
        std::size_t m_size = 0U;
    };

    inline bool operator==(StringSpan lhs, StringSpan rhs)
    {
        return lhs.compare(rhs) == 0;
    }

    inline bool operator!=(StringSpan lhs, StringSpan rhs)
    {
        return !operator==(lhs, rhs);
    }
}
