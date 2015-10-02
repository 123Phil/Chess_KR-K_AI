#ifndef MOVE_H
#define MOVE_H

#include "helper.h"

unsigned char moveX(state s);
unsigned char moveY(state s);
unsigned char ex_minimax_moveX(state s, int depth);
unsigned char minimax_moveY(state s, int depth);
unsigned char additive_minimax_moveY(state s, int depth);
unsigned char maximax_moveX(state s, int depth);

#endif