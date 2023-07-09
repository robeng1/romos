#include "loader.h"
#include "fs/file.h"

#include <stdbool.h>
#include "mm/memory.h"
#include "mm/heap/kernel_heap.h"
#include "string/string.h"
#include "mm/paging/paging.h"
#include "kernel/kernel.h"
#include "common/system.h"

const char elf_signature[] = {0x7f, 'E', 'L', 'F'};

static bool elf_valid_signature(void *buffer)
{
  return memcmp(buffer, (void *)elf_signature, sizeof(elf_signature)) == 0;
}

static bool elf_valid_class(struct elf_header_t *header)
{
  // We only support 32 bit binaries.
  return header->e_ident[EI_CLASS] == ELFCLASSNONE || header->e_ident[EI_CLASS] == ELFCLASS32;
}

static bool elf_valid_encoding(struct elf_header_t *header)
{
  return header->e_ident[EI_DATA] == ELFDATANONE || header->e_ident[EI_DATA] == ELFDATA2LSB;
}

static bool elf_is_executable(struct elf_header_t *header)
{
  return header->e_type == ET_EXEC && header->e_entry >= PROGRAM_VIRTUAL_ADDRESS;
}

static bool elf_has_program_header(struct elf_header_t *header)
{
  return header->e_phoff != 0;
}

void *elf_memory(struct elf_file_t *file)
{
  return file->elf_memory;
}

struct elf_header_t *elf_header(struct elf_file_t *file)
{
  return file->elf_memory;
}

struct elf32_shdr_t *elf_sheader(struct elf_header_t *header)
{
  return (struct elf32_shdr_t *)((int)header + header->e_shoff);
}

struct elf32_phdr_t *elf_pheader(struct elf_header_t *header)
{
  if (header->e_phoff == 0)
  {
    return 0;
  }

  return (struct elf32_phdr_t *)((int)header + header->e_phoff);
}

struct elf32_phdr_t *elf_program_header(struct elf_header_t *header, int index)
{
  return &elf_pheader(header)[index];
}

struct elf32_shdr_t *elf_section(struct elf_header_t *header, int index)
{
  return &elf_sheader(header)[index];
}

void *elf_phdr_phys_address(struct elf_file_t *file, struct elf32_phdr_t *phdr)
{
  return elf_memory(file) + phdr->p_offset;
}

char *elf_str_table(struct elf_header_t *header)
{
  return (char *)header + elf_section(header, header->e_shstrndx)->sh_offset;
}

void *elf_virtual_base(struct elf_file_t *file)
{
  return file->virtual_base_address;
}

void *elf_virtual_end(struct elf_file_t *file)
{
  return file->virtual_end_address;
}

void *elf_phys_base(struct elf_file_t *file)
{
  return file->physical_base_address;
}

void *elf_phys_end(struct elf_file_t *file)
{
  return file->physical_end_address;
}

int elf_validate_loaded(struct elf_header_t *header)
{
  return (elf_valid_signature(header) && elf_valid_class(header) && elf_valid_encoding(header) && elf_has_program_header(header)) ? ALL_OK : -EINFORMAT;
}

int elf_process_phdr_pt_load(struct elf_file_t *elf_file, struct elf32_phdr_t *phdr)
{
  if (elf_file->virtual_base_address >= (void *)phdr->p_vaddr || elf_file->virtual_base_address == 0x00)
  {
    elf_file->virtual_base_address = (void *)phdr->p_vaddr;
    elf_file->physical_base_address = elf_memory(elf_file) + phdr->p_offset;
  }

  unsigned int end_virtual_address = phdr->p_vaddr + phdr->p_filesz;
  if (elf_file->virtual_end_address <= (void *)(end_virtual_address) || elf_file->virtual_end_address == 0x00)
  {
    elf_file->virtual_end_address = (void *)end_virtual_address;
    elf_file->physical_end_address = elf_memory(elf_file) + phdr->p_offset + phdr->p_filesz;
  }
  return 0;
}
int elf_process_pheader(struct elf_file_t *elf_file, struct elf32_phdr_t *phdr)
{
  int res = 0;
  switch (phdr->p_type)
  {
  case PT_LOAD:
    res = elf_process_phdr_pt_load(elf_file, phdr);
    break;
  }
  return res;
}
int elf_process_pheaders(struct elf_file_t *elf_file)
{
  int res = 0;
  struct elf_header_t *header = elf_header(elf_file);
  for (int i = 0; i < header->e_phnum; i++)
  {
    struct elf32_phdr_t *phdr = elf_program_header(header, i);
    res = elf_process_pheader(elf_file, phdr);
    if (res < 0)
    {
      break;
    }
  }
  return res;
}

int elf_process_loaded(struct elf_file_t *elf_file)
{
  int res = 0;
  struct elf_header_t *header = elf_header(elf_file);
  res = elf_validate_loaded(header);
  if (res < 0)
  {
    goto out;
  }

  res = elf_process_pheaders(elf_file);
  if (res < 0)
  {
    goto out;
  }

out:
  return res;
}

int elf_load(const char *filename, struct elf_file_t **file_out)
{

  struct elf_file_t *elf_file = kernel_zalloc(sizeof(struct elf_file_t));
  int fd = 0;
  int res = fopen(filename, "r");
  if (res <= 0)
  {
    res = -EIO;
    goto out;
  }

  fd = res;
  struct file_stat_t stat;
  res = fstat(fd, &stat);
  if (res < 0)
  {
    goto out;
  }

  elf_file->elf_memory = kernel_zalloc(stat.filesize);
  res = fread(elf_file->elf_memory, stat.filesize, 1, fd);
  if (res < 0)
  {
    goto out;
  }

  res = elf_process_loaded(elf_file);
  if (res < 0)
  {
    goto out;
  }

  *file_out = elf_file;
out:
  fclose(fd);
  return res;
}

void elf_close(struct elf_file_t *file)
{
  if (!file)
    return;

  kernel_free(file->elf_memory);
  kernel_free(file);
}