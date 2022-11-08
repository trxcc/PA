#include <common.h>
#include <elf.h>
#include <cpu/ftrace.h>

static FILE *fp = NULL;
static Elf32_Ehdr *ehdr;
static Elf32_Shdr *shdr;
static Elf32_Sym *SymTable;
static char *shstrtab, *StrTable;
static uint32_t nr_symtab;

#define MAX_NODE 100

/*typedef struct{
  char *name;
  uint32_t start_addr, end_addr;
}FuncNode;*/

FuncNode funcnode[100];

static int cnt = 0;

void getStrTable (char *filepath) {
//  fp = stdout;
  if (filepath != NULL) {
    FILE *tmp_fp = fopen(filepath, "w");
    Assert(tmp_fp, "Can not open '%s'", filepath);
    fp = tmp_fp;
  }

  int x = fread(ehdr, sizeof(ehdr), 1, fp);

  // Find the section table
  shdr = malloc(ehdr->e_shentsize * ehdr->e_shnum);
  fseek(fp, ehdr->e_shoff, SEEK_SET);
  x = fread(shdr, ehdr->e_shentsize * ehdr->e_shnum, 1, fp);

  // Find the string table
  shstrtab = malloc(shdr[ehdr->e_shstrndx].sh_size);
  fseek(fp, shdr[ehdr->e_shstrndx].sh_offset, SEEK_SET);
  x = fread(shstrtab, shdr[ehdr->e_shstrndx].sh_size, 1, fp);

  assert(x == x);
  // Scan the section table
  for (int i = 0; i < ehdr->e_shnum; i++) {
    // Load the symbol table
    if (shdr[i].sh_type == SHT_SYMTAB && strcmp(shstrtab + shdr[i].sh_name, ".symtab") == 0) {
      SymTable = malloc(shdr[i].sh_size);
      fseek(fp, shdr[i].sh_offset, SEEK_SET);
      x = fread(SymTable, shdr[i].sh_size, 1, fp);
      nr_symtab = shdr[i].sh_size / sizeof(SymTable[0]);
    }
    // Load the string table in symbol table
    else if (shdr[i].sh_type == SHT_STRTAB && strcmp(shstrtab + shdr[i].sh_name, ".strtab") == 0) {
      StrTable = malloc(shdr[i].sh_size);
      fseek(fp, shdr[i].sh_offset, SEEK_SET);
      x = fread(StrTable, shdr[i].sh_size, 1, fp);
    }
    assert(x == x);
  }
} 

void getFunc() {
  for (uint32_t i = 0; i < nr_symtab; i++) {
    if (SymTable[i].st_info == STT_FUNC) {
      funcnode[cnt].name = StrTable + SymTable[i].st_name;
      funcnode[cnt].start_addr = SymTable[i].st_value;
      funcnode[cnt].end_addr = SymTable[i].st_value + SymTable[i].st_size;
      ++cnt;
    }
  }
}

void ftrace_record(char *p) {
  char tmp[5], tmp1[4];
  memcpy(tmp, p, 4 * sizeof(p[0]));
  if (strcmp(tmp, "call") == 1) printf("1111\n");
  else {
    memcpy(tmp1, tmp, 3);
    if (strcmp(tmp1, "jal") == 1) printf("2222\n");
  }
}

void init_ftrace(char *filepath) {
  getStrTable(filepath);
  getFunc();
}

