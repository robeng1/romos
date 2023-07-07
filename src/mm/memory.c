#include "memory.h"

void *memset(void *s, int c, size_t n)
{
  int i;
  char *ss = s;

  for (i = 0; i < n; i++)
    ss[i] = c;
  return s;
}

int memcmp(void *a, void *b, int count)
{
  char *c1 = a;
  char *c2 = b;
  while (count-- > 0)
  {
    if (*c1++ != *c2++)
    {
      return c1[-1] < c2[-1] ? -1 : 1;
    }
  }

  return 0;
}

void *memcpy(void *dest, void *src, int len)
{
  char *d = dest;
  char *s = src;
  while (len--)
  {
    *d++ = *s++;
  }
  return dest;
}