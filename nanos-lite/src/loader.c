#include <proc.h>
#include <elf.h>
#include <fs.h>
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

extern int fs_open(const char *, int, int);
extern size_t fs_read(int, void *, size_t);
extern size_t fs_write(int, const void *, size_t);
extern size_t fs_lseek(int, size_t, int);
extern int fs_close(int);
extern size_t fs_get_file_size(int);
extern size_t fs_get_file_off(int);

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);

  //void *Elf32_File = malloc(get_ramdisk_size());
  void *Elf32_File = malloc(fs_get_file_size(fd));
  //ramdisk_read(Elf32_File, 0, get_ramdisk_size());
  fs_lseek(fd, 0, SEEK_SET);
  fs_read(fd, Elf32_File, fs_get_file_size(fd));
  //assert(0);
  assert(Elf32_File != NULL);
  
  Elf_Ehdr *ehdr = malloc(sizeof(Elf_Ehdr));
  //ramdisk_read(ehdr, 0, sizeof(Elf_Ehdr));
  fs_lseek(fd, 0, SEEK_SET);
  fs_read(fd, ehdr, sizeof(Elf_Ehdr));
  assert(ehdr != NULL);
  assert(*(uint32_t *)ehdr->e_ident == 0x464c457f); 
  assert(ehdr->e_machine == EXPECT_TYPE);
  
  uint32_t phdr_size = ehdr->e_phentsize * ehdr->e_phnum;
  Elf_Phdr *phdr = malloc(phdr_size);
  printf("phdr_size: %u, phoff: %u\n", phdr_size, ehdr->e_phoff);
  //ramdisk_read(phdr, ehdr->e_phoff, phdr_size);  
  fs_lseek(fd, ehdr->e_phoff, SEEK_SET);
  fs_read(fd, phdr, phdr_size);
  assert(phdr != NULL);
 // assert(0);
  
  for (int i = 0; i < ehdr->e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      printf("p_vaddr: %u, p_offset: %u, p_filesz: %u\n", phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_filesz);
      //ramdisk_read((void *)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_filesz);
      fs_lseek(fd, phdr[i].p_offset + fs_get_file_off(fd), SEEK_SET);
      fs_read(fd, (void *)phdr[i].p_vaddr, phdr[i].p_filesz);
      memset((void *)(phdr[i].p_vaddr + phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  //assert(0);
  printf("0x%u\n", ehdr->e_entry);
  return ehdr->e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
  //assert(0);
}

