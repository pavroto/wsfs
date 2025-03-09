// SPDX-License-Identifier: MIT

#ifndef _WSFS_CORE
#define _WSFS_CORE

#define WSFS_STR_T_CHAR(A) { .len=sizeof(A), .string=A }

typedef struct {
  size_t  len;
  char    *string;
} wsfs_str_t;

#endif
