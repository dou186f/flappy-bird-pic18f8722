#include "scoring.h"
#include <xc.h>
#include <stdint.h>
#include "game.h"

const uint8_t SEG_PATTERNS[10] = {
    0x3F,
    0x06,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x07,
    0x7F,
    0x6F,
};

uint16_t score              = 0;
uint8_t  current_digit      = 0;
uint32_t last_alive_ms      = 0;
uint8_t  alive_initialized  = 0;

void display_score(void) {
    uint8_t digits[3];
    uint16_t temp = score;

    digits[0] = temp % 10;
    digits[1] = (temp / 10) % 10;
    digits[2] = (temp / 100) % 10;

    LATH = 0x00;
    LATJ = SEG_PATTERNS[digits[current_digit]];

    if      (current_digit == 0) LATH = 0x08;
    else if (current_digit == 1) LATH = 0x04;
    else if (current_digit == 2) LATH = 0x02;

    current_digit++;
    if (current_digit >= 3) current_digit = 0;
}

static void add_score(uint16_t points) {
    score += points;
    if (score > 999) score = 999;
}

void reset_score(void) {
    score             = 0;
    current_digit     = 0;
    last_alive_ms     = 0;
    alive_initialized = 0;

    LATH = 0x00;
    LATJ = 0x00;
}

void update_score(void) {
    if (check_pipe_pass()) {
        add_score(2);
    }
}

void update_alive_score(uint32_t now) {
    if (!alive_initialized) {
        last_alive_ms    = now;
        alive_initialized = 1;
        return;
    }
    if (now - last_alive_ms >= 2000) {
        last_alive_ms += 2000;
        add_score(3);
    }
}
