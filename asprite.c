#include "asprite.h"

AnimSprite *create_asprite(uint8_t no_pic, char *pic1[], ...){
  AnimSprite*asp = malloc(sizeof(AnimSprite));
  // create a standard sprite with first pixmap
  asp->sp = create_sprite(pic1,0,0,0,0);
  // allocate array of pointers to pixmaps
  asp->map = malloc((no_pic)*sizeof(char*));
  // initialize the first pixmap
  asp->map[0] = asp->sp->map;
  // initialize the remainder with the variable arguments
  // iterate over the list of arguments
  va_list ap;
  va_start(ap, pic1);
  for(uint16_t i = 1; i <no_pic; i++ ) {
    char**tmp = va_arg(ap, char**);
    xpm_image_t img;
    
    asp->map[i] = xpm_load(tmp, XPM_INDEXED, &img);
    
    if( asp->map[i] == NULL|| img.width != asp->sp->width || img.height != asp->sp->height) {
      // failure: realease allocated memory
      for (uint16_t j = 1; j<i;j ++) free(asp->map[i]);
    
      free(asp->map);
      destroy_sprite(asp->sp);
      free(asp);
    
      va_end(ap);
    
      return NULL;
    }
  }va_end(ap);
}
