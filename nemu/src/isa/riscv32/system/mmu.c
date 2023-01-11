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
#include <memory/paddr.h>
#include <memory/vaddr.h>

typedef uint32_t PTE;
extern word_t csr[];

#define PTE_V 0x01
#define PTE_R 0x02
#define PTE_W 0x04
#define PTE_X 0x08
#define PTE_U 0x10
#define PTE_A 0x40
#define PTE_D 0x80

#define satp 0x180
#define SATP_PPN(x) ((paddr_t)(x) & 0x003fffffL)
#define PTE_PPN(x) (((paddr_t)(x) & 0xfffffc00L) >> 10)
#define VA_VPN_0(x) (((vaddr_t)(x) & 0x003ff000L) >> 12)
#define VA_VPN_1(x) (((vaddr_t)(x) & 0xffc00000L) >> 22)
#define VA_VPO(x) ((vaddr_t)(x) & 0xfff)

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  printf("In isa_mmu_translate\n");
  paddr_t pte_addr = (SATP_PPN(csr[satp]) << 12) + VA_VPN_1(vaddr) * 4;
  PTE pte = paddr_read(pte_addr, 4);
  assert(pte & PTE_V);
  
  paddr_t leaf_pte_addr = (PTE_PPN(pte) << 12) + VA_VPN_0(vaddr) * 4;
  PTE leaf_pte = paddr_read(leaf_pte_addr, 4);
  assert(leaf_pte & PTE_V);

  paddr_t pa = (PTE_PPN(leaf_pte) << 12) + VA_VPO(vaddr);
  assert(pa == vaddr);
  return pa;
}











