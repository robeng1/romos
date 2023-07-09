#include <stdint.h>
#include <common/system.h>
#include <common/printf.h>

void panic(const char *message, const char *file, uint32_t line)
{
  printf("PANIC(%s) at %s : %u\n", message, file, line);
  for (;;)
    ;
}

void log(const char *message, const char *file, uint32_t line)
{
  printf("LOG: (%s) at %s : %u\n", message, file, line);
}