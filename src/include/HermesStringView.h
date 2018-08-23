// Copyright (c) ASM Assembly Systems GmbH & Co. KG
//
// C interface helper for Hermes
//
#ifndef HERMESSTRINGVIEW_H
#define HERMESSTRINGVIEW_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    // Not part of The Hermes Standard, but used extensively: a non-owning string type, in the spirit of std::string_view.
    // This relieves us from the need to terminate all strings with \0.
    // Note that all the C interface structures are non-owning and need to be backed up by actual storage.
    struct HermesStringView
    {
        const char* m_pData; // if nullptr then we have no string at all - not even an empty string
        size_t m_size;
    };

#ifdef __cplusplus
}
#endif

#endif //HERMESSTRINGVIEW_H

