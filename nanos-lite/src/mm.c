#include <memory.h>
#include <stdio.h>

static void *pf = NULL;
#define PAGE_SIZE (1 << 12)

void* new_page(size_t nr_page) {
  printf("new_page_pf: %u\n", (uintptr_t)pf);
  void *tmp = pf;
  pf += (nr_page << 12);
  printf("tmp: %u, pf: %u\n", (uintptr_t)tmp, (uintptr_t)pf);
  return tmp;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  printf("In pg_alloc, n = %d\n", n);
  assert(n % PAGE_SIZE == 0);
  void *page_head = new_page(n / PAGE_SIZE);
  memset(page_head, 0, n);
  return page_head;
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
  printf("init_pf: %u\n", (uintptr_t)pf);
#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
