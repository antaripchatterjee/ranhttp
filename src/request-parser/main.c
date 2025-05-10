#define DEBUG_LEVEL 0

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "request-parser/main.h"
#include "request-parser/@util.h"



DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_destroy(ranhttp__request_t *request) {
    if (request == NULL) {
        return RANHTTP_REQUEST_PARSER_ERROR_INVALID_POINTER;
    }
    
    // if (request->path_params) {
    //     for (size_t i = 0; i < request->limits.max_path_param_count; i++) {
    //         if(request->path_params[i].value) free(request->path_params[i].value);
    //     }
    //     free(request->path_params);
    // }
    
    if (request->query_params) {
        for (size_t i = 0; i < request->limits.max_query_param_count; i++) {
            if(request->query_params[i].value) free(request->query_params[i].value);
        }
        free(request->query_params);
    }
    
    if (request->headers) {
        for (size_t i = 0; i < request->limits.max_header_count; i++) {
            if(request->headers[i].value) free(request->headers[i].value);
        }
        free(request->headers);
    }
    
    // if (request->cookies) {
    //     for (size_t i = 0; i < request->limits.max_cookie_count; i++) {
    //         if(request->cookies[i].value) free(request->cookies[i].value);
    //     }
    //     free(request->cookies);
    // }
    
    if (request->payload) {
        free(request->payload);
    }

    return RANHTTP_REQUEST_PARSER_ERROR_NONE;
}

DLLEXPORT
ranhttp__request_parser_error_t ranhttp__request_init(ranhttp__request_t *request, ranhttp__request_limit_t *limits) {
    if (request == NULL) {
        return RANHTTP_REQUEST_PARSER_ERROR_INVALID_POINTER;
    }
    memset(request->http_method, 0, sizeof(request->http_method));
    memset(request->path, 0, sizeof(request->path));
    memset(request->http_version, 0, sizeof(request->http_version));
    request->cookie_header_index = -1;
    
    if(limits) {
        memcpy(&(request->limits), limits, sizeof(ranhttp__request_limit_t));
    } else {
        request->limits = (ranhttp__request_limit_t) RANHTTP_REQUEST_DEFAULT_LIMITS;
    }

    // if(request->limits.max_path_param_count) {
    //     request->path_params = (ranhttp__request_path_param_t*) malloc(sizeof(ranhttp__request_path_param_t) * request->limits.max_path_param_count);
    //     if (!request->path_params) {
    //         return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
    //     }
    //     for (size_t i = 0; i < request->limits.max_path_param_count; i++) {
    //         memset(request->path_params[i].name, 0, sizeof(request->path_params[i].name));
    //         request->path_params[i].value = NULL;
    //     }
    // } else {
    //     request->path_params = NULL;
    // }

    if(request->limits.max_query_param_count) {
        request->query_params = (ranhttp__request_query_param_t*) malloc(sizeof(ranhttp__request_query_param_t) * request->limits.max_query_param_count);
        if (!request->query_params) {
            // free(request->path_params);
            return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
        }
        for (size_t i = 0; i < request->limits.max_query_param_count; i++) {
            memset(request->query_params[i].name, 0, sizeof(request->query_params[i].name));
            request->query_params[i].value = NULL;
        }
    } else {
        request->query_params = NULL;
    }
    
    if(request->limits.max_header_count) {
        request->headers = (ranhttp__request_header_t*) malloc(sizeof(ranhttp__request_header_t) * request->limits.max_header_count);
        if (!request->headers) {
            // free(request->path_params);
            free(request->query_params);
            return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
        }
        for (size_t i = 0; i < request->limits.max_header_count; i++) {
            memset(request->headers[i].name, 0, sizeof(request->headers[i].name));
            request->headers[i].value = NULL;
        }
    } else {
        request->headers = NULL;
    }
    
    // if(request->limits.max_cookie_count) {
    //     request->cookies = (ranhttp__request_cookie_t*) malloc(sizeof(ranhttp__request_cookie_t) * request->limits.max_cookie_count);
    //     if (!request->cookies) {
    //         free(request->path_params);
    //         free(request->query_params);
    //         free(request->headers);
    //         return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
    //     }
    //     for (size_t i = 0; i < request->limits.max_cookie_count; i++) {
    //         memset(request->cookies[i].name, 0, sizeof(request->cookies[i].name));
    //         request->cookies[i].value = NULL;
    //     }
    // } else {
    //     request->cookies = NULL;
    // }

    if(request->limits.max_payload_size) {
        request->payload = (char*) malloc(RANHTTP_REQUEST_PAYLOAD_BUF_SIZE);
        if (!request->payload) {
            // free(request->path_params);
            free(request->query_params);
            free(request->headers);
            // free(request->cookies);
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
    //     if (payload_len > request->limits.max_payload_size) {
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


ranhttp__request_parser_error_t ranhttp__request_parse_from_fd(ranhttp__request_t *request, int fd) {
    if(!request) {
        return RANHTTP_REQUEST_PARSER_ERROR_INVALID_POINTER;
    }
    if(fd < 0) {
        return RANHTTP_REQUEST_PARSER_ERROR_INVALID_FD;
    }

    char buffer[RANHTTP_REQUEST_READER_BUFFER_SIZE+1];
    size_t data_size = 0, allocated_space = RANHTTP_REQUEST_READER_BUFFER_SIZE+1;
    char* data = (char*) malloc(allocated_space);
    if(!data) {
        return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
    }
    memset(data, 0, allocated_space);
    int bytes_read = 0;
    int parsing_stage = 0; // 0: request line, 1,2: headers, 3: payload
    char last_char = '\0';
    size_t header_count = 0;
    while(1) {
        bytes_read = read(fd, buffer, RANHTTP_REQUEST_READER_BUFFER_SIZE);
        buffer[bytes_read] = '\0';
        if (bytes_read < 0) {
            free(data);
            return RANHTTP_REQUEST_PARSER_ERROR_READ_FAILED;
        } else if (bytes_read == 0) {
            // process the data
            break; // End of file
        }
        // DEBUG_LOG("Read %d bytes: %s\n", bytes_read, buffer);
        if(last_char == '\r' && buffer[0] == '\n') {
            data[data_size-1] = '\0'; 
            if(parsing_stage == 0) {
                // process the first data
                ranhttp__request_parser_error_t error = ranhttp__request_read_line_0(request, data);
                if(error != RANHTTP_REQUEST_PARSER_ERROR_NONE) {
                    free(data);
                    return error;
                }
                DEBUG_LOG("Request line: [%s]\n", data);
                parsing_stage = 1;
            } else if(parsing_stage == 1 || parsing_stage == 2) {
                if(data[0] == '\0') {
                    // Empty data indicates end of headers
                    parsing_stage = 3;
                    DEBUG_LOG("Ready to process payload\n");
                } else {
                    // process the headers
                    DEBUG_LOG("Header data: [%s]\n", data);
                    ranhttp__request_parser_error_t error = ranhttp__request_read_header_string(request, data, header_count);
                    if(error != RANHTTP_REQUEST_PARSER_ERROR_NONE) {
                        free(data);
                        return error;
                    }
                    header_count++;
                    parsing_stage = 2;
                }
            }
            memset(data, 0, allocated_space);
            data_size = 0;
            memcpy(buffer, buffer + 1, --bytes_read);
            buffer[bytes_read] = '\0';
        }
        if(bytes_read > 0) {
            char* start = buffer;
            size_t bytes_remained = bytes_read;
            do {
                char* end = strstr(start, "\r\n");
                if(end) {
                    size_t data_delta_len = end - start;
                    // DEBUG_LOG("Data size: %zu, Data delta length: %zu, Allocated Space %zu\n", data_size,data_delta_len, allocated_space);
                    if(data_delta_len > 0) {
                        if ((data_size + data_delta_len) > allocated_space) {
                            allocated_space += RANHTTP_REQUEST_READER_BUFFER_SIZE;
                            data = (char*) realloc(data, allocated_space);
                            if(!data) {
                                return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
                            }
                        }
                        strncpy(data+data_size, start, data_delta_len);
                        data_size += data_delta_len;
                        data[data_size] = '\0';
                    }
                    start = end + 2;
                    bytes_remained -= (data_delta_len + 2);
                    if(parsing_stage == 0) {
                        // pocess the first data
                        ranhttp__request_parser_error_t error = ranhttp__request_read_line_0(request, data);
                        if(error != RANHTTP_REQUEST_PARSER_ERROR_NONE) {
                            free(data);
                            return error;
                        }
                        DEBUG_LOG("Request line: [%s]\n", data);
                        parsing_stage = 1;
                        memset(data, 0, allocated_space);
                        data_size = 0;
                    } else if(parsing_stage == 1 || parsing_stage == 2) {
                        if(data[0] == '\0') {
                            // Empty data indicates end of headers
                            parsing_stage = 3;
                            DEBUG_LOG("Ready to process payload\n");
                        } else {
                            // process the headers
                            DEBUG_LOG("Header data: [%s]\n", data);
                            ranhttp__request_parser_error_t error = ranhttp__request_read_header_string(request, data, header_count);
                            if(error != RANHTTP_REQUEST_PARSER_ERROR_NONE) {
                                free(data);
                                return error;
                            }
                            header_count++;
                            parsing_stage = 2;
                        }
                        memset(data, 0, allocated_space);
                        data_size = 0;
                    }
                } else if(bytes_remained > 0) {
                    if((data_size + bytes_remained) > allocated_space) {
                        allocated_space += RANHTTP_REQUEST_READER_BUFFER_SIZE;
                        data = (char*) realloc(data, allocated_space);
                        if(!data) {
                            return RANHTTP_REQUEST_PARSER_ERROR_INITIALIZE_FAILED;
                        }
                    }
                    strncpy(data + data_size, start, bytes_remained);
                    data_size += bytes_remained;
                    data[data_size] = '\0';
                    bytes_remained = 0;
                }
                // DEBUG_LOG("Current data: %s, Data size %zu, Bytes Remained %zu\n", data, data_size, bytes_remained);
            } while(bytes_remained > 0);
        }
        last_char = buffer[bytes_read - 1];
    }
    free(data);
    return RANHTTP_REQUEST_PARSER_ERROR_NONE;
}