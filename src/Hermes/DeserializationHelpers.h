#pragma once

#include "IService.h"
#include "StringSpan.h"

#ifdef _WINDOWS
#include "pugixml/pugixml.hpp"
#else
#include "pugixml.hpp"
#endif

#include <boost/variant.hpp>

namespace Hermes
{

    template<class... DataTs>
    boost::variant<boost::blank, Error, DataTs...> Deserialize(StringSpan message);

    StringSpan TakeMessage(StringSpan& buffer);
    Error ParseXmlMessage(StringSpan message, pugi::xml_document*, pugi::xml_node*);







}
