#include <assert.h>
#include <stdio.h>
#include <fixedptc.h>

#define ABS(a) ((a) >= 0 ? (a) : -(a))

int main() {
  fixedpt a = fixedpt_rconst(1.2);
  fixedpt b = fixedpt_fromint(10);
  int c = 0;
  if (b > (fixedpt_rconst(7.9))) {
    c = fixedpt_toint(fixedpt_div(fixedpt_mul(a + FIXEDPT_ONE, b), fixedpt_rconst(2.3)));
  }
  assert(c == 9);
  return 0;
}
