#ifndef __RANHTTP_MODULE_RANHTTP_UTILITIES_MAIN_H__
#define __RANHTTP_MODULE_RANHTTP_UTILITIES_MAIN_H__

#include <stdlib.h>
#include <stdint.h>

#include "request-parser/@types.h"

#include "../common.h"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

DLLEXPORT
int ranhttp__utility_is_valid_query_string(const char* query_string, const size_t max_char_count);

DLLEXPORT
int ranhttp__utility_is_valid_utf8_sequence(const uint8_t *data, size_t len, size_t *step_out);

DLLEXPORT
char* ranhttp__utility_decode_uri_component(const char* encoded_component, const size_t max_char_count, char* decoded_output, const size_t max_output_size);

DLLEXPORT
int ranhttp__utility_is_valid_header_name(const char* header_name);

DLLEXPORT
char* ranhttp__utility_find_header_from_request(ranhttp__request_t* request, const char* lc_header_name, int* error);

DLLEXPORT
int ranhttp__utility_find_content_length(ranhttp__request_t* request, uint64_t* value);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __RANHTTP_MODULE_RANHTTP_UTILITIES_MAIN_H__