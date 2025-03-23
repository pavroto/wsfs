// SPDX-License-Identifier: MIT

#ifndef _LOGGER
#define _LOGGER

#include <linux/limits.h>

#include "wsfs_core.h"

extern int log_level;
extern char log_output[PATH_MAX];

int logger(int level, wsfs_str_t* source, char *msg);

#endif
