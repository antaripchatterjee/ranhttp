#ifndef __RANHTTP_MODULE_REQUEST_PARSER_MAIN_H__
#define __RANHTTP_MODULE_REQUEST_PARSER_MAIN_H__

#include "request-parser/@error.h"
#include "request-parser/@types.h"
#include "request-parser/@config.h"

#include "../common.h"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_destroy(ranhttp__request_t *request);

DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_init(ranhttp__request_t *request, ranhttp__request_control_limit_t *control_limits);

DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_parse_from_buffer(ranhttp__request_t *request, const char* buf);

DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_parse_from_fd(ranhttp__request_t *request, int fd);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __RANHTTP_MODULE_REQUEST_PARSER_MAIN_H__
