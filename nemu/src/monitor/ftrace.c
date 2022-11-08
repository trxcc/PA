#include <common.h>
#include <elf.h>
#include <cpu/decode.h>
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
char *ftrace_ans;

int FTRACE_CNT = 0;

void getStrTable (char *filepath) {
//  fp = stdout;
  if (filepath != NULL) {
    FILE *tmp_fp = fopen(filepath, "rb");
    Assert(tmp_fp, "Can not open '%s'", filepath);
    fp = tmp_fp;
  }
 
  ehdr = malloc(sizeof(Elf32_Ehdr));
  int x = fread(ehdr, sizeof(Elf32_Ehdr), 1, fp);
  // Find the section table
  assert(ehdr != NULL);
  uint32_t shdr_size = ehdr->e_shentsize * ehdr->e_shnum;
  printf("%u\n", ehdr->e_shnum);
  shdr = malloc(shdr_size);
  fseek(fp, ehdr->e_shoff, SEEK_SET);
  x = fread(shdr, shdr_size, 1, fp);
  assert(shdr != NULL);
  // Find the string table
  shstrtab = malloc(shdr[ehdr->e_shstrndx].sh_size);
  fseek(fp, shdr[ehdr->e_shstrndx].sh_offset, SEEK_SET);
  x = fread(shstrtab, shdr[ehdr->e_shstrndx].sh_size, 1, fp);
  assert(shstrtab != NULL);
  assert(x == x);
  printf("%u\n", ehdr->e_shnum);
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
  printf("%u\n", nr_symtab);
  for (uint32_t i = 0; i < nr_symtab; i++) {
    printf("i:%d\n", i);
    if (ELF32_ST_TYPE(SymTable[i].st_info) == STT_FUNC) {
      printf("i: %d,  %s\n", i, StrTable + SymTable[i].st_name);
      assert(0);
      funcnode[FTRACE_CNT].name = StrTable + SymTable[i].st_name;
      funcnode[FTRACE_CNT].start_addr = SymTable[i].st_value;
      funcnode[FTRACE_CNT].end_addr = SymTable[i].st_value + SymTable[i].st_size;
      ++FTRACE_CNT;
    }
  }
}

void ftrace_record(Decode *s) {
/*
  char tmp[5], tmp1[4];
  memcpy(tmp, p, 4 * sizeof(p[0]));
  printf("tmp: %s\n", tmp);
  if (strcmp(tmp, "call") == 0) printf("1111\n");
  else {
    memcpy(tmp1, tmp, 3);
    if (strcmp(tmp1, "jal") == 0) printf("2222\n");
  }*/
  char *tmp = ftrace_ans, *func_name;
  char tmp_str[5];
  char a1[] = "call", a2[] = "ret";
  if (s->Type == 0) memcpy(tmp_str, a1, sizeof(a1));
  else if (s->Type == 1) memcpy(tmp_str, a2, sizeof(a2));
  while (*tmp != '\0') { tmp += 1; }
  for(int i = 0; i < FTRACE_CNT; i++) {
    if (funcnode[i].start_addr <= s->jmpAddr && s->jmpAddr <= funcnode[i].end_addr) {
      func_name = funcnode[i].name;
      break;
    }
  }
  int x = sprintf(tmp, "\n0x%08x: %s [%s@0x%08x]", s->pc, tmp_str, func_name, s->jmpAddr); 
  assert(x == x);
}

void init_ftrace(char *filepath) {
  getStrTable(filepath);
  getFunc();
}

