/* Header for KRk.cpp
Author: Phillip Stewart

*/


/* State class
Defines variables to represent the state of the board.
Also includes a validation function.
*/
class state {
public:
	unsigned char K;
	unsigned char R;
	unsigned char k;
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

unsigned char moveX(state s);
unsigned char moveY(state s);
int heuristicX(state s);
int heuristicY(state s);
bool is_valid_move(state s, unsigned char move, bool player_x);
state make_move(state s, unsigned char move, bool player_x);
std::vector<unsigned char> list_all_moves(state s, bool player_x);
bool in_check(state s);
bool in_checkmate(state s);
int play(state s, int max_turns);
int test_play(state s, int max_turns);
std::string translate_moves_to_PGN(unsigned char x, unsigned char y);
void print_board(state s);
bool get_is_test();
unsigned int get_max_turns();
state get_initial_state();
state get_state_from_file();
state get_state_from_stdin();
unsigned char convert_PGN_to_char(std::string square);


