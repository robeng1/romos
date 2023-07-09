#include "shell.h"
#include "stdio.h"
#include "stdlib.h"
#include "os.h"

int main(int argc, char **argv)
{
  printf("********************************Welcome RomOS!********************************\n");
  while (1)
  {
    print("$  ");
    char buf[1024];
    sys_terminal_readline(buf, sizeof(buf), true);
    print("\n");
    sys_system_run(buf);
    print("\n");
  }
  return 0;
}