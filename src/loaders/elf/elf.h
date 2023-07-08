// This is a header guard to prevent multiple inclusions of the same header file.
#ifndef ELF_H
#define ELF_H

// These are standard C library headers for fixed-width integer types and general object size computations.
#include <stdint.h>
#include <stddef.h>

// These are flags used in the program header to specify permissions of the segment.
#define PF_X 0x01 // Segment is executable
#define PF_W 0x02 // Segment is writable
#define PF_R 0x04 // Segment is readable

// These are the types of segments in the program header.
#define PT_NULL 0    // Unused entry
#define PT_LOAD 1    // Loadable segment
#define PT_DYNAMIC 2 // Dynamic linking information
#define PT_INTERP 3  // Interpreter path
#define PT_NOTE 4    // Auxiliary information
#define PT_SHLIB 5   // Reserved
#define PT_PHDR 6    // Program header table

// These are the types of sections in the section header.
#define SHT_NULL 0     // Unused section
#define SHT_PROGBITS 1 // Program-specific data
#define SHT_SYMTAB 2   // Symbol table
#define SHT_STRTAB 3   // String table
#define SHT_RELA 4     // Relocation entries with addends
#define SHT_HASH 5     // Symbol hash table
#define SHT_DYNAMIC 6  // Dynamic linking information
#define SHT_NOTE 7     // Auxiliary information
#define SHT_NOBITS 8   // Section occupies no space in file
#define SHT_REL 9      // Relocation entries without addends
#define SHT_SHLIB 10   // Reserved
#define SHT_DYNSYM 11  // Dynamic linker symbol table
#define SHT_LOPROC 12  // Processor-specific semantics start
#define SHT_HIPROC 13  // Processor-specific semantics end
#define SHT_LOUSER 14  // Application-specific semantics start
#define SHT_HIUSER 15  // Application-specific semantics end

// These are the types of ELF files.
#define ET_NONE 0 // No file type
#define ET_REL 1  // Relocatable file
#define ET_EXEC 2 // Executable file
#define ET_DYN 3  // Shared object file
#define ET_CORE 4 // Core file

// These are indices into the identification array (e_ident).
#define EI_NIDENT 16 // Size of e_ident[]
#define EI_CLASS 4   // File class
#define EI_DATA 5    // Data encoding

// These are the file classes.
#define ELFCLASSNONE 0 // Invalid class
#define ELFCLASS32 1   // 32-bit objects
#define ELFCLASS64 2   // 64-bit objects

// These are the data encodings.
#define ELFDATANONE 0 // Invalid data encoding
#define ELFDATA2LSB 1 // Little-endian
#define ELFDATA2MSB 2 // Big-endian

// This is a special section index.
#define SHN_UNDEF 0 // Undefined, missing, irrelevant, or meaningless

// These are typedefs for 16-bit and 32-bit unsigned and signed integers.
typedef uint16_t elf32_half_t; // Unsigned half int (16-bit)
typedef uint32_t elf32_word_t; // Unsigned word (32-bit)
typedef int32_t elf32_sword_t; // Signed word (32-bit)
typedef uint32_t elf32_addr_t; // Unsigned program address (32-bit)
typedef int32_t elf32_off_t;   // Signed file offset (32-bit)

// This is the program header structure.
struct elf32_phdr_t
{
  elf32_word_t p_type;     // Type of segment
  elf32_off_t p_offset;    // Offset of segment in file
  elf32_addr_t p_vaddr;    // Virtual address of segment in memory
  elf32_addr_t p_paddr;    // Reserved
  elf32_word_t p_filesz;   // Size of segment in file
  elf32_word_t p_memsz;    // Size of segment in memory
  elf32_word_t p_flags;    // Segment flags
  elf32_word_t p_align;    // Segment alignment
} __attribute__((packed)); // This attribute ensures the structure is packed with no padding.

// This is the section header structure.
struct elf32_shdr_t
{
  elf32_word_t sh_name;      // Section name
  elf32_word_t sh_type;      // Section type
  elf32_word_t sh_flags;     // Section flags
  elf32_addr_t sh_addr;      // Section virtual address
  elf32_off_t sh_offset;     // Section offset in file
  elf32_word_t sh_size;      // Section size
  elf32_word_t sh_link;      // Link to another section
  elf32_word_t sh_info;      // Additional section information
  elf32_word_t sh_addralign; // Section alignment
  elf32_word_t sh_entsize;   // Entry size for fixed-size section entries
} __attribute__((packed));   // This attribute ensures the structure is packed with no padding.

// This is the ELF header structure.
struct elf_header_t
{
  unsigned char e_ident[EI_NIDENT]; // Identification array
  elf32_half_t e_type;              // File type
  elf32_half_t e_machine;           // Machine type
  elf32_word_t e_version;           // ELF format version
  elf32_addr_t e_entry;             // Entry point address
  elf32_off_t e_phoff;              // Program header offset
  elf32_off_t e_shoff;              // Section header offset
  elf32_word_t e_flags;             // Processor-specific flags
  elf32_half_t e_ehsize;            // ELF header size
  elf32_half_t e_phentsize;         // Program header entry size
  elf32_half_t e_phnum;             // Program header entry count
  elf32_half_t e_shentsize;         // Section header entry size
  elf32_half_t e_shnum;             // Section header entry count
  elf32_half_t e_shstrndx;          // Section header string table index
} __attribute__((packed));          // This attribute ensures the structure is packed with no padding.

// This is the dynamic section structure.
struct elf32_dyn_t
{
  elf32_sword_t d_tag; // Dynamic entry type
  union
  {
    elf32_word_t d_val; // Integer value
    elf32_addr_t d_ptr; // Address value
  } d_un;
} __attribute__((packed)); // This attribute ensures the structure is packed with no padding.

// This is the symbol table entry structure.
struct elf32_sym_t
{
  elf32_word_t st_name;    // Symbol name (string table index)
  elf32_addr_t st_value;   // Symbol value
  elf32_word_t st_size;    // Symbol size
  unsigned char st_info;   // Symbol type and binding
  unsigned char st_other;  // Symbol visibility
  elf32_half_t st_shndx;   // Section index
} __attribute__((packed)); // This attribute ensures the structure is packed with no padding.

// This function returns a pointer to the entry point of the ELF file.
void *elf_get_entry_ptr(struct elf_header_t *elf_header);

// This function returns the entry point address of the ELF file.
uint32_t elf_get_entry(struct elf_header_t *elf_header);

#endif // This ends the header guard.
