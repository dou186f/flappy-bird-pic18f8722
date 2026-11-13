#ifndef SCORING_H
#define SCORING_H

#include <stdint.h>

void reset_score(void);
void update_score(void);
void update_alive_score(uint32_t now);
void display_score(void);

#endif
