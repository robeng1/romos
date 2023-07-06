#ifndef KERNEL_H
#define KERNEL_H

#define STDOUT 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 20

void start_kernel();
void print(const char *str);
void panic(const char *msg);

#define ERROR(value) (void *)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)

#endif