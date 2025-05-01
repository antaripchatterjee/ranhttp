#define DEBUG_LEVEL 0

#include <string.h>
#include <stdlib.h>
#include "request-parser/main.h"
#include "request-parser/@util.h"



DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_destroy(ranhttp__request_t *request) {
    if (request == NULL) {
        return RANHTTP_REQUEST_PARSER_ERROR_INVALID_POINTER;
    }
    
    if (request->path_params) {
        for (size_t i = 0; i < request->control_limits.max_path_param_count; i++) {
            if(request->path_params[i].value) free(request->path_params[i].value);
        }
        free(request->path_params);
    }
    
    if (request->query_params) {
        for (size_t i = 0; i < request->control_limits.max_query_param_count; i++) {
            if(request->query_params[i].value) free(request->query_params[i].value);
        }
        free(request->query_params);
    }
    
    if (request->headers) {
        for (size_t i = 0; i < request->control_limits.max_header_count; i++) {
            if(request->headers[i].value) free(request->headers[i].value);
        }
        free(request->headers);
    }
    
    if (request->cookies) {
        for (size_t i = 0; i < request->control_limits.max_cookie_count; i++) {
            if(request->cookies[i].value) free(request->cookies[i].value);
        }
        free(request->cookies);
    }
    
    if (request->payload) {
        free(request->payload);
    }

    return RANHTTP_REQUEST_PARSER_ERROR_NONE;
}

DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_init(ranhttp__request_t *request, ranhttp__request_control_limit_t *control_limits) {
    if (request == NULL) {
        return RANHTTP_REQUEST_PARSER_ERROR_INVALID_POINTER;
    }
    memset(request->http_method, 0, sizeof(request->http_method));
    memset(request->path, 0, sizeof(request->path));
    memset(request->http_version, 0, sizeof(request->http_version));
    
    if(control_limits) {
        memcpy(&(request->control_limits), control_limits, sizeof(ranhttp__request_control_limit_t));
    } else {
        request->control_limits = (ranhttp__request_control_limit_t) RANHTTP_INIT_REQUEST_CONTROL_LIMITS;
    }

    if(request->control_limits.max_path_param_count) {
        request->path_params = (ranhttp__request_path_param_t*) malloc(sizeof(ranhttp__request_path_param_t) * request->control_limits.max_path_param_count);
        if (!request->path_params) {
            return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
        }
        for (size_t i = 0; i < request->control_limits.max_path_param_count; i++) {
            memset(request->path_params[i].name, 0, sizeof(request->path_params[i].name));
            request->path_params[i].value = NULL;
        }
    } else {
        request->path_params = NULL;
    }

    if(request->control_limits.max_query_param_count) {
        request->query_params = (ranhttp__request_query_param_t*) malloc(sizeof(ranhttp__request_query_param_t) * request->control_limits.max_query_param_count);
        if (!request->query_params) {
            free(request->path_params);
            return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
        }
        for (size_t i = 0; i < request->control_limits.max_query_param_count; i++) {
            memset(request->query_params[i].name, 0, sizeof(request->query_params[i].name));
            request->query_params[i].value = NULL;
        }
    } else {
        request->query_params = NULL;
    }
    
    if(request->control_limits.max_header_count) {
        request->headers = (ranhttp__request_header_t*) malloc(sizeof(ranhttp__request_header_t) * request->control_limits.max_header_count);
        if (!request->headers) {
            free(request->path_params);
            free(request->query_params);
            return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
        }
        for (size_t i = 0; i < request->control_limits.max_header_count; i++) {
            memset(request->headers[i].name, 0, sizeof(request->headers[i].name));
            request->headers[i].value = NULL;
        }
    } else {
        request->headers = NULL;
    }
    
    if(request->control_limits.max_cookie_count) {
        request->cookies = (ranhttp__request_cookie_t*) malloc(sizeof(ranhttp__request_cookie_t) * request->control_limits.max_cookie_count);
        if (!request->cookies) {
            free(request->path_params);
            free(request->query_params);
            free(request->headers);
            return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
        }
        for (size_t i = 0; i < request->control_limits.max_cookie_count; i++) {
            memset(request->cookies[i].name, 0, sizeof(request->cookies[i].name));
            request->cookies[i].value = NULL;
        }
    } else {
        request->cookies = NULL;
    }

    if(request->control_limits.max_payload_size) {
        request->payload = (char*) malloc(RANHTTP_REQUEST_PAYLOAD_BUF_SIZE);
        if (!request->payload) {
            free(request->path_params);
            free(request->query_params);
            free(request->headers);
            free(request->cookies);
            return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
        }
        memset(request->payload, 0, RANHTTP_REQUEST_PAYLOAD_BUF_SIZE);
    } else {
        request->payload = NULL;
    }
    
    return RANHTTP_REQUEST_PARSER_ERROR_NONE;
}

DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_parse_from_buffer(ranhttp__request_t *request, const char* buf) {
    if(!request || !buf) {
        return RANHTTP_REQUEST_PARSER_ERROR_INVALID_POINTER;
    }
    int parsing_stage = 0; // 0: request line, 1,2: headers, 3: payload
    const char* start = buf;
    const char* end;
    size_t r_buffer_size = 257; // 256 + 1 for null terminator
    // size_t r_buffer_size_last = r_buffer_size;
    char* r_buffer = (char*) malloc(r_buffer_size);
    if (!r_buffer) {
        return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
    }
    ranhttp__request_parser_error_t error = RANHTTP_REQUEST_PARSER_ERROR_NONE;
    while((end = strstr(start, "\r\n")) != NULL && error == RANHTTP_REQUEST_PARSER_ERROR_NONE) {
        size_t line_len = end - start;
        if (line_len == 0 && parsing_stage == 2) {
            // Empty r_buffer indicates end of headers
            parsing_stage = 3;
            break; // Break out of the loop
        } 
        if (line_len > r_buffer_size) {
            // Line too long, realloc
            r_buffer_size = line_len + 1;
            r_buffer = (char*) realloc(r_buffer, r_buffer_size);
            if (!r_buffer) {
                return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
            }
            // r_buffer_size_last = r_buffer_size;
        }
        
        memset(r_buffer, 0, r_buffer_size);
        strncpy(r_buffer, start, line_len);

        switch(parsing_stage) {
            case 0:
                // First r_buffer is the request r_buffer
                error = ranhttp__request_read_line_0(request, r_buffer);
                parsing_stage = 1;
                break;
            case 1:
            case 2:
                // Subsequent lines are headers
                parsing_stage = 2;
                break;
        }
        start = end + 2; // Move past "\r\n"
    }
    // Read payload if any
    // if (parsing_stage == 3) {
    //     size_t payload_len = strlen(start);
    //     if (payload_len > request->control_limits.max_payload_size) {
    //         error = RANHTTP_REQUEST_PARSER_ERROR_INVALID_VERSION;
    //     } else {
    //         if (payload_len > r_buffer_size_last) {
    //             r_buffer = (char*) realloc(r_buffer, payload_len + 1);
    //             if (!r_buffer) {
    //                 return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
    //             }
    //         }
    //         strncpy(r_buffer, start, payload_len);
    //         r_buffer[payload_len] = '\0';
    //         request->payload = r_buffer;
    //     }
    // }
    // Free the buffer
    free(r_buffer);
    return error;
}
