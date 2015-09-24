#ifndef KRk_H
#define KRk_H

#include <string>
#include <vector>


enum DIR {NONE=0, UP, DOWN, LEFT, RIGHT, UL, UR, DL, DR};


/* State class
Defines variables to represent the state of the board.
Also includes a simple validation function.
*/
class state {
public:
	unsigned char K;
	unsigned char R;
	unsigned char k;
	state() {
		K = 0;
		R = 0;
		k = 0;
	}
	state(unsigned char a, unsigned char b, unsigned char c) {
		K = a;
		R = b;
		k = c;
	}
	bool is_valid() {
		if (K == R || R == k || K == k ||
			K > 63 || R > 63 || k > 63) {
			return false;
		} else {
			return true;
		}
	}
};



void err(std::string msg);
unsigned char moveX(state s);
unsigned char moveY(state s);
int get_push_dir(state s);
state orient(state s, int& dir);
int heuristicX(state s);
int heuristicY(state s);
bool is_valid_move(state s, unsigned char move, bool player_x);
state make_move(state s, unsigned char move, bool player_x);
std::vector<unsigned char> list_all_moves_x(state s);
std::vector<unsigned char> list_all_moves_y(state s);
bool K_can_move(state s, unsigned char move);
bool kings_too_close(state s);
bool y_in_check(state s);
bool in_checkmate(state s);
void play(state s, int max_turns, bool x_ai);
int test_play(state s, int max_turns);
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
void verify_lam(state s);
void test_heuristics();
void test_orient(state s);


#endif

