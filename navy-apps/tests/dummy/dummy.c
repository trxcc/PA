#include <stdint.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern intptr_t _syscall_(intptr_t, intptr_t, intptr_t, intptr_t);

int main() {
  return _syscall_(SYS_yield, 0, 0, 0);
}
