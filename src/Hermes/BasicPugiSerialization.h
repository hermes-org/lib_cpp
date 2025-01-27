#pragma once

#include <HermesStringView.hpp>
#include <HermesData.hpp>

#ifdef _WINDOWS
#include "pugixml/pugixml.hpp"
#else
# include "pugixml.hpp"
#endif

#include <iomanip>

namespace Hermes
{
    template<class T, class EnableT = void>
    struct PugiSerializer;

    template<> struct PugiSerializer<std::string> 
    {
        using AsAttributeTag = int;

        static void WriteAttribute(pugi::xml_attribute attr, const std::string& value) 
        { 
            attr.set_value(value.c_str()); 
        }
        static void ReadAttribute(pugi::xml_attribute attr, Error&, std::string& value)
        {
            value = attr.as_string();
        }
    };

    template<> struct PugiSerializer<int>
    {
        using AsAttributeTag = int;

        static void WriteAttribute(pugi::xml_attribute attr, int value)
        {
            attr.set_value(value);
        }
        static void ReadAttribute(pugi::xml_attribute attr, Error&, int& value)
        {
            value = attr.as_int();
        }
    };

    template<> struct PugiSerializer<unsigned int>
    {
        using AsAttributeTag = int;

        static void WriteAttribute(pugi::xml_attribute attr, unsigned int value)
        {
            attr.set_value(value);
        }
        static void ReadAttribute(pugi::xml_attribute attr, Error&, unsigned int& value)
        {
            value = attr.as_uint();
        }
    };

    template<> struct PugiSerializer<unsigned short>
    {
        using AsAttributeTag = int;

        static void WriteAttribute(pugi::xml_attribute attr, unsigned short value)
        {
            attr.set_value(value);
        }
        static void ReadAttribute(pugi::xml_attribute attr, Error&, unsigned short& value)
        {
            value = static_cast<unsigned short>(attr.as_int());
        }
    };

    template<> struct PugiSerializer<double>
    {
        using AsAttributeTag = int;

        static void WriteAttribute(pugi::xml_attribute attr, double value)
        {
            // Standard states that we should have 3 trailing decimals
            std::stringstream stream;
            stream << std::fixed << std::setprecision(3) << value;
            attr.set_value(stream.str().c_str());
        }
        static void ReadAttribute(pugi::xml_attribute attr, Error&, double& value)
        {
            value = attr.as_double();
        }
    };

    template<class E> struct PugiSerializer<E, std::enable_if_t<std::is_enum<E>::value>>
    {
        using AsAttributeTag = int;

        static void WriteAttribute(pugi::xml_attribute attr, E value)
        {
            attr.set_value(static_cast<int>(value));
        }
        static void ReadAttribute(pugi::xml_attribute attr, Error&, E& value)
        {
            int i  = attr.as_int();
            if (0 <= i && i < static_cast<int>(size(E())))
            {
                value = static_cast<E>(i);
                return;
            }
            value = E{}; // all our enums (must!) have a sensible default value
        }
    };

    
    void Serialize(pugi::xml_node parent, const char* name, const std::string& value)
    {
        PugiSerializer<std::string>::WriteAttribute(parent.append_attribute(name), value);
    }

    template<class T>
    void Serialize(pugi::xml_node parent, const char* name, T value, typename PugiSerializer<T>::AsAttributeTag = 0)
    {
        PugiSerializer<T>::WriteAttribute(parent.append_attribute(name), value);
    }

    template<class T>
    void Serialize(pugi::xml_node parent, const char* name, const T& value, typename PugiSerializer<T>::AsElementTag = 0)
    {
        PugiSerializer<T>::WriteElement(parent.append_child(name), value);
    }

    template<class T>
    void Serialize(pugi::xml_node parent, const char* name, const Optional<T>& value)
    {
        if (!value)
            return;

        Serialize(parent, name, *value);
    }

    template<class T>
    void Deserialize(pugi::xml_node parent, const char* name, Error& error, T& value, typename PugiSerializer<T>::AsAttributeTag = 0)
    {
        auto attribute = parent.attribute(name);
        if (attribute)
            return PugiSerializer<T>::ReadAttribute(attribute, error, value);

        error = {EErrorCode::ePEER_ERROR, std::string("missing attribute ") + name};
    }

    template<class T>
    void Deserialize(pugi::xml_node parent, const char* name, Error& error, T& value, typename PugiSerializer<T>::AsElementTag = 0)
    {
        auto element = parent.child(name);
        if (element)
            return PugiSerializer<T>::ReadElement(element, error, value);

        error = {EErrorCode::ePEER_ERROR, std::string("missing element ") + name};
    }

    template<class T>
    void Deserialize(pugi::xml_node parent, const char* name, Error& error, Optional<T>& value, typename PugiSerializer<T>::AsAttributeTag = 0)
    {
        auto attribute = parent.attribute(name);
        if (!attribute)
            return;

        PugiSerializer<T>::ReadAttribute(attribute, error, *value.emplace());
    }

    template<class T>
    void Deserialize(pugi::xml_node parent, const char* name, Error& error, Optional<T>& value, typename PugiSerializer<T>::AsElementTag = 0)
    {
        auto element = parent.child(name);
        if (!element)
            return;

        PugiSerializer<T>::ReadElement(element, error, *value.emplace());
    }

}
