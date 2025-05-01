#ifndef __RANHTTP_TEST_HELPER_H__
#define __RANHTTP_TEST_HELPER_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define TEST_STARTED printf("TEST STARTED [%s::%s:%d]\n", __FILE__, __FUNCTION__, __LINE__);
#define TEST_ENDED printf("TEST ENDED [%s::%s:%d]\n", __FILE__, __FUNCTION__, __LINE__);
#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            DEBUG_LOG("Assertion failed: %s, file %s, line %d\n", #condition, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_EQ(str1, str2) \
    do { \
        if (strcmp(str1, str2) != 0) { \
            DEBUG_LOG("String assertion failed: %s == %s, file %s, line %d\n", str1, str2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_NEQ(str1, str2) \
    do { \
        if (strcmp(str1, str2) == 0) { \
            DEBUG_LOG("String assertion failed: %s != %s, file %s, line %d\n", str1, str2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_INT_EQ(int1, int2) \
    do { \
        if ((int1) != (int2)) { \
            DEBUG_LOG("Integer assertion failed: %d == %d, file %s, line %d\n", int1, int2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_INT_NEQ(int1, int2) \
    do { \
        if ((int1) == (int2)) { \
            DEBUG_LOG("Integer assertion failed: %d != %d, file %s, line %d\n", int1, int2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            DEBUG_LOG("Pointer assertion failed: %s is not NULL, file %s, line %d\n", #ptr, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            DEBUG_LOG("Pointer assertion failed: %s is NULL, file %s, line %d\n", #ptr, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            DEBUG_LOG("Boolean assertion failed: %s is not true, file %s, line %d\n", #condition, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            DEBUG_LOG("Boolean assertion failed: %s is not false, file %s, line %d\n", #condition, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_PTR_EQ(ptr1, ptr2) \
    do { \
        if ((ptr1) != (ptr2)) { \
            DEBUG_LOG("Pointer assertion failed: %s == %s, file %s, line %d\n", #ptr1, #ptr2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_PTR_NEQ(ptr1, ptr2) \
    do { \
        if ((ptr1) == (ptr2)) { \
            DEBUG_LOG("Pointer assertion failed: %s != %s, file %s, line %d\n", #ptr1, #ptr2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_SIZE_EQ(size1, size2) \
    do { \
        if ((size1) != (size2)) { \
            DEBUG_LOG("Size assertion failed: %zu == %zu, file %s, line %d\n", size1, size2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_SIZE_NEQ(size1, size2) \
    do { \
        if ((size1) == (size2)) { \
            DEBUG_LOG("Size assertion failed: %zu != %zu, file %s, line %d\n", size1, size2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_SIZE_GT(size1, size2) \
    do { \
        if ((size1) <= (size2)) { \
            DEBUG_LOG("Size assertion failed: %zu > %zu, file %s, line %d\n", size1, size2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_SIZE_LT(size1, size2) \
    do { \
        if ((size1) >= (size2)) { \
            DEBUG_LOG("Size assertion failed: %zu < %zu, file %s, line %d\n", size1, size2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_SIZE_GE(size1, size2) \
    do { \
        if ((size1) < (size2)) { \
            DEBUG_LOG("Size assertion failed: %zu >= %zu, file %s, line %d\n", size1, size2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_SIZE_LE(size1, size2) \
    do { \
        if ((size1) > (size2)) { \
            DEBUG_LOG("Size assertion failed: %zu <= %zu, file %s, line %d\n", size1, size2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_GT(str1, str2) \
    do { \
        if (strcmp(str1, str2) <= 0) { \
            DEBUG_LOG("String assertion failed: %s > %s, file %s, line %d\n", str1, str2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_LT(str1, str2) \
    do { \
        if (strcmp(str1, str2) >= 0) { \
            DEBUG_LOG("String assertion failed: %s < %s, file %s, line %d\n", str1, str2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_GE(str1, str2) \
    do { \
        if (strcmp(str1, str2) < 0) { \
            DEBUG_LOG("String assertion failed: %s >= %s, file %s, line %d\n", str1, str2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_LE(str1, str2) \
    do { \
        if (strcmp(str1, str2) > 0) { \
            DEBUG_LOG("String assertion failed: %s <= %s, file %s, line %d\n", str1, str2, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_CONTAINS(str, substr) \
    do { \
        if (strstr(str, substr) == NULL) { \
            DEBUG_LOG("String assertion failed: %s contains %s, file %s, line %d\n", str, substr, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_NOT_CONTAINS(str, substr) \
    do { \
        if (strstr(str, substr) != NULL) { \
            DEBUG_LOG("String assertion failed: %s does not contain %s, file %s, line %d\n", str, substr, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_STARTS_WITH(str, prefix) \
    do { \
        if (strncmp(str, prefix, strlen(prefix)) != 0) { \
            DEBUG_LOG("String assertion failed: %s starts with %s, file %s, line %d\n", str, prefix, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_ENDS_WITH(str, suffix) \
    do { \
        size_t str_len = strlen(str); \
        size_t suffix_len = strlen(suffix); \
        if (str_len < suffix_len || strcmp(str + str_len - suffix_len, suffix) != 0) { \
            DEBUG_LOG("String assertion failed: %s ends with %s, file %s, line %d\n", str, suffix, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_EMPTY(str) \
    do { \
        if (strlen(str) != 0) { \
            DEBUG_LOG("String assertion failed: %s is not empty, file %s, line %d\n", str, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_NOT_EMPTY(str) \
    do { \
        if (strlen(str) == 0) { \
            DEBUG_LOG("String assertion failed: %s is empty, file %s, line %d\n", str, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_NULL(str) \
    do { \
        if (str != NULL) { \
            DEBUG_LOG("String assertion failed: %s is not NULL, file %s, line %d\n", str, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_NOT_NULL(str) \
    do { \
        if (str == NULL) { \
            DEBUG_LOG("String assertion failed: %s is NULL, file %s, line %d\n", str, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)
#define TEST_ASSERT_STR_NULL_OR_EMPTY(str) \
    do { \
        if (str != NULL && strlen(str) != 0) { \
            DEBUG_LOG("String assertion failed: %s is not NULL or empty, file %s, line %d\n", str, __FILE__, __LINE__); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)


#endif // __RANHTTP_TEST_HELPER_H__