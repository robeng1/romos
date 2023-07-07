#ifndef GDT_H // This line starts an include guard. If the preprocessor variable GDT_H is already defined, this file will not be included again.
#define GDT_H // This line defines the preprocessor variable GDT_H. It signals that this file has been included.

#include <stdint.h> // This line includes the stdint.h header file, which defines integer types with specific widths.

// Here begins the declaration of a structure named gdt_entry. It represents a single entry in the Global Descriptor Table.
struct gdt_entry_t
{
  uint16_t segment;        // This field holds the size of the segment.
  uint16_t base_first;     // The lower 16 bits of the base address of the segment.
  uint8_t base;            // The next 8 bits of the base address of the segment.
  uint8_t access;          // The access flags determine the type of the segment and the privileges it requires.
  uint8_t high_flags;      // This field holds the high flags for this GDT entry. It can contain the granularity and operand size bits.
  uint8_t base_24_31_bits; // The highest 8 bits of the base address of the segment.
} __attribute__((packed)); // This line ensures that the compiler doesn't add any padding bytes to the struct. The packed attribute is necessary because the hardware expects the GDT entries to be laid out in a certain way.

// This structure represents a GDT pointer. It is used to load the GDT into the GDTR register.
struct gdt_ptr_t
{
  uint32_t base;  // The base address of the GDT.
  uint32_t limit; // The size of the GDT.
  uint8_t type;   // The type of descriptor (code segment, data segment, etc.)
};

// This function is likely used to load the GDT into the GDTR register.
extern void gdt_load(struct gdt_entry_t *gdt, int size);

// This function convert a higher-level, more structured representation of the GDT into the packed format used by the hardware.
void gdt_ptr_to_gdt(struct gdt_entry_t *gdt, struct gdt_ptr_t *gdt_ptr, int total_entries);

#endif // This line ends the include guard started at the beginning of the file.
