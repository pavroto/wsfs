// SPDX-License-Identifier: MIT

#ifndef _HTTP_FUNC
#define _HTTP_FUNC

int http_parse_request (struct http_request *out, int socketfd);
int http_construct_response (struct http_response *out, struct http_request *request);
int http_cache_set (struct http_response *response);
int http_cache_get (struct http_response *response);
int http_status_string_get (char* out, int status);

#endif
