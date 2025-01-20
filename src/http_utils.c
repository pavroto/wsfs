// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <http_core.h>

// struct http_request
// {
//   int version;
//   int method;
//   char path[HTTP_PATH_MAX];
//
//   struct {
//     char name[HTTP_HEADERS_LENGTH_MAX];
//     char key[HTTP_HEADERS_LENGTH_MAX];
//   } headers[HTTP_HEADERS_MAX];
//   size_t header_count;
//
//   char body[HTTP_BODY_LENGTH_MAX];
// };

static int
check (int exp, const char *msg)
{
  if (exp == -1)
    {
      // TODO: replace with logging macro/function
      fprintf (stderr, "%s", msg);
      return exp;
    }
  return exp;
}

int
http_parse_request (struct http_request *out, int socketfd)
{
  char buffer[4096];
  int msgsize = 0;
  ssize_t bytes_read;

  int i = 0;
  while ((bytes_read
          = read (socketfd, buffer + msgsize, sizeof (buffer) - msgsize - 1))
         > 0)
    {
      msgsize += bytes_read;
      if (msgsize > 4095 || buffer[msgsize - 1] == '\n')
        break;
    }

  check (bytes_read, "read error\n");

  printf ("%s\n", buffer);
  return 0;
}

int
http_construct_response (struct http_response *out,
                         struct http_request *request)
{
  return 0;
}

int
http_cache_set (struct http_response *response)
{
  return 0;
}

int
http_cache_get (struct http_response *response)
{
  return 0;
}
