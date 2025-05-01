#ifndef __RANHTTP_COMMON_H__
#define __RANHTTP_COMMON_H__


#if defined(_WIN32) && defined(BUILD_TYPE) && BUILD_TYPE == 3
#define DLLEXPORT __declspec(dllexport)
#elif defined(__GNUC__) && defined(BUILD_TYPE) && BUILD_TYPE == 3
#define DLLEXPORT __attribute__((visibility("default")))
#else
#define DLLEXPORT
#endif

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif // DEBUG_LEVEL

#if defined(DEBUG_MODE) && DEBUG_MODE >= DEBUG_LEVEL
#include <stdio.h>
#define DEBUG_LOG(fmt, ...) \
    do { \
        fprintf(stderr, "[DEBUG%d] %s:%d: " fmt "\n", DEBUG_LEVEL, __FILE__, __LINE__ __VA_OPT__(,) ##__VA_ARGS__); \
    } while (0)
#else
#define DEBUG_LOG(fmt, ...)
#endif // DEBUG_MODE

#endif // __RANHTTP_COMMON_H__
