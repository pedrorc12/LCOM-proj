#include <lcom/lcf.h>
#include <stdint.h>

//#define LAB3
#ifdef LAB3
int inb_counter;
#endif

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = val >> 8;
  return 0;
}

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t lst;
  if (sys_inb(port, &lst) != OK){
    printf("ERROR in sys_inb\n"); 
    return 1;
  }
  *value = (uint8_t) lst;
  //printf("lst value: 0x%x\n", lst);
  
  #ifdef LAB3
  inb_counter++;
  #endif
  return 0;
}

uint8_t util_get_byte(int which_byte, uint32_t word){
  word = word << (2 - which_byte);
  word = word >> 2;
  return word;
}
