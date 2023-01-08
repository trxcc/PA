#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

extern void naive_uload(PCB *pcb, const char *filename);
extern void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
extern void context_uload(PCB *pcb, const char *filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

int flag = 1;

void hello_fun(void *arg) {
  Log("Into hello_fun");
  int j = 1;
  //printf("Into hello for %d time", flag++);
  while (1) {
    Log("Hello World from Nanos-lite with arg %s for the %dth time!", (char *)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  //context_kload(&pcb[0], hello_fun, "CRT");
  //context_kload(&pcb[1], hello_fun, "TRX");
  switch_boot_pcb();
//  hello_fun(NULL);
  Log("Initializing processes...");
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/pal");
//  assert(0);
//  naive_uload(NULL, "/bin/pal");
  // load program here

}

Context* schedule(Context *prev) {
  current->cp = prev;
  //current = &pcb[0];
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
