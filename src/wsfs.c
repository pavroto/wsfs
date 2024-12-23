// SPDX-License-Identifier: MIT

#include <getopt.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "httpv.h"
#include "loglevels.h"

#define DEF_PORT 8080

void in4_socket (in_addr_t sin4_addr, in_port_t sin4_port);
void in6_socket (struct in6_addr *sin6_addr, in_port_t sin6_port);

static int verbose_flag;
static int log_level = LOGL_WARN;
static char log_output[PATH_MAX];

// IPv4 options
static int sin4_only_flag;
static in_addr_t sin4_addr;
static in_port_t sin4_port;

// IPv6 options
static int sin6_only_flag;
static struct in6_addr sin6_addr;
static in_port_t sin6_port;

enum LONG_OPTS
{
  OPT_VERBOSE = 1,
  OPT_HELP,
  OPT_VERSION,
  OPT_LOG_LEVEL,
  OPT_LOG_OUTPUT,
  OPT_INET4_SOURCE,
  OPT_INET6_SOURCE,
  OPT_INET4_PORT,
  OPT_INET6_PORT,
  OPT_INET4_ONLY,
  OPT_INET6_ONLY,
};

int
main (int argc, char *argv[])
{
  int c;

  while (1)
    {
      static struct option options[] = {
        // General
        { "verbose", no_argument, &verbose_flag, OPT_VERBOSE },
        { "help", no_argument, 0, OPT_HELP },
        { "version", no_argument, 0, OPT_VERSION },
        { "log-level", required_argument, 0, OPT_LOG_LEVEL },
        { "log-output", required_argument, 0, OPT_LOG_OUTPUT },

        // Source address
        { "source4", required_argument, 0, OPT_INET4_SOURCE },
        { "source6", required_argument, 0, OPT_INET6_SOURCE },

        // Source port
        { "port4", required_argument, 0, OPT_INET4_PORT },
        { "port6", required_argument, 0, OPT_INET6_PORT },

        // Only IPv4 or IPv6 (setting both will cause a crash on startup);
        { "only4", no_argument, 0, OPT_INET4_ONLY },
        { "only6", no_argument, 0, OPT_INET6_ONLY },

        // END
        { 0, 0, 0, 0 }
      };

      int option_index = 0;

      c = getopt_long (argc, argv, "", options, &option_index);
      if (c == -1)
        break;

      switch (c)
        {
        case OPT_HELP: 
          break;
        case OPT_VERSION:
          break;
        case OPT_LOG_LEVEL:
          break;
        case OPT_LOG_OUTPUT:
          break;
        case OPT_INET4_SOURCE:
          break;
        case OPT_INET6_SOURCE:
          break;
        case OPT_INET4_PORT:
          break;
        case OPT_INET6_PORT:
          break;
        case OPT_INET4_ONLY:
          break;
        case OPT_INET6_ONLY:
          break;

        default:
          abort ();
        }
    }

  exit(EXIT_SUCCESS); 
}

void
in4_socket (in_addr_t sin4_addr, in_port_t sin4_port)
{
}

void
in6_socket (struct in6_addr *sin6_addr, in_port_t sin6_port)
{
}
