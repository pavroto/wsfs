// SPDX-License-Identifier: MIT

#include <arpa/inet.h>
#include <config.h>
#include <ctype.h>
#include <getopt.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http_core.h"
#include "http_utils.h"
#include "log_levels.h"
#include "logger.h"

#define DEF_PORT 8080
#define OPTION_ERROR 1
#define SERVER_BACKLOG 1

#define S_EQ(a, b) (strcmp(a, b) == 0)

enum LONG_OPTS {
  OPT_LOG_LEVEL = 1,
  OPT_LOG_OUTPUT,
  OPT_INET4_SOURCE,
  OPT_INET6_SOURCE,
  OPT_INET4_PORT,
  OPT_INET6_PORT,
  OPT_TARGET,
};

enum IP_MODE {
  IPV4 = 1,
  IPV6 = 2,
};

int in4_socket(struct in_addr *sin4_addr, in_port_t sin4_port);
int in6_socket(struct in6_addr *sin6_addr, in_port_t sin6_port);

int handle_connection(int client_socketfd);

// Printf info for users
void printf_help();
void printf_version();

// Option handling functions
int handle_log_level(int *out, char *argument);
int handle_log_output(char *out, char *argument);
int handle_sin4_addr(struct in_addr *out, char *argument);
int handle_sin_port(in_port_t *out, char *argument);
int handle_sin6_addr(struct in6_addr *out, char *argument);
int handle_target(char *out, char *argument);

static int check(int exp, const char *msg);

// General
static int pid6 = -1;
static uint8_t mode = IPV6 | IPV4;

// Info options flags
static int verbose_flag;
static int help_flag;
static int version_flag;

// IPv4 options
static int sin4_only_flag = 0;
static struct in_addr sin4_addr = { .s_addr = INADDR_ANY };
static in_port_t sin4_port = DEF_PORT;

// IPv6 options
static int sin6_only_flag = 0;
static struct in6_addr sin6_addr;
static in_port_t sin6_port = DEF_PORT;

// Target options
static char target[PATH_MAX];

int
main(int argc, char *argv[])
{
  int c;
  int in4_socketfd, in6_socketfd, in_socketfd;
  memcpy(&sin6_addr, &in6addr_any, sizeof(in6addr_any));

  int client_socketfd;
  size_t client_addr_size;

  while (1) {
    static struct option options[] = {
      // General
      { "verbose", no_argument, &verbose_flag, 1 },
      { "help", no_argument, &help_flag, 1 },
      { "version", no_argument, &version_flag, 1 },

      // Log
      { "log-level", required_argument, 0, OPT_LOG_LEVEL },
      { "log-output", required_argument, 0, OPT_LOG_OUTPUT },

      // Source address
      { "source4", required_argument, 0, OPT_INET4_SOURCE },
      { "source6", required_argument, 0, OPT_INET6_SOURCE },

      // Source port
      { "port4", required_argument, 0, OPT_INET4_PORT },
      { "port6", required_argument, 0, OPT_INET6_PORT },

      // Only IPv4 or IPv6 (setting both will cause an error on startup);
      { "only4", no_argument, &sin4_only_flag, 1 },
      { "only6", no_argument, &sin6_only_flag, 1 },

      // Target
      { "target", required_argument, 0, OPT_TARGET },

      // END
      { 0, 0, 0, 0 }
    };

    int option_index = 0;

    c = getopt_long(argc, argv, "", options, &option_index);
    if (c == -1)
      break;

    switch (c) {
    case OPT_LOG_LEVEL:
      check(handle_log_level(&log_level, optarg),
        "log-level argument is invalid. Use --help flag to see "
        "available options.\n");
      break;
    case OPT_LOG_OUTPUT:
      check(handle_log_output(log_output, optarg),
        "wsfs: --log-output fail.\n");
      break;
    case OPT_INET4_SOURCE:
      check(handle_sin4_addr(&sin4_addr, optarg),
        "wsfs: --source4 fail.\n");
      break;
    case OPT_INET6_SOURCE:
      check(handle_sin6_addr(&sin6_addr, optarg),
        "wsfs: --source6 fail.\n");
      break;
    case OPT_INET4_PORT:
      check(handle_sin_port(&sin4_port, optarg),
        "wsfs: --port4 fail.\n");
      break;
    case OPT_INET6_PORT:
      check(handle_sin_port(&sin6_port, optarg),
        "wsfs: --port6 fail.\n");
      break;
    case OPT_TARGET:
      check(handle_target(target, optarg), "wsfs: --target fail.\n");
      break;

    case '?':
      break;
    case 0:
      break;

    default:
      abort();
    }
  }

  if (help_flag) {
    printf_help();
    exit(EXIT_SUCCESS);
  }

  if (version_flag) {
    printf_version();
    exit(EXIT_SUCCESS);
  }

  check(sin4_only_flag && sin6_only_flag,
    "--only4 argument conflicts with --only6 argument\n");

  if (sin4_only_flag)
    mode ^= IPV6;
  if (sin6_only_flag)
    mode ^= IPV4;

  if (mode == (IPV6 | IPV4)) {
    // you may ask "Why fork()?"
    // Why not? <https://youtu.be/kVTx0JFQCkg?si=DJzJolab_20zk0I5&t=30>
    if ((pid6 = fork()) == 0)
      in_socketfd = in6_socketfd = in6_socket(&sin6_addr, sin6_port);
    else
      in_socketfd = in4_socketfd = in4_socket(&sin4_addr, sin4_port);
  } else if (mode == IPV6)
    in_socketfd = in6_socketfd = in6_socket(&sin6_addr, sin6_port);
  else
    in_socketfd = in4_socketfd = in4_socket(&sin4_addr, sin4_port);

  struct sockaddr_in client_addr;
  while (1) {
    client_addr_size = sizeof(struct sockaddr_in);
    check((client_socketfd = accept(in_socketfd, (struct sockaddr *)&client_addr,
             (socklen_t *)&client_addr_size))
        == -1,
      "accept failed.\n");
    handle_connection(client_socketfd);
  }

  exit(EXIT_SUCCESS);
}

int
handle_connection(int client_socketfd)
{
  http_request_t *http_client_request = (http_request_t *)malloc(sizeof(http_request_t));
  http_parse_request(http_client_request, client_socketfd);
  free(http_client_request);
  return 0;
}

int
in4_socket(struct in_addr *in4_addr, in_port_t in4_port)
{
  int opt = 1;
  int in4_socketfd;

  check(
    (in4_socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1, "ipv4: socket creation failed.\n");

  check(
    setsockopt(
      in4_socketfd,
      SOL_SOCKET,
      SO_REUSEADDR | SO_REUSEPORT,
      &opt, sizeof(opt)),
    "ipv4: socket option setting failed.\n");

  struct sockaddr_in in4_sockaddr;
  in4_sockaddr.sin_family = AF_INET;
  in4_sockaddr.sin_port = htons(in4_port);
  memcpy(&in4_sockaddr.sin_addr, in4_addr, sizeof(struct in_addr));

  check(
    bind(
      in4_socketfd,
      (struct sockaddr *)&in4_sockaddr,
      sizeof(in4_sockaddr)),
    "ipv4: bind failed.\n");

  check(listen(in4_socketfd, SERVER_BACKLOG), "ipv4: listen failed.\n");

  return in4_socketfd;
}

int
in6_socket(struct in6_addr *sin6_addr, in_port_t sin6_port)
{
  int opt = 1;
  int in6_socketfd;

  check(
    (in6_socketfd = socket(AF_INET6, SOCK_STREAM, 0)) == -1,
    "ipv6: socket creation failed.\n");

  check(
    setsockopt(
      in6_socketfd,
      SOL_SOCKET,
      SO_REUSEADDR | SO_REUSEPORT,
      &opt,
      sizeof(opt)),
    "ipv6: socket option setting failed.\n");

  check(
    setsockopt(
      in6_socketfd,
      IPPROTO_IPV6,
      IPV6_V6ONLY,
      &opt,
      sizeof(opt)),
    "ipv6: socket protocol level option setting failed.\n");

  struct sockaddr_in6 in6_sockaddr;
  in6_sockaddr.sin6_family = AF_INET6;
  in6_sockaddr.sin6_port = htons(sin6_port);

  // TODO: use user specified address from --source6 option
  in6_sockaddr.sin6_addr = in6addr_any;

  check(
    bind(
      in6_socketfd,
      (struct sockaddr *)&in6_sockaddr,
      sizeof(in6_sockaddr)),
    "ipv6: bind failed.\n");

  check(listen(in6_socketfd, SERVER_BACKLOG), "ipv6: listen failed.\n");

  return in6_socketfd;
}

static int
check(int exp, const char *msg)
{
  if (exp != 0) {
    // TODO: replace with logging macro/function
    fprintf(stderr, "%s", msg);
    exit(EXIT_FAILURE);
  }
  return exp;
}

int
handle_log_level(int *out, char *argument)
{
  // Supported arguments
  // Numeric: 0-7
  // Keyword: EMERG, ALERT, CRIT, ERROR, WARN, NOTICE, INFO, DEBUG
  // More info in loglevels.h

  if (strlen(argument) == 1 && isdigit(argument[0]) && argument[0] - '0' < 8) {
    *out = argument[0] - '0';
    return 0;
  }

  if (S_EQ(argument, LOGL_STRING_EMERG)) {
    *out = LOGL_EMERG;
    return 0;
  } else if (S_EQ(argument, LOGL_STRING_ALERT)) {
    *out = LOGL_ALERT;
    return 0;
  } else if (S_EQ(argument, LOGL_STRING_CRIT)) {
    *out = LOGL_CRIT;
    return 0;
  } else if (S_EQ(argument, LOGL_STRING_ERROR)) {
    *out = LOGL_ERROR;
    return 0;
  } else if (S_EQ(argument, LOGL_STRING_WARN)) {
    *out = LOGL_WARN;
    return 0;
  } else if (S_EQ(argument, LOGL_STRING_NOTICE)) {
    *out = LOGL_NOTICE;
    return 0;
  } else if (S_EQ(argument, LOGL_STRING_INFO)) {
    *out = LOGL_INFO;
    return 0;
  } else if (S_EQ(argument, LOGL_STRING_DEBUG)) {
    *out = LOGL_DEBUG;
    return 0;
  } else
    return OPTION_ERROR;
}

int
handle_log_output(char *out, char *argument)
{
  if (realpath(argument, out) == NULL)
    return OPTION_ERROR;
  return 0;
}

int
handle_sin4_addr(struct in_addr *out, char *argument)
{
  if (inet_aton(argument, out) == 0)
    return OPTION_ERROR;
  return 0;
}

int
handle_sin_port(in_port_t *out, char *argument)
{
  int temp = atoi(argument);
  if (temp <= 0 || temp > UINT16_MAX)
    return OPTION_ERROR;

  *out = (uint16_t)temp;
  return 0;
}

int
handle_sin6_addr(struct in6_addr *out, char *argument)
{
  int status;
  if ((status = inet_pton(AF_INET6, argument, out)) == 1)
    return 0;
  else if (status == 0) {
    fprintf(stderr,
      "source6: does not contain a character string representing a "
      "valid network address.\n");
    return OPTION_ERROR;
  } else {
    perror("source6");
    return OPTION_ERROR;
  }
}

int
handle_target(char *out, char *argument)
{
  char temp[PATH_MAX];
  if (realpath(argument, temp) == NULL)
    return OPTION_ERROR;
  return 0;
}

void
printf_help()
{
  static const char *help_text = "Usage: wsfs [OPTION]... \n"
                                 "\n"
                                 "Options:\n"
                                 "--help           Show this help page.\n"
                                 "--version        Show package version.\n"
                                 "\n"
                                 "Report bugs to: <" PACKAGE_URL "/issues>\n"
                                 "Wsfs home page: <" PACKAGE_URL ">\n";

  printf(help_text);
}

void
printf_version()
{
  static const char *version_text = PACKAGE_STRING
    "\nCopyright (c) 2024 pavroto\n"
    "MIT License"
    "\n"
    "This is free software; you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.\n"
    "More information here: <https://mit-license.org/>\n";

  printf(version_text);
}
