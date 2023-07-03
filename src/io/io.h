#ifndef IO_H
#define IO_H
#include <stdint.h>

extern uint8_t read_byte(uint16_t port);
extern uint16_t read_word(uint16_t port);

extern void write_byte(uint16_t port, uint8_t value);
extern void wite_word(uint16_t port, uint16_t value);

#endif // IO_H
