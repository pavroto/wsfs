#include <linux/limits.h>
#include <stdio.h>
#include <unistd.h>

#include "log_levels.h"

int log_level = LOGL_WARN;
char log_output[PATH_MAX];

int
logger(int level, char *msg)
{
  if (level > log_level)
    return -1;

  fprintf(stderr, "%s", msg);
  return 0;
}

