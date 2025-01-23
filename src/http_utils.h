// SPDX-License-Identifier: MIT

#ifndef _HTTP_FUNC
#define _HTTP_FUNC

int http_parse_request(http_request_t *out, int socketfd);
int http_construct_response(http_response_t *out, http_request_t *request);
int http_cache_set(http_response_t *response);
int http_cache_get(http_response_t *response);
int http_status_string_get(char *out, int status);

#endif
