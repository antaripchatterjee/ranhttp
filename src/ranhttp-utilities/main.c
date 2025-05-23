#define DEBUG_LEVEL 0

#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

#include "ranhttp-utilities/@private.h"
#include "ranhttp-utilities/main.h"

DLLEXPORT
int ranhttp__utility_is_valid_query_string(const char* query_string, const size_t max_char_count) {
    if (!query_string || query_string[0] != '?') {
        return 0;
    }
    for (size_t i = 1; i < max_char_count && query_string[i] != '\0'; i++) {
        char c = query_string[i];
        if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~' 
            || c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' ||
            c == '*' || c == '+' || c == ',' || c == ';' || c == '=' || c == ':' || c == '@' || c == '/' || c == '?') {
            continue; // Valid character
        } else if (c == '%') {
            // Check for percent-encoded characters
            if (i + 2 < max_char_count && isxdigit(query_string[i + 1]) && isxdigit(query_string[i + 2])) {
                continue; // Valid percent-encoded character
                i += 2; // Skip the next two characters
            } else {
                return 0; // Invalid percent-encoding
            }
        } else {
            return 0; // Invalid character
        }
    }
    return 1;
}


DLLEXPORT
int ranhttp__utility_is_valid_utf8_sequence(const uint8_t *data, size_t len, size_t *step_out) {
    uint8_t b0 = data[0];

    if (b0 <= 0x7F) {
        *step_out = 1;
        return 1;
    } else if (b0 >= 0xC2 && b0 <= 0xDF && len >= 2 &&
        data[1] >= 0x80 && data[1] <= 0xBF) {
        *step_out = 2;
        return 1;
    } else if (b0 == 0xE0 && len >= 3 &&
        data[1] >= 0xA0 && data[1] <= 0xBF &&
        data[2] >= 0x80 && data[2] <= 0xBF) {
        *step_out = 3;
        return 1;
    } else if (((b0 >= 0xE1 && b0 <= 0xEC) || (b0 >= 0xEE && b0 <= 0xEF)) && len >= 3 &&
        data[1] >= 0x80 && data[1] <= 0xBF &&
        data[2] >= 0x80 && data[2] <= 0xBF) {
        *step_out = 3;
        return 1;
    } else if (b0 == 0xED && len >= 3 &&
        data[1] >= 0x80 && data[1] <= 0x9F &&
        data[2] >= 0x80 && data[2] <= 0xBF) {
        *step_out = 3;
        return 1;
    } else if (b0 == 0xF0 && len >= 4 &&
        data[1] >= 0x90 && data[1] <= 0xBF &&
        data[2] >= 0x80 && data[2] <= 0xBF &&
        data[3] >= 0x80 && data[3] <= 0xBF) {
        *step_out = 4;
        return 1;
    } else if (b0 >= 0xF1 && b0 <= 0xF3 && len >= 4 &&
        data[1] >= 0x80 && data[1] <= 0xBF &&
        data[2] >= 0x80 && data[2] <= 0xBF &&
        data[3] >= 0x80 && data[3] <= 0xBF) {
        *step_out = 4;
        return 1;
    } else if (b0 == 0xF4 && len >= 4 &&
        data[1] >= 0x80 && data[1] <= 0x8F &&
        data[2] >= 0x80 && data[2] <= 0xBF &&
        data[3] >= 0x80 && data[3] <= 0xBF) {
        *step_out = 4;
        return 1;
    }

    return 0;
}



DLLEXPORT
char* ranhttp__utility_decode_uri_component(const char* encoded_component, const size_t max_char_count, char* decoded_output, const size_t max_output_size) {
    if(!encoded_component) {
        return NULL;
    }

    int is_dynamic = 0;
    char *out = NULL;
    if(decoded_output) {
        out = decoded_output;
    } else {
        out = (char*) malloc(max_output_size); // max output is length of input
        if (!out) return NULL;
        is_dynamic = 1;
    }

    size_t oi = 0;
    size_t ii = 0;
    char* s = (char*) encoded_component;
    while (ii < max_char_count && oi < max_output_size - 1) {
        if (s[ii] == '%') {
            uint8_t utf8[4];
            size_t utf8len = 0;

            while (ii + 2 < max_char_count && s[ii] == '%' && utf8len < 4) {
                int error = 0;
                uint8_t byte = ranhttp__utility_hex_to_byte(s[ii + 1], s[ii + 2], &error);
                if (error) {
                    if(is_dynamic) {
                        free(out);
                    } else {
                        memset(out, 0, max_output_size);
                    }
                    return NULL; // invalid hex
                }
                utf8[utf8len++] = byte;
                ii += 3;
            }

            if(utf8len == 0) {
                if(is_dynamic) {
                    free(out);
                } else {
                    memset(out, 0, max_output_size);
                }
                return NULL; // invalid percent encoding
            }

            size_t step = 0;
            if (!ranhttp__utility_is_valid_utf8_sequence(utf8, utf8len, &step)) {
                if(is_dynamic) {
                    free(out);
                } else {
                    memset(out, 0, max_output_size);
                }
                return NULL; // invalid UTF-8
            }
            size_t j;
            for (j = 0; j < step && oi < max_output_size - 1; j++) {
                out[oi++] = utf8[j];
            }
            if(j != step) {
                if(is_dynamic) {
                    free(out);
                } else {
                    memset(out, 0, max_output_size);
                }
                return NULL; // incomplete UTF-8 sequence
            }

            // If extra utf8 bytes collected beyond one character, keep for next loop
            ii -= (utf8len - step);

        } else if (s[ii] == '+') {
            out[oi++] = ' ';
            ii++;
        } else {
            out[oi++] = s[ii++];
        }
    }
    if(ii < max_char_count) {
        if(is_dynamic) {
            free(out);
        } else {
            memset(out, 0, max_output_size);
        }
        return NULL; // output buffer too small to fit decoded string
    }

    out[oi] = '\0';
    return out;
}

DLLEXPORT
int ranhttp__utility_is_valid_header_name(const char* header_name) {
    if (!header_name || *header_name == '\0') return 0;

    size_t len = strlen(header_name);
    // Reject leading or trailing whitespace
    if (isspace((unsigned char)header_name[0]) || isspace((unsigned char)header_name[len - 1]))
        return 0;

    for (size_t i = 0; i < len; ++i) {
        char c = header_name[i];
        
        if (!isalnum((unsigned char)c) && 
            c != '!' && c != '#' && c != '$' && c != '%' && c != '&' && c != '\'' &&
            c != '*' && c != '+' && c != '-' && c != '.' && c != '^' && c != '_' &&
            c != '`' && c != '|' && c != '~') {
            return 0;
        }
    }

    return 1;
}

DLLEXPORT
char* ranhttp__utility_find_header_from_request(ranhttp__request_t* request, const char* lc_header_name, int* error) {
    if(!request || !(request->headers) || !lc_header_name) {
        if(error) {
            *error = -1;
        }
        return NULL; // This is error
    }
    if(!error) {
        return NULL; // This is error too
    }

    // Iterate through each header and compare the value of header name with arg lc_header_name until the lc_header_name is matched or header name is empty string.
    for(size_t i = 0; i < request->limits.max_header_count && request->headers[i].name[0] != '\0'; i++) {
        if(strcmp(request->headers[i].name, lc_header_name) == 0) {
            *error = 0;
            return request->headers[i].value;
        }
    }
    *error = 0; 
    return NULL; // This is not error
}

DLLEXPORT
int ranhttp__utility_find_content_length(ranhttp__request_t* request, uint64_t* value) {
    if(!value) return 0;
    int error = 0;
    uint64_t content_length = 0;
    char* end;
    char* content_length_buf = ranhttp__utility_find_header_from_request(request, "content-length", &error);
    if(error == 0 && content_length_buf) {
        // convert content_length_buf to uint64_t
        errno = 0;
        for(const char* p = content_length_buf; *p; ++p) {
            if(*p < '0' || *p > '9') {
                error = 1;
                break;
            }
        }
        if(error == 0) {
            content_length = (uint64_t) strtoull(content_length_buf, &end, 10);
            if(errno == ERANGE || content_length > UINT64_MAX || *end != '\0') {
                error = 1;
            }
            *value = content_length;
        }
    }
    return error;
}


DLLEXPORT
size_t ranhttp__utility_whitelist_header(ranhttp__header_whitelist_t* whitelist, const char* header_name) {
    if(!whitelist || !header_name) {
        return 0U;
    }
    size_t header_name_len = strlen(header_name);

    if(header_name_len >= sizeof(whitelist->headers[whitelist->count])) {
        return whitelist->count;
    }
    
    char _header_name_lower[sizeof(*(whitelist->headers))] = { 0 };
    for(size_t i = 0; i < header_name_len; i++) {
        _header_name_lower[i] = tolower(header_name[i]);
    }

    for(size_t i = 0; i < whitelist->count; i++) {
        if(strcmp(_header_name_lower, whitelist->headers[i]) == 0) {
            return whitelist->count;
        }
    }
    DEBUG_LOG("Whitelisted Header Name: %s\n", _header_name_lower);

    if(!ranhttp__utility_is_valid_header_name(_header_name_lower)) {
        return whitelist->count;
    }

    if(whitelist->count % 32 == 0) {
        whitelist->headers = realloc(whitelist->headers , sizeof(*(whitelist->headers)) * (((whitelist->count / 32) + 1) * 32));
        if(!(whitelist->headers)) {
            whitelist->count = 0U;
            return 0U;
        }
    }
    strcpy(whitelist->headers[whitelist->count++], _header_name_lower);
    return whitelist->count;
}