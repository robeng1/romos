#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"

uint16_t *vram = 0;
uint16_t t_row = 0;
uint16_t t_column = 0;

// takes a character and colour and outputs hex (in the correct endianess) representation for VRAM
uint16_t makechar(char c, char colour)
{
  return (colour << 8) | c;
}

void putchar(int row, int col, char c, char colour)
{
  vram[(row * VGA_WIDTH) + col] = makechar(c, colour);
}

void writechar(char c, char colour)
{
  if (c == '\n')
  {
    ++t_row;
    t_column = 0;
    return;
  }

  putchar(t_row, t_column, c, colour);
  ++t_column;
  if (t_column > VGA_WIDTH)
  {
    ++t_row;
    t_column = 0;
  }
}

void init_terminal()
{

  vram = (uint16_t* )(STDOUT);
  t_row = 0;
  t_column = 0;

  for (int y = 0; y < VGA_HEIGHT; ++y)
  {
    for (int x = 0; x < VGA_WIDTH; ++x)
    {
      putchar(y, x, ' ', 0);
    }
  }
}

size_t strlen(const char *str)
{
  size_t len = 0;
  while (str[len])
  {
    ++len;
  }
  return len;
}

void print(const char *str)
{
  size_t len = strlen(str);
  for (size_t i = 0; i < len; i++)
  {
    writechar(str[i], 15);
  }
}

void kernel_main()
{
  init_terminal();
  print("Welcome to RomOS, the only Operating System you'll ever need");

  // Initialize the interrupt descriptor table
  init_idt();
}