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
// 0-99: Debug including file and line number
// >= 100: Debug without file and line number
#endif // DEBUG_LEVEL

#if defined(DEBUG_MODE) && DEBUG_MODE >= DEBUG_LEVEL
#if DEBUG_LEVEL < 100
#define FILE_LINE_INFO_FMT "%s:%d: "
#define FILE_LINE_INFO_ARGS ,__FILE__,__LINE__
#else
#define FILE_LINE_INFO_FMT
#define FILE_LINE_INFO_ARGS
#endif // DEBUG_LEVEL >= 100

#include <stdio.h>
#define DEBUG_LOG(fmt, ...) \
    do { \
        fprintf(stderr, "[DEBUG%d] " FILE_LINE_INFO_FMT fmt "\n", DEBUG_LEVEL FILE_LINE_INFO_ARGS __VA_OPT__(,) ##__VA_ARGS__); \
    } while (0)
#else
#define DEBUG_LOG(fmt, ...)
#endif // DEBUG_MODE

#endif // __RANHTTP_COMMON_H__
