#ifndef KERNEL_H
#define KERNEL_H

#define STDOUT 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 20

void start_kernel();
void print(const char *str);

#endif