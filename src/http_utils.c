// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http_core.h"

// note: rfc2068:2.2
#define CR 13 // Carriage return
#define LF 10 // Linefeed

struct http_status_entry {
  int code;
  char msg[HTTP_STATUS_STRING_LENGTH_MAX];
};

static int
check(int exp, const char *msg)
{
  if (exp == -1) {
    // TODO: replace with logging macro/function
    fprintf(stderr, "%s", msg);
    return exp;
  }
  return exp;
}

int
http_fill_request_buffer(char* buffer, size_t size, ssize_t msgsize, int socketfd)
{
  return 0;
}

int
http_parse_request(http_request_t *out, int socketfd)
{
  // http_parse_request(): 
  // Read data stream from `socketfd` and define a `http_request_t` at `out` location.
  //
  // Caller is expected to allocate and free memory pointed to by `out` pointer.
  
  if (out == NULL)
  {
    // TODO: use logging function. It is a critical server error.
    // so it is going to be sth like 5xx http response status 
    // and will produce an EMERG/ALERT/CRIT (?) log.
    return -1;
  }

  if (socketfd <= 0)
  {
    // TODO: use logging function. It is a critical server error.
    // so it is going to be sth like 5xx http response status 
    // and will produce an EMERG/ALERT/CRIT (?) log.
    return -1;
  }

  const size_t buffer_max = 4096;
  
  char buffer[buffer_max];
  int msgsize = 0;
  ssize_t bytes_read;

  int i = 0;
  while ((bytes_read = read(socketfd, buffer + msgsize, sizeof(buffer) - msgsize - 1)) > 0) {
    msgsize += bytes_read;
    if (msgsize > buffer_max-1 || buffer[msgsize - 1] == '\n')
      break;
  }

  check(bytes_read, "read error\n");

  printf("%s\n", buffer);
  return 0;
}

int
http_construct_response(http_response_t *out, http_request_t *request)
{
  return -1;
}

int
http_cache_set(http_response_t *response)
{
  return -1;
}

int
http_cache_get(http_response_t *response)
{
  return -1;
}

int
http_status_entry_bsearch(char *out, const int *key, const struct http_status_entry list[], size_t count)
{
  size_t left = 0;
  size_t right = count - 1;
  size_t middle;

  while (left <= right) {
    middle = (left + right) / 2;

    if (list[middle].code == *key) {
      strncpy(out, list[middle].msg, HTTP_STATUS_STRING_LENGTH_MAX);
      return 0;
    }

    if (list[middle].code > *key)
      right = middle - 1;
    else
      left = middle + 1;
  }

  return -1;
}

int
http_status_string_get(char *out, int status)
{
  //! IMPORTANT
  //! This struct MUST BE ASCENDINGLY SORTED by .code
  static struct http_status_entry http_status_list[] = {
    // Info 1xx
    { INFO_CONTINUE, INFO_CONTINUE_STRING },
    { INFO_SWITCH_PROTOCOLS, INFO_SWITCH_PROTOCOLS_STRING },
    { INFO_PROCESSING, INFO_PROCESSING_STRING },
    { INFO_EARLY_HINTS, INFO_EARLY_HINTS_STRING },

    // Success 2xx
    { SUCCESS_OK, SUCCESS_OK_STRING },
    { SUCCESS_CREATED, SUCCESS_CREATED_STRING },
    { SUCCESS_ACCEPTED, SUCCESS_ACCEPTED_STRING },
    { SUCCESS_NON_AUTHORITATIVE_INFORMATION, SUCCESS_NON_AUTHORITATIVE_INFORMATION_STRING },
    { SUCCESS_NO_CONTENT, SUCCESS_NO_CONTENT_STRING },
    { SUCCESS_RESET_CONTENT, SUCCESS_RESET_CONTENT_STRING },
    { SUCCESS_PARTIAL_CONTENT, SUCCESS_PARTIAL_CONTENT_STRING },
    { SUCCESS_MULTI_STATUS, SUCCESS_MULTI_STATUS_STRING },
    { SUCCESS_ALREADY_REPORTED, SUCCESS_ALREADY_REPORTED_STRING },
    { SUCCESS_THIS_IS_FINE, SUCCESS_THIS_IS_FINE_STRING },
    { SUCCESS_IM_USED, SUCCESS_IM_USED_STRING },

    // Redirect 3xx
    { REDIRECT_MULTIPLE_CHOICES, REDIRECT_MULTIPLE_CHOICES_STRING },
    { REDIRECT_MOVED_PERMANENTLY, REDIRECT_MOVED_PERMANENTLY_STRING },
    { REDIRECT_FOUND, REDIRECT_FOUND_STRING },
    { REDIRECT_SEE_OTHER, REDIRECT_SEE_OTHER_STRING },
    { REDIRECT_NOT_MODIFIED, REDIRECT_NOT_MODIFIED_STRING },
    { REDIRECT_USE_PROXY, REDIRECT_USE_PROXY_STRING },
    { REDIRECT_SWITCH_PROXY, REDIRECT_SWITCH_PROXY_STRING },
    { REDIRECT_TEMPORARY_REDIRECT, REDIRECT_TEMPORARY_REDIRECT_STRING },
    { REDIRECT_PERMANENT_REDIRECT, REDIRECT_PERMANENT_REDIRECT_STRING },

    // Error 4xx
    { ERROR_BAD_REQUEST, ERROR_BAD_REQUEST_STRING },
    { ERROR_UNAUTHORIZED, ERROR_UNAUTHORIZED_STRING },
    { ERROR_PAYMENT_REQUIRED, ERROR_PAYMENT_REQUIRED_STRING },
    { ERROR_FORBIDDEN, ERROR_FORBIDDEN_STRING },
    { ERROR_NOT_FOUND, ERROR_NOT_FOUND_STRING },
    { ERROR_METHOD_NOT_ALLOWED, ERROR_METHOD_NOT_ALLOWED_STRING },
    { ERROR_NOT_ACCEPTABLE, ERROR_NOT_ACCEPTABLE_STRING },
    { ERROR_PROXY_AUTHENTICATION_REQUIRED, ERROR_PROXY_AUTHENTICATION_REQUIRED_STRING },
    { ERROR_REQUEST_TIMEOUT, ERROR_REQUEST_TIMEOUT_STRING },
    { ERROR_CONFLICT, ERROR_CONFLICT_STRING },
    { ERROR_GONE, ERROR_GONE_STRING },
    { ERROR_LENGTH_REQUIRED, ERROR_LENGTH_REQUIRED_STRING },
    { ERROR_PRECONDITION_FAILED, ERROR_PRECONDITION_FAILED_STRING },
    { ERROR_PAYLOAD_TOO_LARGE, ERROR_PAYLOAD_TOO_LARGE_STRING },
    { ERROR_URI_TOO_LONG, ERROR_URI_TOO_LONG_STRING },
    { ERROR_UNSUPPORTED_MEDIA_TYPE, ERROR_UNSUPPORTED_MEDIA_TYPE_STRING },
    { ERROR_RANGE_NOT_SATISFIABLE, ERROR_RANGE_NOT_SATISFIABLE_STRING },
    { ERROR_EXPECTATION_FAILED, ERROR_EXPECTATION_FAILED_STRING },
    { ERROR_IM_A_TEAPOT, ERROR_IM_A_TEAPOT_STRING },
    { ERROR_PAGE_EXPIRED, ERROR_PAGE_EXPIRED_STRING },
    { ERROR_METHOD_FAILURE_OR_ENHANCE_YOUR_CALM, ERROR_METHOD_FAILURE_OR_ENHANCE_YOUR_CALM_STRING },
    { ERROR_MISDIRECTED_REQUEST, ERROR_MISDIRECTED_REQUEST_STRING },
    { ERROR_UNPROCESSABLE_ENTITY, ERROR_UNPROCESSABLE_ENTITY_STRING },
    { ERROR_LOCKED, ERROR_LOCKED_STRING },
    { ERROR_FAILED_DEPENDENCY, ERROR_FAILED_DEPENDENCY_STRING },
    { ERROR_TOO_EARLY, ERROR_TOO_EARLY_STRING },
    { ERROR_UPGRADE_REQUIRED, ERROR_UPGRADE_REQUIRED_STRING },
    { ERROR_PRECONDITION_REQUIRED, ERROR_PRECONDITION_REQUIRED_STRING },
    { ERROR_TOO_MANY_REQUESTS, ERROR_TOO_MANY_REQUESTS_STRING },
    { ERROR_HTTP_STATUS_CODE, ERROR_HTTP_STATUS_CODE_STRING },
    { ERROR_REQUEST_HEADER_FIELDS_TOO_LARGE, ERROR_REQUEST_HEADER_FIELDS_TOO_LARGE_STRING },
    { ERROR_LOGIN_TIME_OUT, ERROR_LOGIN_TIME_OUT_STRING },
    { ERROR_NO_RESPONSE, ERROR_NO_RESPONSE_STRING },
    { ERROR_RETRY_WITH, ERROR_RETRY_WITH_STRING },
    { ERROR_BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS, ERROR_BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS_STRING },
    { ERROR_UNAVAILABLE_FOR_LEGAL_REASONS, ERROR_UNAVAILABLE_FOR_LEGAL_REASONS_STRING },
    { ERROR_CLIENT_CLOSED_CONNECTION_PREMATURELY, ERROR_CLIENT_CLOSED_CONNECTION_PREMATURELY_STRING },
    { ERROR_TOO_MANY_FORWARDED_IP_ADDRESSES, ERROR_TOO_MANY_FORWARDED_IP_ADDRESSES_STRING },
    { ERROR_INCOMPATIBLE_PROTOCOL, ERROR_INCOMPATIBLE_PROTOCOL_STRING },
    { ERROR_REQUEST_HEADER_TOO_LARGE, ERROR_REQUEST_HEADER_TOO_LARGE_STRING },
    { ERROR_SSL_CERTIFICATE_ERROR, ERROR_SSL_CERTIFICATE_ERROR_STRING },
    { ERROR_SSL_CERTIFICATE_REQUIRED, ERROR_SSL_CERTIFICATE_REQUIRED_STRING },
    { ERROR_HTTP_REQUEST_SENT_TO_HTTPS_PORT, ERROR_HTTP_REQUEST_SENT_TO_HTTPS_PORT_STRING },
    { ERROR_INVALID_TOKEN, ERROR_INVALID_TOKEN_STRING },
    { ERROR_TOKEN_REQUIRED_OR_CLIENT_CLOSED_REQUEST, ERROR_TOKEN_REQUIRED_OR_CLIENT_CLOSED_REQUEST_STRING },

    // Crit (Server Error) 5xx
    { CRIT_INTERNAL_SERVER_ERROR, CRIT_INTERNAL_SERVER_ERROR_STRING },
    { CRIT_NOT_IMPLEMENTED, CRIT_NOT_IMPLEMENTED_STRING },
    { CRIT_BAD_GATEWAY, CRIT_BAD_GATEWAY_STRING },
    { CRIT_SERVICE_UNAVAILABLE, CRIT_SERVICE_UNAVAILABLE_STRING },
    { CRIT_GATEWAY_TIMEOUT, CRIT_GATEWAY_TIMEOUT_STRING },
    { CRIT_HTTP_VERSION_NOT_SUPPORTED, CRIT_HTTP_VERSION_NOT_SUPPORTED_STRING },
    { CRIT_VARIANT_ALSO_NEGOTIATES, CRIT_VARIANT_ALSO_NEGOTIATES_STRING },
    { CRIT_INSUFFICIENT_STORAGE, CRIT_INSUFFICIENT_STORAGE_STRING },
    { CRIT_LOOP_DETECTED, CRIT_LOOP_DETECTED_STRING },
    { CRIT_BANDWIDTH_LIMIT_EXCEEDED, CRIT_BANDWIDTH_LIMIT_EXCEEDED_STRING },
    { CRIT_NOT_EXTENDED, CRIT_NOT_EXTENDED_STRING },
    { CRIT_NETWORK_AUTHENTICATION_REQUIRED, CRIT_NETWORK_AUTHENTICATION_REQUIRED_STRING },
    { CRIT_WEB_SERVER_IS_RETURNING_AN_UNKNOWN_ERROR, CRIT_WEB_SERVER_IS_RETURNING_AN_UNKNOWN_ERROR_STRING },
    { CRIT_WEB_SERVER_IS_DOWN, CRIT_WEB_SERVER_IS_DOWN_STRING },
    { CRIT_CONNECTION_TIMED_OUT, CRIT_CONNECTION_TIMED_OUT_STRING },
    { CRIT_ORIGIN_IS_UNREACHABLE, CRIT_ORIGIN_IS_UNREACHABLE_STRING },
    { CRIT_A_TIMEOUT_OCCURRED, CRIT_A_TIMEOUT_OCCURRED_STRING },
    { CRIT_SSL_HANDSHAKE_FAILED, CRIT_SSL_HANDSHAKE_FAILED_STRING },
    { CRIT_INVALID_SSL_CERTIFICATE, CRIT_INVALID_SSL_CERTIFICATE_STRING },
    { CRIT_RAILGUN_LISTENER_TO_ORIGIN, CRIT_RAILGUN_LISTENER_TO_ORIGIN_STRING },
    { CRIT_THE_SERVICE_IS_OVERLOADED, CRIT_THE_SERVICE_IS_OVERLOADED_STRING },
    { CRIT_SITE_FROZEN, CRIT_SITE_FROZEN_STRING },
    { CRIT_UNAUTHORIZED, CRIT_UNAUTHORIZED_STRING },
    { CRIT_NETWORK_READ_TIMEOUT_ERROR, CRIT_NETWORK_READ_TIMEOUT_ERROR_STRING },
    { CRIT_NETWORK_CONNECT_TIMEOUT_ERROR, CRIT_NETWORK_CONNECT_TIMEOUT_ERROR_STRING },
  };

  static size_t status_code_count = sizeof(http_status_list) / sizeof(struct http_status_entry);

  return check(http_status_entry_bsearch(out, &status, http_status_list, status_code_count), "Status code not found.\n");
}
