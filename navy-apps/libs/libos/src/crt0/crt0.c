#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  char *empty[] =  {NULL };

  uintptr_t argc = *args;
  printf("argc: %d\n", (int)argc);
  
  uintptr_t *tmp = args;
  char **argv = (char **)(tmp + 1);

  for (int i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
  }

  for (tmp = tmp + 1; *tmp; tmp++);
  char **envp = (char **)envp;

  environ = empty;
  exit(main(argc, argv, envp));
  assert(0);
}
