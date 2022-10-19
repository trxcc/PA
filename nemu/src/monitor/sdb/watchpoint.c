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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  word_t val;
  char EXPR[65536];
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp(){
  WP* temp;
  temp = free_;
  if (temp == NULL) assert(0);
  free_ = free_ -> next;
  temp -> next = NULL;
  if (head == NULL) { head = temp, temp->NO = 1; }
  else{
    WP* temp1;
    temp1 = head;
    int NO_cnt = 1;
    while(temp1 -> next != NULL) { temp1 = temp1->next; NO_cnt = temp1->NO; }
    temp1 -> next = temp;
    temp->NO = NO_cnt+1;
  }
  return temp;
}

void free_wp(WP *wp){
  if (wp == NULL) { assert(0); }
  if (wp == head) { head = head -> next; }
  else {
    WP *temp = head;
    while (temp != NULL && temp->next != wp){
      temp = temp->next;
    }
    temp->next = temp->next->next;
    temp = temp->next;
    while (temp->next != NULL) {++temp->NO;}
  }
  wp->next = free_;
  free_ = wp;
  wp->val = 0;
  wp->EXPR[0] = '\0';
}

extern word_t expr(char *e, bool *success);

bool check_wp(){
  WP* temp = head;
  bool flag = true, tmp_flag = true;
  while (temp != NULL) {
    printf("%s\n", temp->EXPR);
    word_t tmp_val = expr(temp->EXPR, &tmp_flag);
    if (tmp_val != temp->val) {
      flag = false;
      printf("Watchpoint %d: %s\n", temp->NO, temp->EXPR);
      printf("Old value is %u,\nNew value is %u\n", temp->val, tmp_val);
      temp->val = tmp_val;
    }
    temp = temp->next;
  }
  assert(0);
  if(!head) printf("%s = %u\n", head->EXPR, head->val);
  return flag;
}

void delete_wp(int ind) {
  WP* temp = head;
  while (temp != NULL) {
    if (temp->NO == ind) {
      free_wp(temp);
      break; 
    }
    temp = temp->next;
  }
  if (temp == NULL) {
    printf("Watchpoint %d doesn't exist!", ind);
    assert(0);
  }
}

void add_wp(char *e){
  WP* temp = new_wp();
  assert(temp != NULL);
  strcpy(temp->EXPR, e);
  bool success = true;
  temp->val = expr(e, &success);
  printf("%s = %u\n", temp->EXPR, temp->val);
  return;
}

void print_wp(){
  WP* temp = head;
  if (temp == NULL) {
    printf("No watchpoint now!\n");
    return;
  }
  else while (temp != NULL) {
    printf("%s\n", temp->EXPR);
    printf("Watchpoint %d: %s = %u\n", temp->NO, temp->EXPR, temp->val);
    temp = temp->next;
  }
  return;
}

/* TODO: Implement the functionality of watchpoint */

