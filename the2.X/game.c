#include <xc.h>
#include <stdint.h>
#include "game.h"
#include "map.h"
#include "scoring.h"

static uint8_t scene_buffer[4];
static uint8_t bird_row;
static uint8_t bird_visible;
static uint8_t scene_index;
static uint8_t game_state = GAME_INIT;
static uint16_t freeze_remaining_ms = 0;

extern volatile uint16_t scroll_step_time;

void game_init(void) {
    bird_row = 1;
    bird_visible = 1;
    scene_index = 0;

    scene_buffer[0] = SCENE_ARRAY[0][0] << 7;
    scene_buffer[1] = SCENE_ARRAY[1][0] << 7;
    scene_buffer[2] = SCENE_ARRAY[2][0] << 7;
    scene_buffer[3] = SCENE_ARRAY[3][0] << 7;

    scene_index = 1;
    scroll_step_time = 1000;
    game_state = GAME_RUNNING;
    freeze_remaining_ms = 0;

    render();
}

uint8_t game_is_running(void) {
    return (game_state == GAME_RUNNING);
}

void bird_flap(void) {
    if (bird_row < 3) bird_row++;
}

void bird_gravity(void) {
    if (bird_row > 0) bird_row--;
}

void bird_toggle(void) {
    bird_visible ^= 1;
}

void scene_scroll(void) {
    scene_buffer[0] = (scene_buffer[0] >> 1) | (SCENE_ARRAY[0][scene_index] << 7);
    scene_buffer[1] = (scene_buffer[1] >> 1) | (SCENE_ARRAY[1][scene_index] << 7);
    scene_buffer[2] = (scene_buffer[2] >> 1) | (SCENE_ARRAY[2][scene_index] << 7);
    scene_buffer[3] = (scene_buffer[3] >> 1) | (SCENE_ARRAY[3][scene_index] << 7);

    scene_index++;
    if (scene_index >= MAP_SIZE) scene_index = 0;
}

uint8_t check_collision(void) {
    uint8_t buf_idx = 3 - bird_row;
    return (scene_buffer[buf_idx] & 0x01) ? 1 : 0;
}

uint8_t check_pipe_pass(void) {
    uint8_t f = scene_buffer[0] & 0x01;
    uint8_t e = scene_buffer[1] & 0x01;
    uint8_t d = scene_buffer[2] & 0x01;
    uint8_t c = scene_buffer[3] & 0x01;

    if (f && e && !d && !c) return 1;
    if (!f && !e && d && c) return 1;
    return 0;
}

uint8_t handle_collision(void) {
    if (check_collision()) {
        game_state = GAME_OVER;
        freeze_remaining_ms = 4000;
        return 1;
    }
    return 0;
}

void tick_ms(uint32_t ms) {
    if (game_state == GAME_RUNNING) return;

    if (ms < freeze_remaining_ms) {
        freeze_remaining_ms -= (uint16_t)ms;
        return;
    }
    ms -= freeze_remaining_ms;

    if (game_state == GAME_OVER) {
        game_state = GAME_INIT;
        freeze_remaining_ms = 1000;
        LATC = 0;
        LATD = 0;
        LATE = 0;
        LATF = 0;
        reset_score();
    } else {
        game_init();
        return;
    }

    if (ms >= freeze_remaining_ms) {
        game_init();
    } else {
        freeze_remaining_ms -= (uint16_t)ms;
    }
}

void render(void) {
    uint8_t buf_idx = 3 - bird_row;

    uint8_t f_val = scene_buffer[0];
    uint8_t e_val = scene_buffer[1];
    uint8_t d_val = scene_buffer[2];
    uint8_t c_val = scene_buffer[3];

    switch (buf_idx) {
        case 0: f_val = (f_val & 0xFE) | bird_visible; break;
        case 1: e_val = (e_val & 0xFE) | bird_visible; break;
        case 2: d_val = (d_val & 0xFE) | bird_visible; break;
        case 3: c_val = (c_val & 0xFE) | bird_visible; break;
    }

    LATF = f_val;
    LATE = e_val;
    LATD = d_val;
    LATC = c_val;
}
