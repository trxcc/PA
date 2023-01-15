/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

#define MSTATUS_IDX 0x300
#define IRQ_TIMER 0x80000007
extern word_t csr[];

#define set_mstatus() do{mstatus_decode.val = csr[0x300];}while(0)

#define get_mie(x) (((uint32_t)(x) & 0x8) >> 3)
#define get_mpie(x) (((uint32_t)(x) & 0x80) >> 7)

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  // 0x305 for %%mtvec, 0x341 for %%mepc, 0x342 for %%mcause. 0x300 for %%mstatus
//  printf("%u\n", csr[0x305]);
  csr[0x341] = epc;
  csr[0x342] = NO;

  word_t new_pie = (get_mie(csr[MSTATUS_IDX]) << 7);
  csr[MSTATUS_IDX] = csr[MSTATUS_IDX] | new_pie;

  return csr[0x305];
}

word_t isa_query_intr() {
  if (get_mie(csr[MSTATUS_IDX]) == 1 && cpu.INTR) {
    cpu.INTR = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
