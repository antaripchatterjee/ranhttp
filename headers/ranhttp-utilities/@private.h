#ifndef __RANHTTP_MODULE_RANHTTP_UTILITIES_PRIVATE_H__
#define __RANHTTP_MODULE_RANHTTP_UTILITIES_PRIVATE_H__

#include <ctype.h>
#include <stdint.h>


#include "../common.h"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

static inline
uint8_t ranhttp__utility_hex_to_byte(char a, char b, int* error) {
    if (!isxdigit(a) || !isxdigit(b)) {
        if (error) {
            *error = 1;
        }
        return (uint8_t) 0xFF;
    }
    uint8_t out = (uint8_t)((isdigit(a) ? a - '0' : tolower(a) - 'a' + 10) << 4 |
                     (isdigit(b) ? b - '0' : tolower(b) - 'a' + 10));
    return out;
}


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __RANHTTP_MODULE_RANHTTP_UTILITIES_PRIVATE_H__