#ifndef GAME_H
#define GAME_H
#include <lcom/lcf.h>

#include "video.h"
#include "sprite.h"
#include "kbc.h"
#include "Assets/background.h"
#include "Assets/Doodler.h"
#include "Assets/Plataform.h"
#include "Assets/Buttons.h"
#include "Assets/main_menu_logo.h"
#include "i8254.h"
#include "asprite.h"

#define FRAME_RATE 60
#define PRESS_SENSIBILITY 30
#define VELOCITY_SENSIBILITY 5
#define SPEED_LIMIT 20
#define GAME_SPEED 5
#define GRAVITY 1
#define IMPULSE 20

/**
 * Game state to control the game 
*/
enum game_state{
  main_menu,
  game_loop,
  confirmation,
  paused,
  game_over
}game_state;

/**
 * The game principal sprites 
*/
typedef struct 
{
  Sprite *doodler;
  Sprite *reverse_doodler;
  Sprite *obstacles[30];
  bool game_over;

} Game;

/**
 * Struct for a simple button 
*/
typedef struct{
  bool is_selected;
  Sprite *button;
  Sprite *selected_button;
}Button;

/**
 * Struct for the Buttons on the main menu 
*/
struct Menu{
  Sprite *menu_logo;
  Button play_button;
  Button exit;
} menu;

/**
 * Struct for the confirmation of exit menu 
*/
struct Confirmation_menu{
  Sprite *confirmation_bg;
  Button yes;
  Button no;
} confirmation_menu;

/**
 *  Struct of a Menu 
*/
typedef struct {
  Sprite *pause_bg;
  Button continue_game;
  Button return_main_menu;
} Pause;

/**
 * Subscribe to the IRQ's and initialize graphics card in mode, return 0 if succeed
 * @param mode mode to initalize grapics card
*/
int subscribe_interrupts(uint16_t mode);

/**
 * Unsubscribe IRQ and go back to text mode
*/
int game_end();

/**
 * Initialize the Sprites and menus inside de game 
*/
void game_initialize();

/**
 * Main loop where driver receive and process the interrupts
*/
int game_main_loop();

/**
 * Initalize menus for the main menu
*/
void main_menu_initialize();

/**
 * Draw the main menu with the UI updates
*/
void draw_main_menu();

/**
 * Draw a frame, uptdating sprites positions, verifying colisions and updating the UI for the menus 
*/
void draw_frame();

/**
 * Return true if ther is a collision between de obstacles and doodler 
*/
bool verify_collision();

#endif
