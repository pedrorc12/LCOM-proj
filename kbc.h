#ifndef kbc_h
#define kbc_h

#include <lcom/lcf.h>

//  KBC/Keyboard
#define KBC_IRQ1 1
#define KBC_OUT_BUF 0x60
#define KBC_WRITE_BUF 0x60
#define KBC_STAT_REG 0x64
#define KBC_IN_BUF 0x64
#define ESC_BREAKCODE 0x81
#define PARITY_ERROR BIT(7)
#define TIMEOUT_ERROR BIT(6)
#define MOUSE_DATA BIT(5)
#define KBC_IBF BIT(1)
#define KBC_OBF BIT(0)
#define WRITE_COMMAND_BYTE 0x60
#define WRITE_BYTE_TO_MOUSE_CMD 0xD4
#define DELAY 20000
#define LEFT_ARROW_PRESS 0xE04B
#define UP_ARROW_PRESS 0xE048
#define DOWN_ARROW_PRESS 0xE050
#define RIGHT_ARROW_PRESS 0xE04D
#define ENTER_PRESS 0x1C
 

//  Mouse
#define MOUSE_IRQ12 12
#define MOUSE_ACK 0xFA
#define MOUSE_NACK 0xFE
#define MOUSE_ERROR 0xFC
#define KEYBOARD_ENABLE_INTERRUPT_OBF BIT(0)
#define ENABLE_STREAM_MODE 0xEA
#define ENABLE_DATA_REPORT 0xF4
#define DISABLE_DATA_REPORT 0xF5

/**
 * Subscribe to the KBC IRQ line
 * @param bit_no  bit number to ben used in the driver receive loop
*/
int kbc_subscribe(uint8_t *bit_no);

/**
 * Unsubscribe to the KBC IRQ line 
*/
int kbc_unsubscribe();

/**
 *  Write a command to the KBC
 *  @param cmd commad to write to the KBC 
*/
int kbc_write_command(uint8_t cmd);

/**
 * Performe a poll to the KBC reading it's Output buffer 
*/
int kbc_polling();

/**
 * Subscrite to the mouse IRQ line
 * @param bit_no bit number to be used in the driver receiveloop
*/
int mouse_subscribe(uint8_t *bit_no);

/**
 * Unsubscribe to the mouse 
*/
int mouse_unsubscribe();

/**
 * Send a command direct to the mouse
 * @param command command to be sent to the mouse
*/
int write_command_to_mouse(uint8_t command);

/**
 * Performe a poll to tht KBC reading it output buffer if there is mouse information
*/
int mouse_polling();

#endif
