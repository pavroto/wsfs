// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http_core.h"
#include "wsfs_core.h"

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
http_fill_request_buffer(char *buffer, size_t size, ssize_t msgsize, int socketfd)
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

  if (out == NULL) {
    // TODO: use logging function. It is a critical server error.
    // so it is going to be sth like 5xx http response status
    // and will produce an EMERG/ALERT/CRIT (?) log.
    return -1;
  }

  if (socketfd <= 0) {
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
    if (msgsize > buffer_max - 1 || buffer[msgsize - 1] == '\n')
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
http_status_bsearch(wsfs_str_t *out, const http_status_code_t *key, const http_status_t list[], size_t count)
{
  // ssize_t is used instead of size_t to prevent underflow in case if status code is missing.
  ssize_t left = 0;
  ssize_t right = count - 1;
  ssize_t middle;

  while (left <= right) {
    middle = (left + right) / 2;
    printf("middle: %ld", middle);

    if (list[middle].code == *key) {
      size_t desired_size = strlen(list[middle].code_string.string);

      if (desired_size >= out->len)
      {
        if (out->string != NULL)
          free(out->string);

        out->string = (char*)malloc(desired_size + 1);
      }

      strncpy(out->string, list[middle].code_string.string, list[middle].code_string.len);
      out->string[desired_size] = '\0';
      out->len = desired_size;

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
http_status_string_get(wsfs_str_t *out, http_status_code_t status)
{
  //! IMPORTANT
  //! This struct MUST BE ASCENDINGLY SORTED by .code
  static http_status_t http_status_list[] = {
    // Info 1xx
    { INFO_CONTINUE, WSFS_STR_T_CHAR(INFO_CONTINUE_STRING) },
    { INFO_SWITCH_PROTOCOLS, WSFS_STR_T_CHAR(INFO_SWITCH_PROTOCOLS_STRING) },
    { INFO_PROCESSING, WSFS_STR_T_CHAR(INFO_PROCESSING_STRING) },
    { INFO_EARLY_HINTS, WSFS_STR_T_CHAR(INFO_EARLY_HINTS_STRING) },

    // Success 2xx
    { SUCCESS_OK, WSFS_STR_T_CHAR(SUCCESS_OK_STRING) },
    { SUCCESS_CREATED, WSFS_STR_T_CHAR(SUCCESS_CREATED_STRING) },
    { SUCCESS_ACCEPTED, WSFS_STR_T_CHAR(SUCCESS_ACCEPTED_STRING) },
    { SUCCESS_NON_AUTHORITATIVE_INFORMATION, WSFS_STR_T_CHAR(SUCCESS_NON_AUTHORITATIVE_INFORMATION_STRING) },
    { SUCCESS_NO_CONTENT, WSFS_STR_T_CHAR(SUCCESS_NO_CONTENT_STRING) },
    { SUCCESS_RESET_CONTENT, WSFS_STR_T_CHAR(SUCCESS_RESET_CONTENT_STRING) },
    { SUCCESS_PARTIAL_CONTENT, WSFS_STR_T_CHAR(SUCCESS_PARTIAL_CONTENT_STRING) },
    { SUCCESS_MULTI_STATUS, WSFS_STR_T_CHAR(SUCCESS_MULTI_STATUS_STRING) },
    { SUCCESS_ALREADY_REPORTED, WSFS_STR_T_CHAR(SUCCESS_ALREADY_REPORTED_STRING) },
    { SUCCESS_THIS_IS_FINE, WSFS_STR_T_CHAR(SUCCESS_THIS_IS_FINE_STRING) },
    { SUCCESS_IM_USED, WSFS_STR_T_CHAR(SUCCESS_IM_USED_STRING) },

    // Redirect 3xx
    { REDIRECT_MULTIPLE_CHOICES, WSFS_STR_T_CHAR(REDIRECT_MULTIPLE_CHOICES_STRING) },
    { REDIRECT_MOVED_PERMANENTLY, WSFS_STR_T_CHAR(REDIRECT_MOVED_PERMANENTLY_STRING) },
    { REDIRECT_FOUND, WSFS_STR_T_CHAR(REDIRECT_FOUND_STRING) },
    { REDIRECT_SEE_OTHER, WSFS_STR_T_CHAR(REDIRECT_SEE_OTHER_STRING) },
    { REDIRECT_NOT_MODIFIED, WSFS_STR_T_CHAR(REDIRECT_NOT_MODIFIED_STRING) },
    { REDIRECT_USE_PROXY, WSFS_STR_T_CHAR(REDIRECT_USE_PROXY_STRING) },
    { REDIRECT_SWITCH_PROXY, WSFS_STR_T_CHAR(REDIRECT_SWITCH_PROXY_STRING) },
    { REDIRECT_TEMPORARY_REDIRECT, WSFS_STR_T_CHAR(REDIRECT_TEMPORARY_REDIRECT_STRING) },
    { REDIRECT_PERMANENT_REDIRECT, WSFS_STR_T_CHAR(REDIRECT_PERMANENT_REDIRECT_STRING) },

    // Error 4xx
    { ERROR_BAD_REQUEST, WSFS_STR_T_CHAR(ERROR_BAD_REQUEST_STRING) },
    { ERROR_UNAUTHORIZED, WSFS_STR_T_CHAR(ERROR_UNAUTHORIZED_STRING) },
    { ERROR_PAYMENT_REQUIRED, WSFS_STR_T_CHAR(ERROR_PAYMENT_REQUIRED_STRING) },
    { ERROR_FORBIDDEN, WSFS_STR_T_CHAR(ERROR_FORBIDDEN_STRING) },
    { ERROR_NOT_FOUND, WSFS_STR_T_CHAR(ERROR_NOT_FOUND_STRING) },
    { ERROR_METHOD_NOT_ALLOWED, WSFS_STR_T_CHAR(ERROR_METHOD_NOT_ALLOWED_STRING) },
    { ERROR_NOT_ACCEPTABLE, WSFS_STR_T_CHAR(ERROR_NOT_ACCEPTABLE_STRING) },
    { ERROR_PROXY_AUTHENTICATION_REQUIRED, WSFS_STR_T_CHAR(ERROR_PROXY_AUTHENTICATION_REQUIRED_STRING) },
    { ERROR_REQUEST_TIMEOUT, WSFS_STR_T_CHAR(ERROR_REQUEST_TIMEOUT_STRING) },
    { ERROR_CONFLICT, WSFS_STR_T_CHAR(ERROR_CONFLICT_STRING) },
    { ERROR_GONE, WSFS_STR_T_CHAR(ERROR_GONE_STRING) },
    { ERROR_LENGTH_REQUIRED, WSFS_STR_T_CHAR(ERROR_LENGTH_REQUIRED_STRING) },
    { ERROR_PRECONDITION_FAILED, WSFS_STR_T_CHAR(ERROR_PRECONDITION_FAILED_STRING) },
    { ERROR_PAYLOAD_TOO_LARGE, WSFS_STR_T_CHAR(ERROR_PAYLOAD_TOO_LARGE_STRING) },
    { ERROR_URI_TOO_LONG, WSFS_STR_T_CHAR(ERROR_URI_TOO_LONG_STRING) },
    { ERROR_UNSUPPORTED_MEDIA_TYPE, WSFS_STR_T_CHAR(ERROR_UNSUPPORTED_MEDIA_TYPE_STRING) },
    { ERROR_RANGE_NOT_SATISFIABLE, WSFS_STR_T_CHAR(ERROR_RANGE_NOT_SATISFIABLE_STRING) },
    { ERROR_EXPECTATION_FAILED, WSFS_STR_T_CHAR(ERROR_EXPECTATION_FAILED_STRING) },
    { ERROR_IM_A_TEAPOT, WSFS_STR_T_CHAR(ERROR_IM_A_TEAPOT_STRING) },
    { ERROR_PAGE_EXPIRED, WSFS_STR_T_CHAR(ERROR_PAGE_EXPIRED_STRING) },
    { ERROR_METHOD_FAILURE_OR_ENHANCE_YOUR_CALM, WSFS_STR_T_CHAR(ERROR_METHOD_FAILURE_OR_ENHANCE_YOUR_CALM_STRING) },
    { ERROR_MISDIRECTED_REQUEST, WSFS_STR_T_CHAR(ERROR_MISDIRECTED_REQUEST_STRING) },
    { ERROR_UNPROCESSABLE_ENTITY, WSFS_STR_T_CHAR(ERROR_UNPROCESSABLE_ENTITY_STRING) },
    { ERROR_LOCKED, WSFS_STR_T_CHAR(ERROR_LOCKED_STRING) },
    { ERROR_FAILED_DEPENDENCY, WSFS_STR_T_CHAR(ERROR_FAILED_DEPENDENCY_STRING) },
    { ERROR_TOO_EARLY, WSFS_STR_T_CHAR(ERROR_TOO_EARLY_STRING) },
    { ERROR_UPGRADE_REQUIRED, WSFS_STR_T_CHAR(ERROR_UPGRADE_REQUIRED_STRING) },
    { ERROR_PRECONDITION_REQUIRED, WSFS_STR_T_CHAR(ERROR_PRECONDITION_REQUIRED_STRING) },
    { ERROR_TOO_MANY_REQUESTS, WSFS_STR_T_CHAR(ERROR_TOO_MANY_REQUESTS_STRING) },
    { ERROR_HTTP_STATUS_CODE, WSFS_STR_T_CHAR(ERROR_HTTP_STATUS_CODE_STRING) },
    { ERROR_REQUEST_HEADER_FIELDS_TOO_LARGE, WSFS_STR_T_CHAR(ERROR_REQUEST_HEADER_FIELDS_TOO_LARGE_STRING) },
    { ERROR_LOGIN_TIME_OUT, WSFS_STR_T_CHAR(ERROR_LOGIN_TIME_OUT_STRING) },
    { ERROR_NO_RESPONSE, WSFS_STR_T_CHAR(ERROR_NO_RESPONSE_STRING) },
    { ERROR_RETRY_WITH, WSFS_STR_T_CHAR(ERROR_RETRY_WITH_STRING) },
    { ERROR_BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS, WSFS_STR_T_CHAR(ERROR_BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS_STRING) },
    { ERROR_UNAVAILABLE_FOR_LEGAL_REASONS, WSFS_STR_T_CHAR(ERROR_UNAVAILABLE_FOR_LEGAL_REASONS_STRING) },
    { ERROR_CLIENT_CLOSED_CONNECTION_PREMATURELY, WSFS_STR_T_CHAR(ERROR_CLIENT_CLOSED_CONNECTION_PREMATURELY_STRING) },
    { ERROR_TOO_MANY_FORWARDED_IP_ADDRESSES, WSFS_STR_T_CHAR(ERROR_TOO_MANY_FORWARDED_IP_ADDRESSES_STRING) },
    { ERROR_INCOMPATIBLE_PROTOCOL, WSFS_STR_T_CHAR(ERROR_INCOMPATIBLE_PROTOCOL_STRING) },
    { ERROR_REQUEST_HEADER_TOO_LARGE, WSFS_STR_T_CHAR(ERROR_REQUEST_HEADER_TOO_LARGE_STRING) },
    { ERROR_SSL_CERTIFICATE_ERROR, WSFS_STR_T_CHAR(ERROR_SSL_CERTIFICATE_ERROR_STRING) },
    { ERROR_SSL_CERTIFICATE_REQUIRED, WSFS_STR_T_CHAR(ERROR_SSL_CERTIFICATE_REQUIRED_STRING) },
    { ERROR_HTTP_REQUEST_SENT_TO_HTTPS_PORT, WSFS_STR_T_CHAR(ERROR_HTTP_REQUEST_SENT_TO_HTTPS_PORT_STRING) },
    { ERROR_INVALID_TOKEN, WSFS_STR_T_CHAR(ERROR_INVALID_TOKEN_STRING) },
    { ERROR_TOKEN_REQUIRED_OR_CLIENT_CLOSED_REQUEST, WSFS_STR_T_CHAR(ERROR_TOKEN_REQUIRED_OR_CLIENT_CLOSED_REQUEST_STRING) },

    // Crit (Server Error) 5xx
    { CRIT_INTERNAL_SERVER_ERROR, WSFS_STR_T_CHAR(CRIT_INTERNAL_SERVER_ERROR_STRING) },
    { CRIT_NOT_IMPLEMENTED, WSFS_STR_T_CHAR(CRIT_NOT_IMPLEMENTED_STRING) },
    { CRIT_BAD_GATEWAY, WSFS_STR_T_CHAR(CRIT_BAD_GATEWAY_STRING) },
    { CRIT_SERVICE_UNAVAILABLE, WSFS_STR_T_CHAR(CRIT_SERVICE_UNAVAILABLE_STRING) },
    { CRIT_GATEWAY_TIMEOUT, WSFS_STR_T_CHAR(CRIT_GATEWAY_TIMEOUT_STRING) },
    { CRIT_HTTP_VERSION_NOT_SUPPORTED, WSFS_STR_T_CHAR(CRIT_HTTP_VERSION_NOT_SUPPORTED_STRING) },
    { CRIT_VARIANT_ALSO_NEGOTIATES, WSFS_STR_T_CHAR(CRIT_VARIANT_ALSO_NEGOTIATES_STRING) },
    { CRIT_INSUFFICIENT_STORAGE, WSFS_STR_T_CHAR(CRIT_INSUFFICIENT_STORAGE_STRING) },
    { CRIT_LOOP_DETECTED, WSFS_STR_T_CHAR(CRIT_LOOP_DETECTED_STRING) },
    { CRIT_BANDWIDTH_LIMIT_EXCEEDED, WSFS_STR_T_CHAR(CRIT_BANDWIDTH_LIMIT_EXCEEDED_STRING) },
    { CRIT_NOT_EXTENDED, WSFS_STR_T_CHAR(CRIT_NOT_EXTENDED_STRING) },
    { CRIT_NETWORK_AUTHENTICATION_REQUIRED, WSFS_STR_T_CHAR(CRIT_NETWORK_AUTHENTICATION_REQUIRED_STRING) },
    { CRIT_WEB_SERVER_IS_RETURNING_AN_UNKNOWN_ERROR, WSFS_STR_T_CHAR(CRIT_WEB_SERVER_IS_RETURNING_AN_UNKNOWN_ERROR_STRING) },
    { CRIT_WEB_SERVER_IS_DOWN, WSFS_STR_T_CHAR(CRIT_WEB_SERVER_IS_DOWN_STRING) },
    { CRIT_CONNECTION_TIMED_OUT, WSFS_STR_T_CHAR(CRIT_CONNECTION_TIMED_OUT_STRING) },
    { CRIT_ORIGIN_IS_UNREACHABLE, WSFS_STR_T_CHAR(CRIT_ORIGIN_IS_UNREACHABLE_STRING) },
    { CRIT_A_TIMEOUT_OCCURRED, WSFS_STR_T_CHAR(CRIT_A_TIMEOUT_OCCURRED_STRING) },
    { CRIT_SSL_HANDSHAKE_FAILED, WSFS_STR_T_CHAR(CRIT_SSL_HANDSHAKE_FAILED_STRING) },
    { CRIT_INVALID_SSL_CERTIFICATE, WSFS_STR_T_CHAR(CRIT_INVALID_SSL_CERTIFICATE_STRING) },
    { CRIT_RAILGUN_LISTENER_TO_ORIGIN, WSFS_STR_T_CHAR(CRIT_RAILGUN_LISTENER_TO_ORIGIN_STRING) },
    { CRIT_THE_SERVICE_IS_OVERLOADED, WSFS_STR_T_CHAR(CRIT_THE_SERVICE_IS_OVERLOADED_STRING) },
    { CRIT_SITE_FROZEN, WSFS_STR_T_CHAR(CRIT_SITE_FROZEN_STRING) },
    { CRIT_UNAUTHORIZED, WSFS_STR_T_CHAR(CRIT_UNAUTHORIZED_STRING) },
    { CRIT_NETWORK_READ_TIMEOUT_ERROR, WSFS_STR_T_CHAR(CRIT_NETWORK_READ_TIMEOUT_ERROR_STRING) },
    { CRIT_NETWORK_CONNECT_TIMEOUT_ERROR, WSFS_STR_T_CHAR(CRIT_NETWORK_CONNECT_TIMEOUT_ERROR_STRING) },
  };

  static size_t status_code_count = sizeof(http_status_list) / sizeof(http_status_t);

  return check(http_status_bsearch(out, &status, http_status_list, status_code_count), "Status code not found.\n");
}
