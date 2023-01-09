#include <memory.h>
#include <stdio.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *tmp = pf;
  pf += (nr_page << 12);
  printf("tmp: %u, pf: %u\n", (uintptr_t)tmp, (uintptr_t)pf);
  return tmp;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  return NULL;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
