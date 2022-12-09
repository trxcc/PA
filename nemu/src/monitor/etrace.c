#include <common.h>
#include <isa.h>

#define CSR(x) csr[get_csr_index(x)]
#define mtvec 0x305
#define mcause 0x342
#define mepc 0x341
#define mstatus 0x300


