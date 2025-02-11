#include <stdio.h>
#include <time.h>

#include "log_levels.h"

int
logger(int level, char *msg)
{
  fprintf(stderr, "%s", msg);
  return -1;
}
