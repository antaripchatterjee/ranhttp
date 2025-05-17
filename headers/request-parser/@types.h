#ifndef __RANHTTP_MODULE_REQUEST_PARSER_TYPES_H__
#define __RANHTTP_MODULE_REQUEST_PARSER_TYPES_H__

#include "request-parser/@macro.h"

#include <stdlib.h>

typedef struct {
    char name[RANHTTP_REQUEST_HEADER_NAME_MAX_SIZE+1];
    char *value;
} ranhttp__request_header_t;


typedef struct {
    char name[RANHTTP_REQUEST_QUERY_PARAM_NAME_MAX_SIZE+1];
    char *value;
} ranhttp__request_query_param_t;

typedef struct {
    size_t count;
    char (*headers)[RANHTTP_REQUEST_HEADER_NAME_MAX_SIZE+1];
} ranhttp__header_whitelist_t;

typedef struct {
    size_t max_header_count;
    size_t max_header_value_size;
    size_t max_query_param_count;
    size_t max_query_param_value_size;
    size_t max_payload_size;
    ranhttp__header_whitelist_t header_wl;
} ranhttp__request_limit_t;

typedef struct {
    char http_method[10];
    char http_version[10];
    char path[RANHTTP_REQUEST_URL_PATH_SIZE+1];
    // ranhttp__request_path_param_t *path_params;
    ranhttp__request_query_param_t *query_params;
    ranhttp__request_header_t *headers;
    // ranhttp__request_cookie_t *cookies;
    char *payload;
    int cookie_header_index;
    ranhttp__request_limit_t limits;
} ranhttp__request_t;

#endif // __RANHTTP_MODULE_REQUEST_PARSER_TYPES_H__