// SPDX-License-Identifier: MIT

#include <arpa/inet.h>
#include <config.h>
#include <ctype.h>
#include <stdint.h>
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
#define OPTION_ERROR 1

#define S_EQ(a, b) (strcmp (a, b) == 0)

int in4_socket (struct in_addr *sin4_addr, in_port_t sin4_port);
int in6_socket (struct in6_addr *sin6_addr, in_port_t sin6_port);

// Printf info for users
void printf_help ();
void printf_version ();

// Option handling functions
int handle_log_level (int *out, char *optarg);
int handle_log_output (char *out, char *optarg);
int handle_sin4_addr (struct in_addr *out, char *optarg);
int handle_sin_port (in_port_t *out, char *optarg);
int handle_sin6_addr (struct in6_addr *out, char *optarg);
int handle_target (char *out, char *optarg);

int check (int exp, const char *msg);

// Info options flags
static int verbose_flag;
static int help_flag;
static int version_flag;

// Log options
static int log_level = LOGL_WARN;
static char log_output[PATH_MAX];

// IPv4 options
static int sin4_only_flag = 0;
static struct in_addr sin4_addr;
static in_port_t sin4_port;

// IPv6 options
static int sin6_only_flag = 0;
static struct in6_addr sin6_addr;
static in_port_t sin6_port;

// Target options
static char target[PATH_MAX];

enum LONG_OPTS
{
  OPT_LOG_LEVEL = 1,
  OPT_LOG_OUTPUT,
  OPT_INET4_SOURCE,
  OPT_INET6_SOURCE,
  OPT_INET4_PORT,
  OPT_INET6_PORT,
  OPT_TARGET,
};

int
main (int argc, char *argv[])
{
  int c;

  while (1)
    {
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

      c = getopt_long (argc, argv, "", options, &option_index);
      if (c == -1)
        break;

      switch (c)
        {
        case OPT_LOG_LEVEL:
          check (handle_log_level (&log_level, optarg),
                 "log-level argument is invalid. Use --help flag to see "
                 "available options.\n");
          break;
        case OPT_LOG_OUTPUT:
          check (handle_log_output (log_output, optarg), "\n");
          break;
        case OPT_INET4_SOURCE:
          check (handle_sin4_addr (&sin4_addr, optarg), "\n");
          break;
        case OPT_INET6_SOURCE:
          check (handle_sin6_addr (&sin6_addr, optarg), "\n");
          break;
        case OPT_INET4_PORT:
          check (handle_sin_port (&sin4_port, optarg), "\n");
          break;
        case OPT_INET6_PORT:
          check (handle_sin_port (&sin6_port, optarg), "\n");
          break;
        case OPT_TARGET:
          check (handle_target (target, optarg), "\n");
          break;

        case '?':
          break;
        case 0:
          break;

        default:
          abort ();
        }
    }

  if (help_flag)
    {
      printf_help ();
      exit (EXIT_SUCCESS);
    }

  if (version_flag)
    {
      printf_version ();
      exit (EXIT_SUCCESS);
    }

  check(sin4_only_flag && sin6_only_flag, "--only4 argument conflicts with --only6 argument\n");

  exit (EXIT_SUCCESS);
}

int
in4_socket (struct in_addr *sin4_addr, in_port_t sin4_port)
{
}

int
in6_socket (struct in6_addr *sin6_addr, in_port_t sin6_port)
{
}

int
check (int exp, const char *msg)
{
  if (exp != 0)
    {
      // TODO: replace with logging macro/function
      fprintf (stderr, "%s", msg);
      exit (EXIT_FAILURE);
    }
  return exp;
}

int
handle_log_level (int *out, char *optarg)
{
  // Supported arguments
  // Numeric: 0-7
  // Keyword: EMERG, ALERT, CRIT, ERROR, WARN, NOTICE, INFO, DEBUG
  // More info in loglevels.h

  if (strlen (optarg) == 1 && isdigit (optarg[0]) && optarg[0] - '0' < 8)
    {
      *out = optarg[0] - '0';
      return 0;
    }

  if (S_EQ (optarg, LOGL_STRING_EMERG))
    {
      *out = LOGL_EMERG;
      return 0;
    }
  else if (S_EQ (optarg, LOGL_STRING_ALERT))
    {
      *out = LOGL_ALERT;
      return 0;
    }
  else if (S_EQ (optarg, LOGL_STRING_CRIT))
    {
      *out = LOGL_CRIT;
      return 0;
    }
  else if (S_EQ (optarg, LOGL_STRING_ERROR))
    {
      *out = LOGL_ERROR;
      return 0;
    }
  else if (S_EQ (optarg, LOGL_STRING_WARN))
    {
      *out = LOGL_WARN;
      return 0;
    }
  else if (S_EQ (optarg, LOGL_STRING_NOTICE))
    {
      *out = LOGL_NOTICE;
      return 0;
    }
  else if (S_EQ (optarg, LOGL_STRING_INFO))
    {
      *out = LOGL_INFO;
      return 0;
    }
  else if (S_EQ (optarg, LOGL_STRING_DEBUG))
    {
      *out = LOGL_DEBUG;
      return 0;
    }
  else
    return OPTION_ERROR;
}

int
handle_log_output (char *out, char *optarg)
{
  if (realpath (optarg, out) == NULL)
    return OPTION_ERROR;
  return 0;
}

int
handle_sin4_addr (struct in_addr *out, char *optarg)
{
  if (inet_aton (optarg, out) != 0)
    return OPTION_ERROR;
  return 0;
}

int
handle_sin_port (in_port_t *out, char *optarg)
{
  int temp = atoi(optarg);
  if (temp <= 0 || temp > UINT16_MAX) 
    return OPTION_ERROR;

  *out = (uint16_t) temp;
  return 0;
}

int
handle_sin6_addr (struct in6_addr *out, char *optarg)
{
  int status;
  if ( (status = inet_pton (AF_INET6, optarg, out)) == 1)
    return 0;
  else if (status == 0)
  {
    fprintf(stderr, "source6: does not contain a character string representing a valid network address.\n");
    return OPTION_ERROR;
  }
  else
  {
    perror("source6");
    return OPTION_ERROR;
  }
}

int
handle_target (char *out, char *optarg)
{
  char temp[PATH_MAX];
  if (realpath (optarg, temp) == NULL)
    return OPTION_ERROR;
  return 0;
}

void
printf_help ()
{
  static const char *help_text = "Usage: wsfs [OPTION]... \n"
                                 "\n"
                                 "Options:\n"
                                 "--help           Show this help page.\n"
                                 "--version        Show package version.\n"
                                 "\n"
                                 "Report bugs to: <" PACKAGE_URL "/issues>\n"
                                 "Wsfs home page: <" PACKAGE_URL ">\n";

  printf (help_text);
}

void
printf_version ()
{
  static const char *version_text = PACKAGE_STRING
      "\nCopyright (c) 2024 pavroto\n"
      "MIT License"
      "\n"
      "This is free software; you are free to change and redistribute it.\n"
      "There is NO WARRANTY, to the extent permitted by law.\n"
      "More information here: <https://mit-license.org/>\n";

  printf (version_text);
}
