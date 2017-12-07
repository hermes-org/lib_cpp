// Copyright (c) ASM Assembly Systems GmbH & Co. KG
//
// in order to avoid dependencies on boost and >= C++11, we roll out our own stripped down optional class
#pragma once

#include <cassert>

namespace Hermes
{
    template<class T>
    class Optional
    {
    public:
        Optional() : m_hasValue(false) {}

        Optional(const T& value) : m_hasValue(true), m_value(value) {}

        // for the time being, avoid C++11 and its variadic templates for better compatibility. Up to two params should do for Hermes ...
        Optional& emplace() { m_value = T(); m_hasValue = true; return *this; }
        template<class U>
        Optional& emplace(const U& u) { m_value = T(u); m_hasValue = true; return *this; }
        template<class U1, class U2>
        Optional& emplace(const U1& u1, const U2& u2) { m_value = T(u1, u2); m_hasValue = true; return *this; }

        Optional& operator=(const T& value)
        {
            m_hasValue = true;
            m_value = value;
            return *this;
        }

        const T* operator->() const { assert(m_hasValue);  return &m_value; }
        T* operator->() { assert(m_hasValue);  return &m_value; }

        const T& operator*() const { assert(m_hasValue);  return m_value; }
        T& operator*() { assert(m_hasValue);  return m_value; }

        typedef bool Optional::*UnspecifiedBoolType;
        operator UnspecifiedBoolType() const { return m_hasValue ? &Optional::m_hasValue : 0; }
        bool operator!() const { return !m_hasValue; }
        bool has_value() const { return m_hasValue; }
        
        const T& value_or(const T& other) const { return m_hasValue ? m_value : other; }
        T& value_or(T& other) { return m_hasValue ? m_value : other; }

        void swap(Optional& rhs)
        {
            using std::swap;
            swap(m_hasValue, rhs.m_value);
            swap(m_value, rhs.m_value);
        }
        friend void swap(Optional& lhs, Optional& rhs) { lhs.swap(rhs); }

        void reset() 
        { 
            m_hasValue = false;
            m_value = T();
        }

        friend bool operator==(const Optional& lhs, const Optional& rhs)
        {
            if (lhs.m_hasValue && rhs.m_hasValue)
                return lhs.m_value == rhs.m_value;
            
            return lhs.m_hasValue == rhs.m_hasValue;
        }
        friend bool operator!=(const Optional& lhs, const Optional& rhs) { return !operator==(lhs, rhs); }

        template<class S>
        friend S& operator<<(S& s, const Optional& o)
        {
            if (o.m_hasValue)
            {
                s << o.m_value;
            }
            else
            {
                s << "<empty>";
            }
            return s;
        }

    private:

        bool m_hasValue;
        T m_value;
    };
}
