#include "kbc.h"
#include "utils.c"

uint8_t scancode = 0;
uint8_t mouse_package = 0;
int hook_id = 0;
int mouse_hook_id = 0;
bool kbc_error_status = true;
bool two_bytes_scancode = false;
bool mouse_error = false;



int kbc_subscribe(uint8_t *bit_no){
  hook_id = *bit_no;
  
  if( sys_irqsetpolicy(KBC_IRQ1, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != OK)
    return 1;
  
  *bit_no = BIT(*bit_no);
  
  return 0;
}
int kbc_unsubscribe(){
  if(sys_irqrmpolicy(&hook_id) != OK)
    return 1;
  return 0;
}
int mouse_subscribe(uint8_t *bit_no){
  mouse_hook_id = *bit_no;

  if(sys_irqsetpolicy(MOUSE_IRQ12, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != OK) return 1;

  *bit_no = BIT(*bit_no);
  return 0;
}
int mouse_unsubscribe(){
  if(sys_irqrmpolicy(&mouse_hook_id) != OK) return 1;
  return 0;
}

void (kbc_ih)(){
  uint8_t status = 0;
 
  util_sys_inb(KBC_OUT_BUF, &scancode);
  util_sys_inb(KBC_STAT_REG, &status);

  if (scancode == 0xE0) two_bytes_scancode = true;
  
  if( (status & (PARITY_ERROR | TIMEOUT_ERROR)) == 0){
    kbc_error_status = false;
  } else kbc_error_status = true;
}

void (mouse_ih)(){
  uint8_t status = 0;

  util_sys_inb(KBC_STAT_REG, &status);
  if((status & (PARITY_ERROR | TIMEOUT_ERROR | MOUSE_DATA)) == MOUSE_DATA){
    util_sys_inb(KBC_OUT_BUF, &mouse_package);
    }else mouse_error = true;
}

int kbc_polling(){
  uint8_t status = 0;

  util_sys_inb(KBC_STAT_REG, &status);
  if((status & (PARITY_ERROR | TIMEOUT_ERROR | MOUSE_DATA | KBC_OBF)) == 1){
    util_sys_inb(KBC_OUT_BUF, &scancode);
    if (scancode == 0xE0) two_bytes_scancode = true;
    return 0;
  }
  return 1;
}

int mouse_polling(){
  uint8_t status = 0;

  util_sys_inb(KBC_STAT_REG, &status);
  if((status & (PARITY_ERROR | TIMEOUT_ERROR | KBC_OBF)) == 1){
    util_sys_inb(KBC_OUT_BUF, &mouse_package);
    return 0;
  }
  return 1;

}

int kbc_write_command(uint8_t cmd){
  uint8_t status;

  while(1)
  {
    util_sys_inb(KBC_STAT_REG, &status);
    if((status & KBC_IBF) == 0){
      sys_outb(KBC_IN_BUF, WRITE_COMMAND_BYTE);
      sys_outb(KBC_WRITE_BUF, cmd);
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY));
  }

}int write_command_to_mouse(uint8_t command){
  uint8_t cmd;
  uint8_t response;
  int tries = 3;
  cmd = WRITE_BYTE_TO_MOUSE_CMD;
  sys_outb(KBC_IN_BUF, cmd);
  cmd = command;
  sys_outb(KBC_WRITE_BUF, cmd);
  while (tries > 0)
  {
    util_sys_inb(KBC_OUT_BUF, &response);
    if(response == MOUSE_ACK){
      return 0;
    }
    else if((response == MOUSE_NACK) || (response == MOUSE_ERROR)){
      cmd = WRITE_BYTE_TO_MOUSE_CMD;
      sys_outb(KBC_IN_BUF, cmd);
      cmd = command;
      sys_outb(KBC_WRITE_BUF, cmd);
    }
    tickdelay(micros_to_ticks(DELAY));
    tries--;
  }
  return 1;
}
