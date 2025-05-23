#ifndef __RANHTTP_MODULE_REQUEST_PARSER_CONFIG_H__
#define __RANHTTP_MODULE_REQUEST_PARSER_CONFIG_H__

#if defined(RANHTTP_APP_CONFIG_REQRES) && RANHTTP_APP_CONFIG_REQRES == 1
#include "@app/config-reqres.h"
#else
#include "request-parser/@helper.h"
#ifdef RANHTTP_APP_ALLOWED_REQUEST_HEADERS
#undef RANHTTP_APP_ALLOWED_REQUEST_HEADERS
#endif // RANHTTP_APP_ALLOWED_REQUEST_HEADERS
#define RANHTTP_APP_ALLOWED_REQUEST_HEADERS RANHTTP_REQUEST_PARSER_ALLOWED_REQUEST_HEADERS()

#ifdef RANHTTP_APP_ALLOWED_RESPONSE_HEADERS
#undef RANHTTP_APP_ALLOWED_RESPONSE_HEADERS
#endif // RANHTTP_APP_ALLOWED_RESPONSE_HEADERS
#define RANHTTP_APP_ALLOWED_RESPONSE_HEADERS RANHTTP_REQUEST_PARSER_ALLOWED_RESPONSE_HEADERS()

#endif // RANHTTP_APP_CONFIG_REQRES

#endif // __RANHTTP_MODULE_REQUEST_PARSER_CONFIG_H__