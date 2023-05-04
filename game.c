#include "game.h"

extern uint8_t scancode;
extern uint32_t counter;
extern uint16_t vres;
extern uint16_t hres;

uint8_t timer_irq = 0, kbc_irq = 1;
int plataforms_counter = 0;

Game game;
Pause pause_menu;
Pause game_over_menu;

int subscribe_interrupts(uint16_t mode){
  vbe_mode_info_t vbe_info;
  game_state = main_menu;
  
  if(video_init(&mode) != OK){
    printf("Failed to initalize graphics\n");
    return 1;
  }

  if (video_get_mode_info(mode, &vbe_info) != OK){
    printf("Failed to get mode information\n");
    return 1;
  }

  if(timer_subscribe_int(&timer_irq) != OK){
    printf("Failed to subscribe timer\n");
    return 1;
  }

  if(kbc_subscribe(&kbc_irq) != OK){
    printf("Failed to subscribe to kbc\n");
    return 1;
  }
  
  if(video_map(&vbe_info) != OK){
    printf("Failed to map memory\n");
    return 1;
  }
  return 0;
}

void game_initialize(){
  game.doodler = create_sprite(Doodler_xpm, 0, 0, 0, 0);
  game.game_over = false;

  pause_menu.pause_bg = create_sprite(PauseMenu_xpm, hres/2 - 209, vres/2 - 142, 0, 0);

  pause_menu.continue_game.button = create_sprite(Continue_xpm, hres/2 - 209, vres/2 - 24, 0, 0);
  pause_menu.continue_game.selected_button = create_sprite(Continuewhite_xpm, hres/2 - 209, vres/2 - 24, 0, 0);
  pause_menu.continue_game.is_selected = true;

  pause_menu.return_main_menu.button = create_sprite(BackMainMenu_xpm, hres/2 - 209, vres/2 + 61, 0, 0);
  pause_menu.return_main_menu.selected_button = create_sprite(BackMainMenuWhite_xpm, hres/2 - 209, vres/2 + 61, 0, 0);
  pause_menu.return_main_menu.is_selected = false;

  game_over_menu.pause_bg = create_sprite(GameOverMenu_xpm, hres/2 - 209, vres/2 - 142, 0, 0);

  game_over_menu.continue_game.button = create_sprite(Continue_xpm, hres/2 - 209, vres/2 - 24, 0, 0);
  game_over_menu.continue_game.selected_button = create_sprite(Continuewhite_xpm, hres/2 - 209, vres/2 - 24, 0, 0);
  game_over_menu.continue_game.is_selected = true;

  game_over_menu.return_main_menu.button = create_sprite(BackMainMenu_xpm, hres/2 - 209, vres/2 + 61, 0, 0);
  game_over_menu.return_main_menu.selected_button = create_sprite(BackMainMenuWhite_xpm, hres/2 - 209, vres/2 + 61, 0, 0);
  game_over_menu.return_main_menu.is_selected = false;


  create_background(background_xpm);
  draw_background();
  for(int y = 0; y < vres; y += 60, plataforms_counter++){
    game.obstacles[plataforms_counter] = create_sprite(Plataform_xpm, (rand()%hres), y, 0, GAME_SPEED);
  }
}

void game_restart(){
  game.doodler = create_sprite(Doodler_xpm, 0, 0, 0, 0);
  game.game_over = false;
}

void draw_frame(){
  draw_background();
  if(game_state == game_loop){
    
    if(verify_collision() && game.doodler->yspeed >= 0){
      game.doodler->yspeed = -IMPULSE;
    }
      
    // plataforms movement
    for(int i = 0; i != plataforms_counter; i++){
      move_sprite(game.obstacles[i]);
      if(game.obstacles[i]->y > vres){
        destroy_sprite(game.obstacles[i]);
        game.obstacles[i] = create_sprite(Plataform_xpm, (rand()%hres), 0, 0, GAME_SPEED);
      }
    }
      
    if (game.doodler->yspeed < SPEED_LIMIT) game.doodler->yspeed += GRAVITY;
      
    // Doodler movement
    if(game.doodler->y > vres - game.doodler->height && !game.game_over) {
      clear_sprite(game.doodler);
      destroy_sprite(game.doodler);
      game.game_over = true;
      game_state = game_over;
    }
    else if (!game.game_over) move_sprite(game.doodler);

    // Doodler speed control
    if((game.doodler->xspeed != 0)){
      if(game.doodler->xspeed < 0) game.doodler->xspeed += VELOCITY_SENSIBILITY;
      else game.doodler->xspeed -= VELOCITY_SENSIBILITY;
    }
  }
  else if(game_state == paused){
    draw_sprite(pause_menu.pause_bg);

    if(pause_menu.continue_game.is_selected) draw_sprite(pause_menu.continue_game.selected_button);
    else draw_sprite(pause_menu.continue_game.button);
    
    if(pause_menu.return_main_menu.is_selected) draw_sprite(pause_menu.return_main_menu.selected_button);
    else draw_sprite(pause_menu.return_main_menu.button);
  }
  else if (game_state == game_over){
    draw_sprite(game_over_menu.pause_bg);

    if(game_over_menu.continue_game.is_selected) draw_sprite(game_over_menu.continue_game.selected_button);
    else draw_sprite(game_over_menu.continue_game.button);
    
    if(game_over_menu.return_main_menu.is_selected) draw_sprite(game_over_menu.return_main_menu.selected_button);
    else draw_sprite(game_over_menu.return_main_menu.button);
  }
  page_flip();
}


bool verify_collision(){
  for(int i = 0; i < plataforms_counter; i++){
    if(game.doodler->y + game.doodler->height >= game.obstacles[i]->y && game.doodler->y + game.doodler->height < game.obstacles[i]->y + game.obstacles[i]->height)
      if(game.doodler->x + game.doodler->width >= game.obstacles[i]->x)
        if(game.doodler->x <= game.obstacles[i]->x + game.obstacles[i]->width)
          return true;
  }
  return false;
}

void main_menu_initialize(){
  //Main menu
  menu.menu_logo = create_sprite(Logo_xpm, 0, 0, 0, 0);

  game_state = main_menu;
  menu.play_button.button = create_sprite(PlayButton_xpm, hres/2, vres/4, 0, 0);
  menu.play_button.selected_button = create_sprite(PlayButtonWhite_xpm, hres/2, vres/4, 0, 0);
  menu.play_button.is_selected = true;

  menu.exit.button = create_sprite(Exit_xpm, hres/2, vres/4 + 200, 0, 0);
  menu.exit.selected_button = create_sprite(ExitWhite_xpm, hres/2, vres/4 + 200, 0, 0);
  menu.exit.is_selected = false;

  //Confirmation menu
  confirmation_menu.confirmation_bg = create_sprite(Confirmation_xpm, hres/2 - 209, vres/2 - 142, 0, 0);
  
  confirmation_menu.yes.button = create_sprite(Yes_xpm, hres/2 - 209, vres/2, 0, 0);
  confirmation_menu.yes.selected_button = create_sprite(YesWhite_xpm, hres/2 - 209, vres/2, 0, 0);
  confirmation_menu.yes.is_selected = false;

  confirmation_menu.no.button = create_sprite(No_xpm, hres/2, vres/2, 0, 0);
  confirmation_menu.no.selected_button = create_sprite(NoWhite_xpm, hres/2, vres/2, 0, 0);
  confirmation_menu.no.is_selected = true;
}

void draw_main_menu(){
  draw_background();
  draw_sprite (menu.menu_logo);

  if(menu.play_button.is_selected) draw_sprite(menu.play_button.selected_button);
  else draw_sprite(menu.play_button.button);
    
  if(menu.exit.is_selected) draw_sprite(menu.exit.selected_button);
  else draw_sprite(menu.exit.button);
  
  if (game_state == confirmation){
    draw_sprite(confirmation_menu.confirmation_bg);

    if(confirmation_menu.yes.is_selected) draw_sprite(confirmation_menu.yes.selected_button);
    else draw_sprite(confirmation_menu.yes.button);
    
    if(confirmation_menu.no.is_selected) draw_sprite(confirmation_menu.no.selected_button);
    else draw_sprite(confirmation_menu.no.button);
  }

  page_flip();
}


int game_main_loop(){
  int r, ipc_status;
  message msg;
  uint16_t packet;
  bool close_game = false;

  main_menu_initialize();
  game_initialize();

  while (!close_game){
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0){
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)){
      switch (_ENDPOINT_P(msg.m_source))
      {
      case HARDWARE:
        if(msg.m_notify.interrupts & timer_irq){
          timer_int_handler();
          if(counter%(60/FRAME_RATE) == 0){
            switch (game_state)
            {
            case main_menu:
              draw_main_menu();
              break;
            case game_loop:
              draw_frame();
              break;
            case confirmation:
              draw_main_menu();
              break;
            case paused:
              draw_frame();
              break;
            case game_over:
              draw_frame();
              break;
            }
            counter = 0;
          }
        }

        if(msg.m_notify.interrupts & kbc_irq){
          kbc_ih();
          if(scancode == 0xe0) packet = scancode << 8;
          else{
            packet |= scancode;
            switch (game_state){
              case main_menu:
              if(packet == DOWN_ARROW_PRESS || packet == UP_ARROW_PRESS){
                if(menu.play_button.is_selected){
                  menu.play_button.is_selected = false;
                  menu.exit.is_selected = true;
                }
                else if(menu.exit.is_selected){
                  menu.exit.is_selected = false;
                  menu.play_button.is_selected = true;
                }
              }
              else if (packet == ENTER_PRESS){
                if(menu.play_button.is_selected) game_state = game_loop;
                else if(menu.exit.is_selected) game_state = confirmation;
              }
              break;
            
              case game_loop:
              if (!game.game_over){
                packet |= scancode;
                if(packet == UP_ARROW_PRESS && game.doodler->yspeed != SPEED_LIMIT) game.doodler->yspeed -= PRESS_SENSIBILITY;
                else if(packet == DOWN_ARROW_PRESS && game.doodler->yspeed != SPEED_LIMIT) game.doodler->yspeed += PRESS_SENSIBILITY;
                else if(packet == RIGHT_ARROW_PRESS && game.doodler->xspeed != SPEED_LIMIT) game.doodler->xspeed += PRESS_SENSIBILITY;
                else if(packet == LEFT_ARROW_PRESS && game.doodler->xspeed != SPEED_LIMIT) game.doodler->xspeed -= PRESS_SENSIBILITY;
              }
              if(packet == ESC_BREAKCODE) game_state = paused;
              break;

              case confirmation:
              if(packet == RIGHT_ARROW_PRESS || packet == LEFT_ARROW_PRESS){
                if(confirmation_menu.yes.is_selected){
                  confirmation_menu.yes.is_selected = false;
                  confirmation_menu.no.is_selected = true;
                }
                else if(confirmation_menu.no.is_selected){
                  confirmation_menu.no.is_selected = false;
                  confirmation_menu.yes.is_selected = true;
                }
              }
              else if (packet == ENTER_PRESS){
                if(confirmation_menu.yes.is_selected) close_game = true;
                else if(confirmation_menu.no.is_selected) game_state = main_menu;
              }
              break;
              
              case paused:
              if(packet == UP_ARROW_PRESS || packet == DOWN_ARROW_PRESS){
                if(pause_menu.continue_game.is_selected){
                  pause_menu.continue_game.is_selected = false;
                  pause_menu.return_main_menu.is_selected = true;
                }
                else if(pause_menu.return_main_menu.is_selected){
                  pause_menu.return_main_menu.is_selected = false;
                  pause_menu.continue_game.is_selected = true;
                }
              }
              else if (packet == ENTER_PRESS){
                if(pause_menu.continue_game.is_selected) game_state = game_loop;
                else if(pause_menu.return_main_menu.is_selected) game_state = main_menu;
              }
              break;
              case game_over:
              if(packet == UP_ARROW_PRESS || packet == DOWN_ARROW_PRESS){
                if(game_over_menu.continue_game.is_selected){
                  game_over_menu.continue_game.is_selected = false;
                  game_over_menu.return_main_menu.is_selected = true;
                }
                else if(game_over_menu.return_main_menu.is_selected){
                  game_over_menu.return_main_menu.is_selected = false;
                  game_over_menu.continue_game.is_selected = true;
                }
              }
              else if (packet == ENTER_PRESS){
                if(game_over_menu.continue_game.is_selected){
                  game_restart();
                  game_state = game_loop;
                }
                else if(game_over_menu.return_main_menu.is_selected) {
                  game_restart();
                  game_state = main_menu;
                }
              }
              break;
            }
            packet = 0;
          }
        }
        break;
      default:
        break;
      }
    }
  }

  return 0;
}

int game_end(){
  if (kbc_unsubscribe() != OK) return 1;
  if (vg_exit()) return 1;
  return 0;
}
