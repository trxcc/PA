#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

extern void naive_uload(PCB *pcb, const char *filename);
extern void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
extern void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

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
  context_kload(&pcb[0], hello_fun, "CRT");
  //context_kload(&pcb[1], hello_fun, "TRX");
//  hello_fun(NULL);
  Log("Initializing processes...");
  //context_uload(&pcb[0], "/bin/hello");
  char *argv[] = {"--skip", NULL};
  context_uload(&pcb[1], "/bin/exec-test", argv, NULL);
//  assert(0);
//  naive_uload(NULL, "/bin/pal");
  switch_boot_pcb();
  // load program here

}

static int cnt_proc = 1;

int execve(const char *filename, char *const argv[], char *const envp[]) {
  printf("cnt_proc: %d\n", cnt_proc);
  context_uload(&pcb[cnt_proc], filename, argv, envp);
  cnt_proc = (cnt_proc + 1) % (MAX_NR_PROC - 1);
  if (cnt_proc == 0) cnt_proc = (MAX_NR_PROC - 1);

  switch_boot_pcb();
  yield();
  return 0;
}

Context* schedule(Context *prev) {
  current->cp = prev;
  //current = &pcb[0];
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
