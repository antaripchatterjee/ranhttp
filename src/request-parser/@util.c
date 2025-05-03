#define DEBUG_LEVEL 0

#include <stdlib.h>
#include <string.h>

#include "request-parser/@util.h"
#include "request-parser/@macro.h"

DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_read_line_0(ranhttp__request_t *request, const char *line_0)
{
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
                strncpy(request->path, full_url_path, url_path_len);

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
                    if (query_param_count >= request->control_limits.max_query_param_count)
                    {
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
                        strncpy(request->query_params[query_param_count].name, query_param_buf, query_param_name_len);
                        request->query_params[query_param_count].name[query_param_name_len] = '\0';
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
                            request->query_params[query_param_count].value = (char *)malloc(query_param_value_len + 1);
                            if (!request->query_params[query_param_count].value)
                            {
                                free(query_param_buf);
                                free(full_url_path);
                                return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
                            }
                            strncpy(request->query_params[query_param_count].value, query_param_value_start, query_param_value_len);
                            request->query_params[query_param_count].value[query_param_value_len] = '\0';
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
                strncpy(request->path, full_url_path, full_url_path_size);
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