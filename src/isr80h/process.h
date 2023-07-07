#ifndef ISR80H_PROCESS_H
#define ISR80H_PROCESS_H

struct interrupt_frame_t;
void *isr80h_proc_cmd_process_load_start(struct interrupt_frame_t *frame);
void *isr80h_proc_cmd_invoke_system_command(struct interrupt_frame_t *frame);
void *isr80h_proc_cmd_get_program_arguments(struct interrupt_frame_t *frame);
void *isr80h_proc_cmd_exit(struct interrupt_frame_t *frame);

#endif