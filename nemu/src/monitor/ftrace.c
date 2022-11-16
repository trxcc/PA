
#include <common.h>
#include <elf.h>
#include <cpu/decode.h>
#include <cpu/ftrace.h>

static FILE *fp = NULL;
static Elf32_Ehdr ehdr[1];
static Elf32_Shdr shdr[1];
static Elf32_Sym SymTable[1];
static char *shstrtab, *StrTable;
static uint32_t nr_symtab;


/*typedef struct{
  char *name;
  uint32_t start_addr, end_addr;
}FuncNode;*/

FuncNode funcnode[MAX_FUNC_NODE];
TraceNode tracenode[MAX_TRACE_NODE];
//char *ftrace_ans;

int FTRACE_CNT = 0, TRACE_NODE_CNT = 0;

void getStrTable (char *filepath) {
//  fp = stdout;
  if (filepath != NULL) {
    FILE *tmp_fp = fopen(filepath, "rb");
    Assert(tmp_fp, "Can not open '%s'", filepath);
    fp = tmp_fp;
  }
 
  int x = fread(ehdr, sizeof(Elf32_Ehdr), 1, fp);
  
  // Find the section table
  assert(ehdr != NULL);
  uint32_t shdr_size = ehdr->e_shentsize * ehdr->e_shnum;
  fseek(fp, ehdr->e_shoff, SEEK_SET);
  x = fread(shdr, shdr_size, 1, fp);
  assert(shdr != NULL);
  
  // Find the string table
  fseek(fp, shdr[ehdr->e_shstrndx].sh_offset, SEEK_SET);
  x = fread(shstrtab, shdr[ehdr->e_shstrndx].sh_size, 1, fp);
  assert(shstrtab != NULL);
  assert(x == x);
  
  // Scan the section table
  for (int i = 0; i < ehdr->e_shnum; i++) {
    // Load the symbol table
    if (shdr[i].sh_type == SHT_SYMTAB && strcmp(shstrtab + shdr[i].sh_name, ".symtab") == 0) {
      fseek(fp, shdr[i].sh_offset, SEEK_SET);
      x = fread(SymTable, shdr[i].sh_size, 1, fp);
      nr_symtab = shdr[i].sh_size / sizeof(SymTable[0]);
    }
    // Load the string table in symbol table
    else if (shdr[i].sh_type == SHT_STRTAB && strcmp(shstrtab + shdr[i].sh_name, ".strtab") == 0) {
      fseek(fp, shdr[i].sh_offset, SEEK_SET);
      x = fread(StrTable, shdr[i].sh_size, 1, fp);
    }
    assert(x == x);
  }
} 

void getFunc() {
  for (uint32_t i = 0; i < nr_symtab; i++) {
    if (ELF32_ST_TYPE(SymTable[i].st_info) == STT_FUNC) {
      Assert(FTRACE_CNT < MAX_FUNC_NODE, "Function node overflow!");
      printf("i: %d,  %s\n", i, StrTable + SymTable[i].st_name);
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
/*
  char *tmp = ftrace_ans, *func_name;
  char tmp_str[5];
  char a1[] = "call", a2[] = "ret";
  if (s->Type == 0) memcpy(tmp_str, a1, sizeof(a1));
  else if (s->Type == 1) memcpy(tmp_str, a2, sizeof(a2));
  assert(*tmp == '\0');
  while (*tmp != '\0') { tmp += 1; }
  for(int i = 0; i < FTRACE_CNT; i++) {
    if (funcnode[i].start_addr <= s->jmpAddr && s->jmpAddr <= funcnode[i].end_addr) {
      func_name = funcnode[i].name;
      break;
    }
  }
  int x = sprintf(tmp, "\n0x%08x: %s [%s@0x%08x]", s->pc, tmp_str, func_name, s->jmpAddr); 
  assert(x == x);*/
#ifdef CONFIG_FTRACE
  if (s->Type == -1) return;
  Assert(TRACE_NODE_CNT < MAX_TRACE_NODE, "Trace node overflow!");
  
  tracenode[TRACE_NODE_CNT].pc = s->pc;
  tracenode[TRACE_NODE_CNT].Type = s->Type;
  if(tracenode[TRACE_NODE_CNT].Type == 0) tracenode[TRACE_NODE_CNT].jmpAddr = s->jmpAddr; 
  else tracenode[TRACE_NODE_CNT].jmpAddr = s->pc;
  for (int i = 0; i < FTRACE_CNT; i++) {
    if (funcnode[i].start_addr <= tracenode[TRACE_NODE_CNT].jmpAddr && tracenode[TRACE_NODE_CNT].jmpAddr <= funcnode[i].end_addr) {
      tracenode[TRACE_NODE_CNT].name = funcnode[i].name;
      break;
    }
  }++TRACE_NODE_CNT;
#endif
}

void print_ftrace(){
  int space_num = 0;
  for (int i = 0; i < TRACE_NODE_CNT; i++) {
    printf("0x%08x: ", tracenode[i].pc);
    if (tracenode[i].Type == 0) ++space_num;
    else if (i != 0 && tracenode[i-1].Type != 0)--space_num;
    for (int i = 0; i < space_num; i++) { printf(" "); }
    if (tracenode[i].Type == 0) { printf("call [%s@0x%08x]\n", tracenode[i].name, tracenode[i].jmpAddr); }
    else { printf("ret [%s]\n", tracenode[i].name);
 }
      }
  printf("%d\n", TRACE_NODE_CNT);
}

void init_ftrace(char *filepath) {
  getStrTable(filepath);
  getFunc();
}

