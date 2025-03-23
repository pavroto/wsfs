#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#include "log_levels.h"
#include "wsfs_core.h"

int log_level = LOGL_WARN;
char log_output[PATH_MAX];

int
log_level_string_get(wsfs_str_t* out, int level)
{
  switch (level)
  {
    case LOGL_EMERG:
      out->len = strlen(LOGL_STRING_EMERG);
      out->string = (char*)malloc(out->len + 1);
      out->string = LOGL_STRING_EMERG;
      return 0;
    case LOGL_ALERT:
      out->len = strlen(LOGL_STRING_ALERT);
      out->string = (char*)malloc(out->len + 1);
      out->string = LOGL_STRING_ALERT;
      return 0;
    case LOGL_CRIT:
      out->len = strlen(LOGL_STRING_CRIT);
      out->string = (char*)malloc(out->len + 1);
      out->string = LOGL_STRING_CRIT;
      return 0;
    case LOGL_ERROR:
      out->len = strlen(LOGL_STRING_ERROR);
      out->string = (char*)malloc(out->len + 1);
      out->string = LOGL_STRING_ERROR;
      return 0;
    case LOGL_WARN:
      out->len = strlen(LOGL_STRING_WARN);
      out->string = (char*)malloc(out->len + 1);
      out->string = LOGL_STRING_WARN;
      return 0;
    case LOGL_NOTICE:
      out->len = strlen(LOGL_STRING_NOTICE);
      out->string = (char*)malloc(out->len + 1);
      out->string = LOGL_STRING_NOTICE;
      return 0;
    case LOGL_INFO:
      out->len = strlen(LOGL_STRING_INFO);
      out->string = (char*)malloc(out->len + 1);
      out->string = LOGL_STRING_INFO;
      return 0;
    case LOGL_DEBUG:
      out->len = strlen(LOGL_STRING_DEBUG);
      out->string = (char*)malloc(out->len + 1);
      out->string = LOGL_STRING_DEBUG;
      return 0;
    default:
      return -1;
  }
}

int
logger(int level, wsfs_str_t* source, char *msg)
{
  time_t rawtime;
  struct tm * timeinfo;
  wsfs_str_t level_string;
  wsfs_str_t source_string = WSFS_STR_T_CHAR("INTERNAL");

  if (level > log_level)
    return -1;

  if (source != NULL)
    memcpy(&source_string, source, sizeof(wsfs_str_t));

  log_level_string_get(&level_string, level);

  time( &rawtime );
  timeinfo = localtime ( &rawtime );
  char timestamp_string[32];
  strftime(timestamp_string, 32, "%F %T %z", timeinfo);

  fprintf(stderr, "%s - [%s] - %s - \"%s\"\n", source_string.string, timestamp_string, level_string.string, msg);
  return 0;
}
