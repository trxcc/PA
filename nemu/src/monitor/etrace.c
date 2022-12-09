#include <common.h>
#include <isa.h>
#include <cpu/etrace.h>

#define CSR(x) csr[get_csr_index(x)]
#define mtvec 0x305
#define mcause 0x342
#define mepc 0x341
#define mstatus 0x300

static uint64_t nr_etrace_node = 0;

exceptionNode etrace_node[MAX_EXCEPTION_NODE];

void etrace_exec() {
#ifdef CONFIG_ETRACE
  etrace_node[nr_etrace_node].MEPC    = CSR(   mepc);
  etrace_node[nr_etrace_node].MCAUSE  = CSR( mcause);
  etrace_node[nr_etrace_node].MTVEC   = CSR(  mtvec);
  etrace_node[nr_etrace_node].MSTATUS = CSR(mstatus);

  nr_etrace_node = (nr_etrace_node + 1) % MAX_EXCEPTION_NODE;
  etrace_cnt++;
#endif
}

//#define extend_printf(x, ...) do { printf(x, ...); log_write(x, ...); } while (0)

void print_etrace() {
  printf("\n");
  log_write("\n");
  uint64_t len = etrace_cnt > MAX_EXCEPTION_NODE ? MAX_EXCEPTION_NODE : etrace_cnt;
  for (uint64_t i = 0; i < len; i++) {
    if (i == nr_etrace_node) { printf("  --->  "); log_write("  --->  "); }
    else                     { printf("        "); log_write("        "); }
    printf("Etrace %lu: MEPC: %u, MCAUSE: %u, MTVEC: %u, MSTATUS: %u\n", i, etrace_node[i].MEPC, etrace_node[i].MCAUSE, etrace_node[i].MTVEC, etrace_node[i].MSTATUS);
    log_write("Etrace %lu: MEPC: %u, MCAUSE: %u, MTVEC: %u, MSTATUS: %u\n", i, etrace_node[i].MEPC, etrace_node[i].MCAUSE, etrace_node[i].MTVEC, etrace_node[i].MSTATUS);
  }
}
