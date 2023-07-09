#include "stdio.h"
#include "os.h"
#include "stdlib.h"
#include <stdarg.h>
#include "string.h"

void putchar(char c)
{
  sys_putchar((char)c);
}

void printf(const char *s, ...)
{
  va_list ap;
  va_start(ap, s);
  vsprintf(NULL, putchar, s, ap);
  va_end(ap);
}