#ifndef __RANHTTP_MODULE_REQUEST_PARSER_TYPES_H__
#define __RANHTTP_MODULE_REQUEST_PARSER_TYPES_H__

#include "request-parser/@macro.h"

#include <stdlib.h>

typedef struct {
    char name[RANHTTP_REQUEST_HEADER_NAME_MAX_SIZE+1];
    char *value;
} ranhttp__request_header_t;

typedef struct {
    char name[RANHTTP_REQUEST_HEADER_NAME_MAX_SIZE+1];
    char *value;
} ranhttp__request_cookie_t;

typedef struct {
    char name[RANHTTP_REQUEST_PATH_PARAM_NAME_MAX_SIZE+1];
    char *value;
} ranhttp__request_path_param_t;

typedef struct {
    char name[RANHTTP_REQUEST_QUERY_PARAM_NAME_MAX_SIZE+1];
    char *value;
} ranhttp__request_query_param_t;

typedef struct {
    size_t max_header_count;
    size_t max_path_param_count;
    size_t max_query_param_count;
    size_t max_cookie_count;
    size_t max_payload_size;
} ranhttp__request_control_limit_t;

typedef struct {
    char http_method[10];
    char http_version[10];
    char path[RANHTTP_REQUEST_URL_PATH_SIZE+1];
    ranhttp__request_path_param_t *path_params;
    ranhttp__request_query_param_t *query_params;
    ranhttp__request_header_t *headers;
    ranhttp__request_cookie_t *cookies;
    char *payload;
    ranhttp__request_control_limit_t control_limits;
} ranhttp__request_t;

#endif // __RANHTTP_MODULE_REQUEST_PARSER_TYPES_H__