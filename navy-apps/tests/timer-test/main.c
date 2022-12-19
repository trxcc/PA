#include <sys/time.h>
#include <stdio.h>

int main() {
  struct timeval val; 
  int cnt = 0;
  int now = gettimeofday(&val, NULL);
  long int pre = val.tv_usec;
  while (cnt <= 100) {
    int ret = gettimeofday(&val, NULL);
    if (ret == -1) {
      printf("Error in gettimeofday!\n");
      assert(0);
    }
    long int tmp = val.tv_usec;
    if (tmp - pre >= 1e5) {
      printf("print for %d at time %u\n", ++cnt, tmp);
    } 
  }
  return 0;
}
