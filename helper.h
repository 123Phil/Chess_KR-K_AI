#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <vector>

#define VERBOSE_RESULTS true
#define DEBUG_VERBOSE false
#define DEPTH 2

enum DIR {NONE=0, UP, DOWN, LEFT, RIGHT, UL, UR, DL, DR};

class state {
public:
	unsigned char K;
	unsigned char R;
	unsigned char k;
	state();
	state(unsigned char a, unsigned char b, unsigned char c);
	bool is_valid();
};

bool operator==(const state& a, const state& b);
bool operator<(const state& a, const state& b);

void err(std::string msg);
std::vector<unsigned char> list_all_moves_x(state s);
std::vector<unsigned char> list_all_moves_y(state s);
bool is_valid_move(state s, unsigned char move, bool player_x);
state make_move(state s, unsigned char move, bool player_x);
bool K_can_move(state s, unsigned char move);
bool kings_too_close(state s);
bool y_in_check(state s);
bool in_checkmate(state s);
void print_board(state s);
bool get_is_test();
unsigned int get_max_turns();
state get_initial_state();
state get_state_from_file();
state get_state_from_stdin();
bool ask_x();
unsigned char convert_PGN_to_char(std::string square);
unsigned char convert_PGN_to_move(std::string move_str, bool player_x);
std::string convert_move_to_PGN(state s, unsigned char move, bool player_x);

#endif

