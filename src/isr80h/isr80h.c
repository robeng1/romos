#include "isr80h.h"
#include "idt/idt.h"
#include "io.h"
#include "memory.h"
#include "process.h"

void isr80h_hookup_commands()
{
  // IO syscalls
  isr80h_register_command(__SYS_IO_CMD_PRINT, isr80h_io_cmd_print);
  isr80h_register_command(__SYS_IO_CMD_GETKEY, isr80h_io_cmd_getkey);
  isr80h_register_command(__SYS_IO_CMD_PUTCHAR, isr80h_io_cmd_putchar);

  // Memory syscalls
  isr80h_register_command(__SYS_MEM_CMD_MALLOC, isr80h_mem_cmd_malloc);
  isr80h_register_command(__SYS_MEM_FREE, isr80h_mem_cmd_free);

  // Process syscalls
  isr80h_register_command(__SYS_PROC_PROCESS_LOAD_START, isr80h_proc_cmd_process_load_start);
  isr80h_register_command(__SYS_PROC_INVOKE_SYSTEM_COMMAND, isr80h_proc_cmd_invoke_system_command);
  isr80h_register_command(__SYS_PROC_GET_PROGRAM_ARGUMENTS, isr80h_proc_cmd_get_program_arguments);
  isr80h_register_command(__SYS_PROC_EXIT, isr80h_proc_cmd_exit);
}