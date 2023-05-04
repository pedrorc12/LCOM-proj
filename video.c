#include "video.h"

extern uint8_t *background;

uint16_t hres;
uint16_t vres;
static void* video_mem_active;         /*frame-buffer VM address*/
static void* video_mem_visual;
static void* video_mem_1;
static void* video_mem_2;
static bool page;               //true == video_mem 1; false == video_mem 2
static uint8_t bits_per_pixels;
static uint8_t bytes_per_pixels;
static uint8_t blue_mask;
static uint8_t red_mask;
static uint8_t green_mask;
static uint32_t scanlines;

int video_init(uint16_t *mode){
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));
  reg.intno = VBE_INT;
  reg.ah = VBE_AH;
  reg.al = VBE_FUNC_02;
  //reg.ax = 0x4F02;
  reg.bx = BIT(14) | *mode; 

  if (sys_int86(&reg) != OK){
    printf("Failed to call sys_int86\n");
    return 1;
  }
  return 0;
}

int video_map(vbe_mode_info_t *vbe_info){
  int r;
  struct minix_mem_range mr; /*physical memory range*/
  unsigned int vram_base = vbe_info->PhysBasePtr;  /*VRAM’s physical addresss*/
  unsigned int vram_size = (vbe_info->XResolution)*(vbe_info->YResolution)*(vbe_info->BitsPerPixel);  /*VRAM’s size, but you can usethe frame-buffer size, instead*/
  unsigned int vram_base_2 = vbe_info->PhysBasePtr + vram_size;

  bits_per_pixels = vbe_info->BitsPerPixel;
  hres = vbe_info->XResolution;
  vres = vbe_info->YResolution;
  red_mask = vbe_info->RedMaskSize;
  blue_mask = vbe_info->BlueMaskSize;
  green_mask = vbe_info->GreenMaskSize;
  bytes_per_pixels = bits_per_pixels/8;

  printf("bytes per pixel: %u\n", bytes_per_pixels);
  printf("hres: %u\n", hres);
  printf("vres: %u\n", vres);
  
  /*Allow memory mapping*/
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;
  
  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  
  /*Map memory*/
  video_mem_1 = vm_map_phys(SELF, (void*)mr.mr_base, vram_size);
  
  if(video_mem_1 == MAP_FAILED)panic("couldn’t map video memory");
  
  /*Allow memory mapping*/
  mr.mr_base = (phys_bytes) vram_base_2;
  mr.mr_limit = mr.mr_base + vram_size;
  
  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  
  /*Map memory*/
  video_mem_2 = vm_map_phys(SELF, (void*)mr.mr_base, vram_size);
  
  if(video_mem_2 == MAP_FAILED)panic("couldn’t map video memory");
  
  video_mem_active = video_mem_1;
  video_mem_visual = video_mem_2;
  
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));
  reg.intno = VBE_INT;
  reg.ax = VBE_AX_FUNC06;
  reg.bl = 0x01;
  if (sys_int86(&reg) != OK){
    printf("falied to call function 6\n");
    return 1;
  }
  uint16_t bytes_per_scanline = reg.bx;
  scanlines = (vram_size)/bytes_per_scanline;
  page = true;

  return 0;
}

void page_flip(){
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));
  reg.intno = VBE_INT;
  reg.ax = VBE_AX_FUNC07;
  reg.bl = 0x80;
  video_mem_visual = video_mem_active;
  if(page){
    video_mem_active = video_mem_2;
    reg.cx = 0;
    reg.dx = 0;
  } else{
    video_mem_active = video_mem_1;
    reg.cx = 0;
    reg.dx = scanlines;
  }

  if (sys_int86(&reg) != OK){
    printf("Failed to page flip");
  }
  page = !page;
}

void draw_pixel(uint16_t x, uint16_t y, uint32_t color){
  if((x < 0) || (x > hres) || (y < 0) || (y > vres) ) return;
  uint32_t temp;
  uint8_t *ptr = video_mem_active;

  // memccpy(ptr, &color, 0, bytes_per_pixels);
  if(color == CHROMA_KEY_GREEN_888){
    return;
  }
  else{
    ptr += (x + y*hres)* bytes_per_pixels;
    for(uint8_t j = 0; j < bits_per_pixels/8; j++, ptr++){
      temp = color;
      temp <<= (24 - j*8);
      temp >>= (24);
      if(temp == *ptr) return;
      *ptr = temp;
    }
  }
}

void draw_pixel_old(uint16_t x, uint16_t y, uint32_t color){
  if((x < 0) || (x > hres) || (y < 0) || (y > vres) ) return;
  uint32_t temp;
  uint8_t *ptr = video_mem_visual;

  // memccpy(ptr, &color, 0, bytes_per_pixels);
  if(color == CHROMA_KEY_GREEN_888){
    return;
  }
  else{
    ptr += (x + y*hres)* bytes_per_pixels;
    for(uint8_t j = 0; j < bits_per_pixels/8; j++, ptr++){
      temp = color;
      temp <<= (24 - j*8);
      temp >>= (24);
      *ptr = temp;
    }
  }
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
  for(unsigned int i = 0; i < hres; i++){
    if( (i >= x) && (i < x + len)){
      draw_pixel(i, y, color);
    }
  }
  return 0;
}


int video_get_mode_info(uint16_t mode, vbe_mode_info_t *vbe_info){
  reg86_t reg;
  phys_bytes buff;
  mmap_t m;

  lm_alloc(sizeof(vbe_mode_info_t), &m);
  buff = m.phys;
  memset(&reg, 0, sizeof(reg));
  reg.intno = VBE_INT;
  reg.ax = VBE_AH_FUNC01;
  reg.cx = mode;
  reg.es = PB2BASE(buff);
  reg.di = PB2OFF(buff);
  if (sys_int86(&reg) != OK){
    printf("Failed to call sys_int86\n");
    lm_free(&m);
    return 1;
  }

  *vbe_info = *(vbe_mode_info_t *)(m.virt);

  if(reg.ah != 0x00){
    printf("Falied to call function erro: 0x%x\n",reg.ax );
    lm_free(&m);
    return 1;
  }
  lm_free(&m);
  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){

  for(uint16_t line = 0; line < hres; line++){
    if(line >= y && line < y + height) 
      vg_draw_hline(x, line, width, color);
  }

  return 0;  
}

uint8_t video_get_index_color(uint16_t row, uint16_t col, uint8_t no_rectangles,  uint8_t step, uint32_t first){
  uint8_t color_byte;
  color_byte = first & 0xff;
  color_byte = (color_byte + (row * no_rectangles + col) * step) % (1 << bits_per_pixels);
  return color_byte;
}

uint32_t video_get_direct_color(uint16_t row, uint16_t col, uint8_t no_rectangles,  uint8_t step, uint32_t first){
  uint8_t R, G, B;
  uint32_t color;

  R = first >> (blue_mask + green_mask);
  G = first >> (blue_mask);
  B = first;
  
  R = (R + col * step) % (1 << red_mask);
	G = (G + row * step) % (1 << green_mask);
	B = (B + (col + row) * step) % (1 << blue_mask);

  color = R << (blue_mask + green_mask) | G << (blue_mask) | B;
  return color;
}
