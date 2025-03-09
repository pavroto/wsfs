#include <stdint.h>
#include <stdlib.h>

#include "wsfs_core.h"

wsfs_str_t*
wsfs_str_t_alloc(const size_t size)
{
  wsfs_str_t* ptr = (wsfs_str_t*)malloc(sizeof(wsfs_str_t));

  ptr->len = size;

  ptr->string = (char*)malloc(sizeof(char) * size);

  return ptr;
}

wsfs_str_t*
wsfs_str_t_calloc(const size_t size)
{
  wsfs_str_t* ptr = (wsfs_str_t*)calloc(1, sizeof(wsfs_str_t));

  ptr->len = size;

  ptr->string = (char*)malloc(sizeof(char) * size);

  return ptr;
}

void
wsfs_str_t_destroy(wsfs_str_t* ptr)
{
  free(ptr->string);
  free(ptr);
}

wsfs_str_t*
char_to_wsfs_str_t(wsfs_str_t* ptr, const char* s, const size_t len)
{
  if (ptr == NULL)
    ptr = wsfs_str_t_alloc(len+1);

  size_t i = 0;
  while ((ptr->string[i] = s[i]) != '\0')
    i++;

  ptr->string[i+1] = '\0'; // Add null terminator to the end of the string
  ptr->len = i;
  
  return ptr;
}
