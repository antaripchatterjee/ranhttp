#ifndef __RANHTTP_MODULE_REQUEST_PARSER_UTIL_H__
#define __RANHTTP_MODULE_REQUEST_PARSER_UTIL_H__

#include "request-parser/@error.h"
#include "request-parser/@types.h"

#include "../common.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_read_line_0(ranhttp__request_t *request, const char* line_0);

DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_read_header_string(ranhttp__request_t *request, const char* header_string, const size_t header_count);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __RANHTTP_MODULE_REQUEST_PARSER_UTIL_H__