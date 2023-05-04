#ifndef VIDEO_H
#define VIDEO_H

#include <lcom/lcf.h>

#define VBE_INT 0x10
#define VBE_AH 0x4F
#define VBE_FUNC_01 0x01
#define VBE_FUNC_02 0x02
#define VBE_FUNC_03 0x03
#define VBE_AH_FUNC01 0x4F01
#define VBE_AX_FUNC07 0x4F07
#define VBE_AX_FUNC06 0x4F06 
#define VBE_INDEX_MODE 0x105

/**
 * Initialize the graphics card in the specified mode
 * @param mode mode to initialize the graphics card
*/
int video_init(uint16_t *mode);

/**
 * Map the VRAM physical memory to the virtual memory
 * @param vbe_info information about the graphics card mode 
*/
int video_map(vbe_mode_info_t *vbe_info);

/**
 * Get the node information through the parameters
 * @param mode Mode to get the information from
 * @param vbe_info The infornation about the mode returned by address 
*/
int video_get_mode_info(uint16_t mode, vbe_mode_info_t *vbe_info);

/**
 * return the next color for the lab5 exercise
*/
uint8_t video_get_index_color(uint16_t row, uint16_t col, uint8_t no_rectangles,  uint8_t step, uint32_t first);

/**
 * return the next color for the lab5 exercise
*/
uint32_t video_get_direct_color(uint16_t row, uint16_t col, uint8_t no_rectangles,  uint8_t step, uint32_t first);

/**
 * Draw a pixel by putting the color on the frame buffer
 * @param x vertical line where the pixel to be drawn is located 
 * @param y horizontal line where the pixel to be drawn is located
 * @param color color to be display by the pixel drawn
*/
void draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/**
 * alters the video's card intern pointer of the frame buffer to another buffer
*/
void page_flip();
#endif
