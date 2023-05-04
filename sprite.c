#include "sprite.h"

extern uint16_t hres;
extern uint16_t vres;
extern bool page;
static uint16_t background_width;
static uint16_t background_height;
static uint8_t *background;

Sprite*create_sprite(xpm_map_t xpm, int x, int y,int xspeed, int yspeed) {
  //allocate space for the "object"
  
  Sprite*sp = (Sprite*) malloc ( sizeof(Sprite));
  xpm_image_t img;
  
  if( sp == NULL ) return NULL;
  
  // read the sprite pixmap
  sp->map = xpm_load(xpm, XPM_8_8_8, &img);
  if( sp->map == NULL ) {
    free(sp);
    return NULL;
  }
  
  sp->width = img.width;
  sp->height = img.height;
  sp->x = x;
  sp->y = y;
  sp->xspeed = xspeed;
  sp->yspeed = yspeed;

  return sp;
} 


void create_background(xpm_map_t xpm){
  xpm_image_t img;
  background = xpm_load(xpm, XPM_8_8_8, &img);
  background_width = img.width;
  background_height = img.height;
}

void draw_sprite(Sprite* sp){
  uint32_t color;
  int x = sp->x;
  int y = sp->y;
  uint8_t *ptr;
  ptr = sp->map; 
  for(int col = y; col < y + sp->height; col++){
    for(int row = x; row < x + sp->width; row++){
      color = *ptr;
      ptr++;
      color |= *ptr << 8;
      ptr++;
      color |= *ptr << 16;
      ptr++;
      draw_pixel(row, col, color);
    }
  }
}

void draw_background(){
  uint32_t color;
  uint8_t *ptr;
  ptr = background;
  for(int col = 0; col < vres; col++){
    for(int row = 0; row < hres; row++){
      color = *ptr;
      ptr++;
      color |= *ptr << 8;
      ptr++;
      color |= *ptr << 16;
      ptr++;
      draw_pixel(row, col, color);
    }
    ptr += (background_width - hres)*3;
  }
}

void clear_sprite(Sprite* sp){
  uint32_t color;
  uint8_t *ptr;
  ptr = background;
  ptr += (sp->x + sp->y*background_width)*3;
  for(int col = sp->y ; col < sp->y  + sp->height; col++){
    for(int row = sp->x; row < sp->x + sp->width; row++){
      color = *ptr;
      ptr++;
      color |= *ptr << 8;
      ptr++;
      color |= *ptr << 16;
      ptr++;
      draw_pixel(row, col, color);
    }
    ptr += (background_width - sp->width)*3;
  }
}

void destroy_sprite(Sprite *sp)
{
  if (sp == NULL)
    return;

  if (sp->map)
    free(sp->map);

  free(sp);
  sp = NULL;
}

void move_sprite(Sprite* sp){
  // if(sp->x <= hres && sp->x >= 0 && sp->y <= vres && sp->y >= 0) clear_sprite(sp);
  sp->x += sp->xspeed;
  sp->y += sp->yspeed;
  if(sp->x <= hres && sp->x >= 0 && sp->y <= vres && sp->y >= 0) draw_sprite(sp);
}
