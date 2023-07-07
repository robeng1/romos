#ifndef ISR80H_HEAP_H
#define ISR80H_HEAP_H

struct interrupt_frame_t;
void *isr80h_mem_cmd_malloc(struct interrupt_frame_t *frame);
void *isr80h_mem_cmd_free(struct interrupt_frame_t *frame);

#endif