#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

extern void __am_get_cur_as(Context *);
extern void __am_switch(Context *);

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 11:
        if (c->GPR1 == (uint32_t)-1) ev.event = EVENT_YIELD; 
        else ev.event = EVENT_SYSCALL; 
        break;
      default: ev.event = EVENT_ERROR; break;
    }
//    printf("mcause: %u, mstatus: %u, mepc: %u\n", c->mcause, c->mstatus, c->mepc);
    printf("I'm here\n");
    c = user_handler(ev, c);
    assert(c != NULL);
  }
  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *context = kstack.end - sizeof(Context);
  context->mstatus = 0x1800;
  context->mepc = (uintptr_t)entry;
  context->gpr[10] = (uintptr_t)arg;
  context->pdir = NULL;
  printf("Hit kcontext, entry = %u!\n", context->mepc);
  return context;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
