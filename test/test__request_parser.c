#define DEBUG_LEVEL 0

#include "request-parser/main.h"
#include "test-helper.h"
#include <fcntl.h>
#include <unistd.h>

#define TEST_FILE_PATH "test/test__request_parser.txt"

#define TEST_BUFFER "GET /path?param1=value1&param2=value2 HTTP/1.1\r\n" \
    "Host: example.com\r\n" \
    "User-Agent: test-agent\r\n" \
    "Accept: */*\r\n\r\n"

void test__ranhttp__request_init(ranhttp__request_t *request) {
    TEST_STARTED;
    int result = ranhttp__request_init(request, NULL);
    TEST_ASSERT(result == RANHTTP_REQUEST_PARSER_ERROR_NONE);
    TEST_ENDED;
}

void test__ranhttp__request_destroy(ranhttp__request_t *request) {
    TEST_STARTED;
    int result = ranhttp__request_destroy(request);
    TEST_ASSERT(result == RANHTTP_REQUEST_PARSER_ERROR_NONE);
    TEST_ENDED;
}

void test__ranhttp__request_parse_from_buffer(ranhttp__request_t *request, const char* buf) {
    TEST_STARTED;
    int result = ranhttp__request_parse_from_buffer(request, buf);

    TEST_ASSERT(result == RANHTTP_REQUEST_PARSER_ERROR_NONE);
    TEST_ASSERT_STR_EQ(request->http_method, "GET");
    TEST_ASSERT_STR_EQ(request->path, "/path");
    TEST_ASSERT_STR_EQ(request->http_version, "HTTP/1.1");
    TEST_ASSERT_STR_EQ(request->query_params[0].name, "param1");
    TEST_ASSERT_STR_EQ(request->query_params[0].value, "value1");
    TEST_ASSERT_STR_EQ(request->query_params[1].name, "param2");
    TEST_ASSERT_STR_EQ(request->query_params[1].value, "value2");
    TEST_ENDED;
}

void test__ranhttp__request_parse_from_fd(ranhttp__request_t *request) {
    TEST_STARTED;
    int fd = open(TEST_FILE_PATH, O_RDONLY);
    DEBUG_LOG("fd: %d\n", fd);
    TEST_ASSERT(fd >= 0);
    int result = ranhttp__request_parse_from_fd(request, fd);
    close(fd);
    DEBUG_LOG("result: %d\n", result);
    TEST_ASSERT(result == RANHTTP_REQUEST_PARSER_ERROR_NONE);
    TEST_ASSERT_STR_EQ(request->http_method, "POST");
    TEST_ASSERT_STR_EQ(request->path, "/api/path");
    TEST_ASSERT_STR_EQ(request->http_version, "HTTP/1.1");
    TEST_ASSERT_STR_EQ(request->query_params[0].name, "api-version");
    TEST_ASSERT_STR_EQ(request->query_params[0].value, "v1");
    TEST_ASSERT_STR_EQ(request->query_params[1].name, "p1");
    TEST_ASSERT_NULL(request->query_params[1].value);
    TEST_ASSERT_STR_EQ(request->query_params[2].name, "p2");
    TEST_ASSERT_NULL(request->query_params[2].value);
    TEST_ASSERT_STR_EQ(request->query_params[3].name, "p4");
    TEST_ASSERT_NOT_NULL(request->query_params[4].value);
    TEST_ENDED;
}

int main() {
    ranhttp__request_t request;
    test__ranhttp__request_init(&request);
    test__ranhttp__request_parse_from_fd(&request);
    test__ranhttp__request_destroy(&request);
    // test__ranhttp__request_parse_from_buffer(&request, TEST_BUFFER);
    return 0;
}
