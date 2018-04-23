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

namespace Hermes
{

    // a wrapper for calling the callbacks
    template<class F> // F assumed to have the member m_pCall and m_pData;
    struct ApiCallback
    {
        F m_f{nullptr, nullptr}; // default: callback is nullptr
        ApiCallback() = default;
        ApiCallback(const F& f) : m_f(f) {}

        template<class... Ts>
        void operator()(Ts&&... args) const
        {
            if (!m_f.m_pCall)
                return;

            m_f.m_pCall(m_f.m_pData, std::forward<Ts>(args)...);
        }

        explicit operator bool() const { return m_f.m_pCall ? true : false; }
    };



}