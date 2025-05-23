#ifndef __RANHTTP_MODULE_REQUEST_PARSER_ERROR_H__
#define __RANHTTP_MODULE_REQUEST_PARSER_ERROR_H__

typedef enum _ranhttp__request_parser_error_t {
    RANHTTP_REQUEST_PARSER_ERROR_INVALID_POINTER = -2,
    RANHTTP_REQUEST_PARSER_ERROR_INVALID_FD = -1,
    RANHTTP_REQUEST_PARSER_ERROR_NONE = 0,
    RANHTTP_REQUEST_PARSER_ERROR_INIT_FAILED,
    RANHTTP_REQUEST_PARSER_ERROR_READ_FAILED,
    RANHTTP_REQUEST_PARSER_ERROR_INVALID_METHOD,
    RANHTTP_REQUEST_PARSER_ERROR_MALFORMED_URI_COMPONENT,
    RANHTTP_REQUEST_PARSER_ERROR_INVALID_PATH,
    RANHTTP_REQUEST_PARSER_ERROR_PATH_TOO_LONG,
    RANHTTP_REQUEST_PARSER_ERROR_METHOD_TOO_LONG,
    RANHTTP_REQUEST_PARSER_ERROR_MALFORMED_QUERY_STRING,
    RANHTTP_REQUEST_PARSER_ERROR_QUERY_PARAM_NAME_TOO_LONG,
    RANHTTP_REQUEST_PARSER_ERROR_QUERY_PARAM_VALUE_TOO_BIG,
    RANHTTP_REQUEST_PARSER_ERROR_INVALID_VERSION,
    RANHTTP_REQUEST_PARSER_ERROR_INVALID_HEADER_STRING,
    RANHTTP_REQUEST_PARSER_ERROR_HEADER_NAME_TOO_LONG,
    RANHTTP_REQUEST_PARSER_ERROR_HEADER_VALUE_TOO_LONG,
    RANHTTP_REQUEST_PARSER_WARN_TOO_MANY_HEADERS,
    RANHTTP_REQUEST_PARSER_WARN_HEADER_NOT_ALLOWED,
    RANHTTP_REQUEST_PARSER_ERROR_HEADER_VALUE_TOO_BIG,
    RANHTTP_REQUEST_PARSER_ERROR_TOO_MANY_HEADERS,
    RANHTTP_REQUEST_PARSER_ERROR_INVALID_HEADER_NAME,
    RANHTTP_REQUEST_PARSER_ERROR_DUPLICATE_HEADER_NAME,
    RANHTTP_REQUEST_PARSER_ERROR_INVALID_HEADER_VALUE,
    RANHTTP_REQUEST_PARSER_ERROR_MALFORMED_CONTENT_LENGTH,
    RANHTTP_REQUEST_PARSER_ERROR_CONTENT_LENGTH_EXCEEDED_LIMIT,
    RANHTTP_REQUEST_PARSER_WARN_CONTENT_READ_NOT_FINISHED
} ranhttp__request_parser_error_t;

#endif // __RANHTTP_MODULE_REQUEST_PARSER_ERROR_H__