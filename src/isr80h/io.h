#ifndef ISR80H_IO_H
#define ISR80H_IO_H

struct interrupt_frame_t;
void *isr80h_io_cmd_print(struct interrupt_frame_t *frame);
void *isr80h_io_cmd_getkey(struct interrupt_frame_t *frame);
void *isr80h_io_cmd_putchar(struct interrupt_frame_t *frame);
#endif