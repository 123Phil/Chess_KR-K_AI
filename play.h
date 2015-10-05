#ifndef PLAY_H
#define PLAY_H

#include <vector>
#include <string>
#include "helper.h"

void play(state s, int max_turns, bool x_ai);
void test_play(state s, int max_turns);
void save_results(std::string initial_board, std::vector<std::string> summary);

#endif