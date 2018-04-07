// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include <Hermes.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace
{
    namespace Detail
    {
        char ToChar(int i)
        {
            return static_cast<char>(i <= 9 ? '0' + i : 'a' - 10 + i);
        }
    }
}

void GenerateHermesGuid(char* pBuffer)
{
    auto uuid = boost::uuids::random_generator()();
    auto i = 0U;
    for (auto it = uuid.begin(); it != uuid.end(); ++it, ++i)
    {
        *pBuffer++ = Detail::ToChar(((*it) >> 4) & 0x0F);
        *pBuffer++ = Detail::ToChar(*it & 0x0F);
        if (i == 3 || i == 5 || i == 7 || i == 9)
        {
            *pBuffer++ = '-';
        }
    }
}



