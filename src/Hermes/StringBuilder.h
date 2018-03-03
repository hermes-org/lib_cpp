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
