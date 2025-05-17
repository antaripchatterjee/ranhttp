#define DEBUG_LEVEL 0

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "request-parser/@util.h"
#include "request-parser/@macro.h"

#include "ranhttp-utilities/main.h"

DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_read_line_0(ranhttp__request_t *request, const char *line_0)
{
    if (!request || !line_0) {
        return RANHTTP_REQUEST_PARSER_ERROR_INVALID_POINTER;
    }
    const char *start = line_0;
    char *method_end = strchr(line_0, ' ');
    if (method_end)
    {
        strncpy(request->http_method, line_0, method_end - start);
        char *full_url_path_end = strchr(method_end + 1, ' ');
        if (full_url_path_end)
        {
            size_t full_url_path_size = full_url_path_end - method_end - 1;
            if (full_url_path_size == 0)
            {
                return RANHTTP_REQUEST_PARSER_ERROR_INVALID_PATH;
            }

            char *full_url_path = (char *)malloc(full_url_path_size + 1);
            if (!full_url_path)
            {
                return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
            }
            strncpy(full_url_path, method_end + 1, full_url_path_size);
            full_url_path[full_url_path_size] = '\0';

            char *path_end = strchr(full_url_path, '?');
            if (path_end)
            {
                size_t url_path_len = path_end - full_url_path;
                if (url_path_len > RANHTTP_REQUEST_URL_PATH_SIZE)
                {
                    free(full_url_path);
                    return RANHTTP_REQUEST_PARSER_ERROR_PATH_TOO_LONG;
                }
                // strncpy(request->path, full_url_path, url_path_len);
                if(!ranhttp__utility_decode_uri_component(full_url_path, url_path_len, request->path, sizeof(request->path)))
                {
                    free(full_url_path);
                    return RANHTTP_REQUEST_PARSER_ERROR_MALFORMED_URI_COMPONENT;
                }
                
                if(!ranhttp__utility_is_valid_query_string(path_end, full_url_path_end - path_end)) {
                    free(full_url_path);
                    return RANHTTP_REQUEST_PARSER_ERROR_MALFORMED_QUERY_STRING;
                }

                // Parse query parameters
                char *query_param_start = path_end + 1;
                char *query_param_end = NULL;
                size_t query_param_buf_len = 257UL; // 256 + 1 for null terminator
                char *query_param_buf = (char *)malloc(query_param_buf_len);
                if (!query_param_buf)
                {
                    free(full_url_path);
                    return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
                }
                size_t query_param_count = 0;
                size_t query_param_len = 0;
                do
                {
                    if (query_param_count >= request->limits.max_query_param_count)
                    {
                        // Breaking out of the loop but not returning error as remaining raw request is still valid
                        break; // Exceeded max query param count
                    }
                    query_param_end = strchr(query_param_start, '&');
                    if (query_param_end)
                    {
                        query_param_len = query_param_end - query_param_start;
                    }
                    else
                    {
                        // Last query parameter
                        query_param_len = strlen(query_param_start);
                    }
                    if (query_param_len > 0)
                    {
                        if (query_param_len >= query_param_buf_len)
                        {
                            query_param_buf_len = query_param_len + 1;
                            query_param_buf = (char *)realloc(query_param_buf, query_param_buf_len);
                            if (!query_param_buf)
                            {
                                free(full_url_path);
                                return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
                            }
                        }
                        strncpy(query_param_buf, query_param_start, query_param_len);
                        query_param_buf[query_param_len] = '\0';
                        char *query_param_name_end = strchr(query_param_buf, '=');
                        size_t query_param_name_len = query_param_name_end ? query_param_name_end - query_param_buf : query_param_len;
                        if (query_param_name_len > RANHTTP_REQUEST_QUERY_PARAM_NAME_MAX_SIZE)
                        {
                            free(query_param_buf);
                            free(full_url_path);
                            return RANHTTP_REQUEST_PARSER_ERROR_QUERY_PARAM_NAME_TOO_LONG;
                        }
                        // strncpy(request->query_params[query_param_count].name, query_param_buf, query_param_name_len);
                        // request->query_params[query_param_count].name[query_param_name_len] = '\0';
                        if(!ranhttp__utility_decode_uri_component(query_param_buf, query_param_name_len, request->query_params[query_param_count].name, sizeof(request->query_params[query_param_count].name))) {
                            free(query_param_buf);
                            free(full_url_path);
                            return RANHTTP_REQUEST_PARSER_ERROR_MALFORMED_URI_COMPONENT;
                        }
                        DEBUG_LOG("query_param_name: %s\n", request->query_params[query_param_count].name);
                        char *query_param_value_start = query_param_name_end + 1;
                        size_t query_param_value_len = query_param_name_end ? strlen(query_param_value_start) : 0;
                        DEBUG_LOG("query_param_value_len: %zu\n", query_param_value_len);
                        if (query_param_value_len > 0)
                        {
                            if (query_param_value_len > RANHTTP_REQUEST_QUERY_PARAM_VALUE_MAX_SIZE)
                            {
                                free(query_param_buf);
                                free(full_url_path);
                                return RANHTTP_REQUEST_PARSER_ERROR_QUERY_PARAM_VALUE_TOO_BIG;
                            }
                            request->query_params[query_param_count].value = ranhttp__utility_decode_uri_component(query_param_value_start, query_param_value_len, NULL, query_param_value_len + 1);
                            if(!request->query_params[query_param_count].value)
                            {
                                free(query_param_buf);
                                free(full_url_path);
                                return RANHTTP_REQUEST_PARSER_ERROR_MALFORMED_URI_COMPONENT;
                            }
                            // request->query_params[query_param_count].value = (char *)malloc(query_param_value_len + 1);
                            // if (!request->query_params[query_param_count].value)
                            // {
                            //     free(query_param_buf);
                            //     free(full_url_path);
                            //     return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
                            // }
                            // strncpy(request->query_params[query_param_count].value, query_param_value_start, query_param_value_len);
                            // request->query_params[query_param_count].value[query_param_value_len] = '\0';
                        }
                        query_param_count++;
                    }
                    query_param_start = query_param_end + 1;
                } while (query_param_end != NULL && query_param_end != query_param_start);
                free(query_param_buf);
            }
            else
            {
                // No query parameters
                // strncpy(request->path, full_url_path, full_url_path_size);
                if(!ranhttp__utility_decode_uri_component(full_url_path, full_url_path_size, request->path, sizeof(request->path))) {
                    free(full_url_path);
                    return RANHTTP_REQUEST_PARSER_ERROR_MALFORMED_URI_COMPONENT;
                }
            }
            free(full_url_path);
        }
        else
        {
            // No full URL path found
            return RANHTTP_REQUEST_PARSER_ERROR_INVALID_PATH;
        }
        // Parse HTTP version
        char *version_start = full_url_path_end + 1;
        size_t version_len = strlen(version_start);
        if (version_len > 0)
        {
            if (version_len >= sizeof(request->http_version))
            {
                return RANHTTP_REQUEST_PARSER_ERROR_INVALID_VERSION;
            }
            strncpy(request->http_version, version_start, version_len);
        }
        else
        {
            // No HTTP version found
            return RANHTTP_REQUEST_PARSER_ERROR_INVALID_VERSION;
        }
    }
    else
    {
        // No method found
        return RANHTTP_REQUEST_PARSER_ERROR_INVALID_METHOD;
    }
    return RANHTTP_REQUEST_PARSER_ERROR_NONE;
}


DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_read_header_string(ranhttp__request_t *request, const char* header_string, const size_t header_count) {
    if(!request || !header_string) {
        return RANHTTP_REQUEST_PARSER_ERROR_INVALID_POINTER;
    }
    if(header_count >= request->limits.max_header_count) {
        return RANHTTP_REQUEST_PARSER_WARN_TOO_MANY_HEADERS;
    }
    
    size_t len = strlen(header_string);
    size_t trailing_space_count = 0;
    // int is_cookie = 0;
    // Parsing cookie should be deferred to the cookie parser on demand

    // Counting trailing spaces
    for (size_t i = len-1; i >= 0 && header_string[i] != ':'; i--) {
        if(isspace(header_string[i])) {
            // len--;
            trailing_space_count++;
        } else {
            break;
        }
    }
    const char *colon = strchr(header_string, ':');
    char _header_name_temp[sizeof(((ranhttp__request_header_t){0}).name)] = { 0 };
    if (colon) {
        size_t header_name_len = colon - header_string;
        if (header_name_len >= sizeof(request->headers[header_count].name)) {
            return RANHTTP_REQUEST_PARSER_ERROR_HEADER_NAME_TOO_LONG;
        }
        // strncpy(request->headers[header_count].name, header_string, header_name_len);
        // copy characters from header_string to request->headers[header_count].name till header_name_len in lower case
        for (size_t i = 0; i < header_name_len; i++) {
            _header_name_temp[i] = tolower(header_string[i]);
            // request->headers[header_count].name[i] = tolower(header_string[i]);
        }
        // request->headers[header_count].name[header_name_len] = '\0';
        int s = -1;
        if(request->limits.header_wl.headers) {
            s = 0;
            for(size_t i = 0; i < request->limits.header_wl.count; i++) {
                if(strcmp(_header_name_temp, request->limits.header_wl.headers[i]) == 0) {
                    s = 1;
                    break;
                }
            }
        }
        if(s == 0) {
            return RANHTTP_REQUEST_PARSER_WARN_HEADER_NOT_ALLOWED;
        }
        strcpy(request->headers[header_count].name, _header_name_temp);
        if(!ranhttp__utility_is_valid_header_name(request->headers[header_count].name)) {
            memset(request->headers[header_count].name, 0, sizeof(request->headers[header_count].name));
            return RANHTTP_REQUEST_PARSER_ERROR_INVALID_HEADER_NAME;
        }
        // Now check if the header_name is already present in the request
        for (size_t i = 0; i < header_count; i++) {
            if (strcmp(request->headers[i].name, request->headers[header_count].name) == 0) {
                // Header already exists
                return RANHTTP_REQUEST_PARSER_ERROR_DUPLICATE_HEADER_NAME;
            }
        }
        // Check if the header is a cookie
        if (strcmp(request->headers[header_count].name, "cookie") == 0) {
            // is_cookie = 1;
            request->cookie_header_index = header_count;
        }

        const char *value_start = colon + 1;
        // Decounting leading spaces
        while (*value_start == ' ') {
            value_start++;
        }
        size_t value_len = strlen(value_start) - trailing_space_count;
        if (value_len <= 0) {
            return RANHTTP_REQUEST_PARSER_ERROR_INVALID_HEADER_STRING;
        } else if(value_len > request->limits.max_header_value_size) {
            return RANHTTP_REQUEST_PARSER_ERROR_HEADER_VALUE_TOO_LONG;
        }
        request->headers[header_count].value = (char *)malloc(value_len + 1);
        if (!request->headers[header_count].value) {
            return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
        }
        strncpy(request->headers[header_count].value, value_start, value_len);
        request->headers[header_count].value[value_len] = '\0';
        for(size_t i = 0; i < value_len; i++) {
            if (iscntrl(request->headers[header_count].value[i]) && request->headers[header_count].value[i] != '\t') {
                free(request->headers[header_count].value);
                request->headers[header_count].value = NULL;
                memset(request->headers[header_count].name, 0, sizeof(request->headers[header_count].name));
                return RANHTTP_REQUEST_PARSER_ERROR_INVALID_HEADER_VALUE;
            }
        }
    } else {
        return RANHTTP_REQUEST_PARSER_ERROR_INVALID_HEADER_STRING;
    }
    return RANHTTP_REQUEST_PARSER_ERROR_NONE;
}