#include "os.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
int main(int argc, char **argv)
{
  for (size_t i = 0; i < argc; i++)
  {
    printf("%s\n", argv[i]);
  }
  while (1)
  {
    /* code */
  }
  
  
  return 0;
}