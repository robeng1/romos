#ifndef ELFLOADER_H
#define ELFLOADER_H

#include <stdint.h>
#include <stddef.h>

#include "elf.h"
#include "config.h"

struct elf_file_t
{
  char filename[MAX_PATH];

  int in_memory_size;

  /**
   * The physical memory address that this elf file is loaded at
   */
  void *elf_memory;

  /**
   * The virtual base address of this binary
   */
  void *virtual_base_address;

  /**
   * The ending virtual address
   */
  void *virtual_end_address;

  /**
   * The physical base address of this binary
   */
  void *physical_base_address;

  /**
   * The physical end address of this bunary
   */
  void *physical_end_address;
};

int elf_load(const char *filename, struct elf_file_t **file_out);
void elf_close(struct elf_file_t *file);
void *elf_virtual_base(struct elf_file_t *file);
void *elf_virtual_end(struct elf_file_t *file);
void *elf_phys_base(struct elf_file_t *file);
void *elf_phys_end(struct elf_file_t *file);

struct elf_header_t *elf_header(struct elf_file_t *file);
struct elf32_shdr_t *elf_sheader(struct elf_header_t *header);
void *elf_memory(struct elf_file_t *file);
struct elf32_phdr_t *elf_pheader(struct elf_header_t *header);
struct elf32_phdr_t *elf_program_header(struct elf_header_t *header, int index);
struct elf32_shdr_t *elf_section(struct elf_header_t *header, int index);
void *elf_phdr_phys_address(struct elf_file_t *file, struct elf32_phdr_t *phdr);

#endif