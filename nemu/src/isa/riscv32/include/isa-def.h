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

#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>

typedef struct {
  word_t gpr[32];
  vaddr_t pc;
  bool INTR;
} riscv32_CPU_state;

union {
  struct {
    uint32_t wpri_0: 1;
    uint32_t sie: 1;
    uint32_t wpri_2: 1;
    uint32_t mie: 1;
    uint32_t wpri_4: 1;
    uint32_t spie: 1;
    uint32_t ube: 1;
    uint32_t mpie: 1;
    uint32_t spp: 1;
    uint32_t wpri_9_10: 2;
    uint32_t mpp: 2;
    uint32_t fs: 2;
    uint32_t xs: 2;
    uint32_t mprv: 2;
    uint32_t sum: 2;
    uint32_t mxr: 2;
    uint32_t tvm: 2;
    uint32_t tw: 2;
    uint32_t tsr: 2;
    uint32_t wpri_23_30: 8;
    uint32_t sd: 1;
  }m_decode;
  uint32_t val;
}mstatus_decode;

// decode
typedef struct {
  union {
    uint32_t val;
  } inst;
} riscv32_ISADecodeInfo;

extern word_t csr[];
#define satp 0x180

#define isa_mmu_check(vaddr, len, type) (((csr[satp] >> 31) & 0x1) ? MMU_TRANSLATE: MMU_DIRECT)

#endif
