#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define GAME_INIT       0
#define GAME_RUNNING    1
#define GAME_OVER       2

extern volatile uint16_t scroll_step_time;

void game_init(void);

uint8_t game_is_running(void);

void bird_flap(void);
void bird_gravity(void);
void bird_toggle(void);

void scene_scroll(void);

uint8_t check_pipe_pass(void);
uint8_t check_collision(void);
uint8_t handle_collision(void);

void tick_ms(uint32_t ms);

void render(void);

#endif
