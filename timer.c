#include <lcom/lcf.h>

#include <stdint.h>

#include "i8254.h"

int timer_hook_id = TIMER0_IRQ;
uint32_t counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint16_t initial_value = TIMER_FREQ/freq;
  uint8_t st, lsb, msb, timer_port;
  uint8_t cmd;
  printf("initial value: 0x%x\n", initial_value);

  timer_get_conf(timer, &st);
  cmd = (st & 15);  //0b00001111
  printf("cmd: 0x%x\n", cmd);
  if (timer == 0){
    cmd = cmd | TIMER_SEL0;
    timer_port = TIMER_0;
  }
  else if (timer == 1){
    cmd = cmd | TIMER_SEL1;
    timer_port = TIMER_1;
    }
  else if (timer == 2){
    cmd = cmd | TIMER_SEL2;
    timer_port = TIMER_2;
    }
  
  cmd = cmd | TIMER_LSB_MSB;
  printf("cmd: 0x%x\n", cmd);
  if (sys_outb(TIMER_CTRL, cmd) != OK){ //change first paramenter to accept timer
    printf("FAILED sys_out");
    return 1;
  } 
  util_get_LSB(initial_value, &lsb);
  util_get_MSB(initial_value, &msb);
  if (sys_outb(timer_port, lsb) != OK){
    return 1;
  }
  if (sys_outb(timer_port, msb) != OK){
    return 1;
  }

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  timer_hook_id = *bit_no;
  
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id) != OK)
    return 1;
    
  *bit_no = BIT(*bit_no);  
  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&timer_hook_id) != OK)
    return 1;
   return 0;
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  //prof code
  uint8_t cmd = 0;
  cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | BIT(1); // read back command 0b11100010
  if (sys_outb(TIMER_CTRL, cmd) != OK){ 
    printf("ERROR in sys_outb\n");
    return 1;
  }
  if (util_sys_inb(TIMER_0, st) != OK){
    printf("ERROR in sys_inb\n");
    return 1;
  }
  
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val val;
  switch (field)
  {
  case(0):    //tfs_all
    val.byte = st;
    break;
  case(1):    //tfs_initial
    val.in_mode = (st >> 4) & 3;
    break;
  case(2):    //tsf_mode
    val.count_mode = (st >> 1) & 7;
    break;
  case(3):    //tfs_base
    val.bcd = st & 1;
    break;
  }
  if (timer_print_config(timer, field, val) != OK){
    printf("FAILED to print config");
    return 1;
  }
  return 0;
}
