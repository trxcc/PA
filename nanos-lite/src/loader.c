#include <proc.h>
#include <elf.h>
//#include <stdio.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
#define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__) || defined(__ISA_RISCV64__)
#define EXPECT_TYPE EM_RISCV
#elif
#error unsupported ISA __ISA__
#endif

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {

  void *Elf32_File = malloc(get_ramdisk_size());
  ramdisk_read(Elf32_File, 0, get_ramdisk_size());
  assert(Elf32_File != NULL);
  
  Elf_Ehdr *ehdr = malloc(sizeof(Elf_Ehdr));
  ramdisk_read(ehdr, 0, sizeof(Elf_Ehdr));
  assert(ehdr != NULL);
  assert(*(uint32_t *)ehdr->e_ident == 0x464c457f); 
  assert(ehdr->e_machine == EXPECT_TYPE);

  uint32_t phdr_size = ehdr->e_phentsize * ehdr->e_phnum;
  Elf_Phdr *phdr = malloc(phdr_size);
  ramdisk_read(phdr, ehdr->e_phoff, phdr_size);  
  assert(phdr != NULL);
  
  for (int i = 0; i < ehdr->e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      ramdisk_read((void *)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_filesz);
      memset((void *)(phdr[i].p_vaddr + phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  return ehdr->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

