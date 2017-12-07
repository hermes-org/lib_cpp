// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include <sstream>

// Build a string from a variadic parameter list:
namespace Hermes
{
    inline void BuildString_(std::ostringstream&) {}

    template<class T, class... Ts>
    void BuildString_(std::ostringstream& oss, const T& head, const Ts&... tail)
    {
        oss << head;
        BuildString_(oss, tail...);
    }

    template<class... Ts>
    std::string BuildString(const Ts&... params)
    {
        std::ostringstream oss;
        BuildString_(oss, params...);
        return oss.str();
    }

}
