#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }
  //printf("Hit vme_init\n");
  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

#define VA_VPN_1(x) (((uintptr_t)(x) & 0xffc00000L) >> 22)
#define VA_VPN_0(x) (((uintptr_t)(x) & 0x003ff000L) >> 12)
//#define VA_IN_OFFSET(x) ((uintptr_t)(x) & 0x00000fffL)

#define PPN_MASK 0xfffffc00L

void map(AddrSpace *as, void *va, void *pa, int prot) {
//  printf("as->ptr: %u, va: %u, pa: %u\n", (uintptr_t)as->ptr, (uintptr_t)va, (uintptr_t)pa);
  PTE* pte = as->ptr + VA_VPN_1(va) * 4;
  if (!(*pte & PTE_V)) {
    //printf("pte not valid\n");
    //printf("pte: %u, *pte: %u\n", (uintptr_t)pte, (uint32_t)*pte);
    void *new_alloc_page = pgalloc_usr(PGSIZE);
    //((PPN >> 10) << 12) = va, so va should be devided by 4
    *pte = (*pte & ~PPN_MASK) | (PPN_MASK & ((uintptr_t)new_alloc_page >> 2));
    *pte = (*pte | PTE_V);
  }
  
  PTE *leaf_pte = (PTE *)((((*pte & PPN_MASK) >> 10) << 12) + VA_VPN_0(va) * 4);
  *leaf_pte = (PPN_MASK & (uintptr_t)pa >> 2) | PTE_V | PTE_R | PTE_W | PTE_X;
  //printf("leaf_pte: %u, *leaf_pte: %u\n", leaf_pte, (uint32_t)*leaf_pte);
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *context = kstack.end - sizeof(Context);
  context->mstatus = 0x1800;
  context->pdir = as->ptr;
  context->mepc = (uintptr_t)entry;
  printf("OK\n");
  //context->gpr[10] = (uintptr_t)heap.end;
  return context;
}
