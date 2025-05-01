#ifndef __RANHTTP_MODULE_REQUEST_PARSER_HELPER_H__
#define __RANHTTP_MODULE_REQUEST_PARSER_HELPER_H__

#define RANHTTP_ALLOWED_GENERIC_REQUEST_HEADERS \
    "Accept", \
    "Accept-Charset", \
    "Accept-Encoding", \
    "Accept-Language", \
    "Authorization", \
    "Cache-Control", \
    "Connection", \
    "Content-Length", \
    "Content-Type", \
    "Date", \
    "Expect", \
    "From", \
    "Host", \
    "If-Match", \
    "If-Modified-Since", \
    "If-None-Match", \
    "If-Range", \
    "If-Unmodified-Since", \
    "Max-Forwards", \
    "Origin", \
    "Pragma", \
    "Proxy-Authorization", \
    "Range", \
    "Referer", \
    "TE", \
    "Upgrade", \
    "User-Agent", \
    "Via", \
    "Warning", \
    "Cookie"

#if defined(RANHTTP_APP_ENABLE_CORS) && RANHTTP_APP_ENABLE_CORS == 1
#define RANHTTP_REQUEST_PARSER_ALLOWED_REQUEST_HEADERS(...) \
    RANHTTP_ALLOWED_GENERIC_REQUEST_HEADERS, \
    "Access-Control-Request-Method", \
    "Access-Control-Request-Headers" \
    __VA_OPT__(,) __VA_ARGS__
#else
#define RANHTTP_REQUEST_PARSER_ALLOWED_REQUEST_HEADERS(...) \
    RANHTTP_ALLOWED_GENERIC_REQUEST_HEADERS \
    __VA_OPT__(,) __VA_ARGS__
#endif // RANHTTP_APP_ENABLE_CORS

#define RANHTTP_ALLOWED_GENERIC_RESPONSE_HEADERS \
    "Accept-Ranges", \
    "Age", \
    "Allow", \
    "Cache-Control", \
    "Connection", \
    "Content-Encoding", \
    "Content-Language", \
    "Content-Length", \
    "Content-Location", \
    "Content-MD5", \
    "Content-Range", \
    "Content-Disposition", \
    "Content-Transfer-Encoding", \
    "Content-Base", \
    "Content-Id", \
    "Content-Description"\
    "Content-Type", \
    "Date", \
    "ETag", \
    "Expires", \
    "Last-Modified", \
    "Link", \
    "Location", \
    "P3P", \
    "Pragma", \
    "Proxy-Authenticate", \
    "Refresh", \
    "Retry-After", \
    "Server", \
    "Set-Cookie", \
    "Strict-Transport-Security", \
    "Trailer", \
    "Transfer-Encoding", \
    "Upgrade", \
    "Vary", \
    "Via", \
    "Warning"

#if defined(RANHTTP_APP_ENABLE_CORS) && RANHTTP_APP_ENABLE_CORS == 1
#define RANHTTP_REQUEST_PARSER_ALLOWED_RESPONSE_HEADERS(...) \
    RANHTTP_ALLOWED_GENERIC_RESPONSE_HEADERS, \
    "Access-Control-Allow-Origin", \
    "Access-Control-Allow-Credentials", \
    "Access-Control-Allow-Headers", \
    "Access-Control-Allow-Methods", \
    "Access-Control-Expose-Headers", \
    "Access-Control-Max-Age", \
    "Access-Control-Request-Headers", \
    "Access-Control-Request-Method" \
    __VA_OPT__(,) __VA_ARGS__
#else
#define RANHTTP_REQUEST_PARSER_ALLOWED_RESPONSE_HEADERS(...) \
    RANHTTP_ALLOWED_GENERIC_RESPONSE_HEADERS \
    __VA_OPT__(,) __VA_ARGS__
#endif // RANHTTP_APP_ENABLE_CORS

#define RANHTTP_ALLOWED_DEFAULT_HTTP_METHODS \
    "GET", \
    "POST", \
    "PUT", \
    "DELETE", \
    "HEAD", \
    "OPTIONS", \
    "PATCH" \
    "TRACE"

#define RANHTTP_ALLOWED_DEFAULT_HTTP_VERSIONS \
    "HTTP/1.1"

#endif // __RANHTTP_MODULE_REQUEST_PARSER_HELPER_H__