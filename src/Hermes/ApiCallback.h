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