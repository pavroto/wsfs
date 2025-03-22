// SPDX-License-Identifier: MIT

#ifndef _LOGGER
#define _LOGGER

#include <linux/limits.h>

extern int log_level;
extern char log_output[PATH_MAX];

int logger(int level, char *msg);

#endif
