#include <common.h>

extern void do_syscall(Context *);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD: 
//      printf("hh\n"); 
#ifdef __ISA_RISCV32__
      c->mepc += 4;
#endif
      break;
    case EVENT_SYSCALL:
//      printf("SYSCALL ing \n");
      do_syscall(c);
#ifdef __ISA_RISCV32__
      c->mepc += 4;
#endif      
      break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
