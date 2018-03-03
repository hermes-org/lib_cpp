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

// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include <cstring>
// while we have not got std::string_view at our disposal, we make our own:

namespace Hermes
{
    class StringView
    {
    public:
        StringView() = default;
        StringView(const char* pStr) : m_pData(pStr), m_size(::strlen(pStr)) {}
        StringView(const char* pData, std::size_t size) : m_pData(pData), m_size(size) {}
        StringView(const std::string& str) : m_pData(str.data()), m_size(str.size()) {}
        StringView& operator=(const std::string& str)
        {
            m_pData = str.data();
            m_size = str.size();
            return *this;
        }
        StringView& operator=(const char* pStr)
        {
            m_pData = pStr;
            m_size = ::strlen(pStr);
            return *this;
        }

        operator std::string() const { return std::string(m_pData, m_size); }

        const std::string cpp_str() const { return std::string(m_pData); }
        const char* data() const { return m_pData; }
        std::size_t size() const { return m_size; }
        std::size_t length() const { return m_size; }
        bool empty() const { return m_size == 0U; }

        int compare(StringView rhs)
        {
            auto minSize = m_size < rhs.m_size ? m_size : rhs.m_size;
            auto cmp = std::char_traits<char>::compare(m_pData, rhs.m_pData, minSize);

            if (cmp)
                return cmp;
            if (m_size < rhs.m_size)
                return -1;
            if (rhs.m_size < m_size)
                return 1;
            return 0;
        }

        friend std::ostream& operator<<(std::ostream& os, StringView sv)
        {
            os.write(sv.m_pData, sv.m_size);
            return os;
        }

    private:
        const char* m_pData = nullptr;
        std::size_t m_size = 0U;
    };

    inline bool operator==(StringView lhs, StringView rhs)
    {
        return lhs.compare(rhs) == 0;
    }

    inline bool operator!=(StringView lhs, StringView rhs)
    {
        return !operator==(lhs, rhs);
    }
}
