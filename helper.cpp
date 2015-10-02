/* KRk program for chess end-games with king & rook vs king
Author: Phillip Stewart

Helper functions for the chess game.
*/


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "helper.h"
using namespace std;


#define TRY_ADD_MOVE_Y s2=make_move(s,move,false);if(!y_in_check(s2)&&!kings_too_close(s2)){moves.push_back(move);}
#define TRY_ADD_KING if(K_can_move(s,move)){moves.push_back(move);}
#define INPUT_FILE "testCase.txt"


/* State constructor */
state::state() {
	K = 0;
	R = 0;
	k = 0;
}

/* State constuctor with values */
state::state(unsigned char a, unsigned char b, unsigned char c) {
	K = a;
	R = b;
	k = c;
}


/* State simple validation */
bool state::is_valid() {
	if (K == R || R == k || K == k ||
		K > 63 || R > 63 || k > 63) {
		return false;
	} else {
		return true;
	}
}


/* State comparators
Equality is actually used to compare states.
The less-than comparator is defined just so that a pair containing a state
	can be easily passed to the std::sort function.
*/
bool operator==(const state& a, const state& b) {
	return (a.k==b.k && a.K==b.K && a.R==b.R);
}
bool operator<(const state& a, const state& b) {
	return a.k<b.k;//return false...
}


/* Error message handling.
Used when the program must exit and display a message to the user.
*/
void err(string msg) {
	cerr << msg << endl << "Exiting...\n";
	exit(EXIT_FAILURE);
}


/* Lists all valid moves for player X
I used macros for these as there is a lot of repetition...
*/
vector<unsigned char> list_all_moves_x(state s) {
	vector<unsigned char> moves;
	unsigned char move;

	//In case something went wrong and R can capture k:
	if (y_in_check(s)) {
		move = s.k + 64;
		moves.push_back(move);
		return moves;
	}

	int rank, file;
	//King
	rank = s.K % 8;
	file = s.K / 8;
	if (rank > 0) {
		//add below
		move = s.K - 1;
		TRY_ADD_KING
		if (file > 0) {
			//add down-left
			move = s.K - 9;
			TRY_ADD_KING
		}
		if (file < 7) {
			//add down-right
			move = s.K + 7;
			TRY_ADD_KING
		}
	}
	if (rank < 7) {
		//add above
		move = s.K + 1;
		TRY_ADD_KING
		if (file > 0) {
			//add up-left
			move = s.K - 7;
			TRY_ADD_KING
		}
		if (file < 7) {
			//add up-right
			move = s.K + 9;
			TRY_ADD_KING
		}
	}
	if (file > 0) {
		//add left
		move = s.K - 8;
		TRY_ADD_KING
	}
	if (file < 7) {
		//add right
		move = s.K + 8;
		TRY_ADD_KING
	} 
	//Rook
	rank = s.R % 8;
	file = s.R / 8;

	//if rank clear:
	if (rank != s.K%8) {
		for (move=rank; move<64; move+=8) {
			if (move != s.R) {
				moves.push_back((unsigned char)(move + 64));
			}
		}
	} else { //K in rank...
		if (file > 0) {
			move = s.R - 8;
			while (move >= 0 && move != s.K) {
				if (move > 200) {//watch out for unsigned overflow...
					break;
				}
				moves.push_back((unsigned char)(move + 64));
				move -= 8;
			}
		}
		if (file < 7) {
			move = s.R + 8;
			while (move < 64 && move != s.K) {
				moves.push_back((unsigned char)(move + 64));
				move += 8;
			}
		}
	}
	//if file is clear
	if (file != s.K/8) {
		for (move=file*8; move<(file+1)*8; move++) {
			if (move != s.R) {
				moves.push_back((unsigned char)(move + 64));
			}
		}
	} else {//K in file...
		if (rank > 0) {
			move = s.R - 1;
			while (move%8 != 7 && move != s.K) {
				moves.push_back((unsigned char)(move + 64));
				move -= 1;
			}
		}
		if (rank < 7) {
			move = s.R + 1;
			while (move%8 != 0 && move != s.K) {
				moves.push_back((unsigned char)(move + 64));
				move += 1;
			}
		}
	}
	return moves;
}


/* Lists all valid moves for player Y */
vector<unsigned char> list_all_moves_y(state s) {
	vector<unsigned char> moves;
	unsigned char move;
	int rank, file;
	state s2(0,0,0);
	rank = s.k % 8;
	file = s.k / 8;
	if (rank > 0) {
		//add below
		move = s.k - 1;
		TRY_ADD_MOVE_Y
		if (file > 0) {
			//add down-left
			move = s.k - 9;
			TRY_ADD_MOVE_Y
		}
		if (file < 7) {
			//add down-right
			move = s.k + 7;
			TRY_ADD_MOVE_Y
		}
	}
	if (rank < 7) {
		//add above
		move = s.k + 1;
		TRY_ADD_MOVE_Y
		if (file > 0) {
			//add up-left
			move = s.k - 7;
			TRY_ADD_MOVE_Y
		}
		if (file < 7) {
			//add up-right
			move = s.k + 9;
			TRY_ADD_MOVE_Y
		}
	}
	if (file > 0) {
		//add left
		move = s.k - 8;
		TRY_ADD_MOVE_Y
	}
	if (file < 7) {
		//add right
		move = s.k + 8;
		TRY_ADD_MOVE_Y
	} 
	return moves;
}


/* Called to validate player input. */
bool is_valid_move(state s, unsigned char move, bool player_x) {
	vector<unsigned char> moves;
	if (player_x) {
		moves = list_all_moves_x(s);
		for (int i=0; i < (int)moves.size(); i++) {
			if (move == moves[i]) {
				return true;
			}
		}
		return false;
	} else {
		moves = list_all_moves_y(s);
		for (int i=0; i < (int)moves.size(); i++) {
			if (move == moves[i]) {
				return true;
			}
		}
		return false;
	}
}


/* Move's the piece and returns the new state.
Note: this is used to test moves, and as such should have no side-effects.
Input:	state s - current state of the board
		char move - the move to make:
			if player_x
				if char < 64, move K. else move R.
				char /8 = col, (a-h zero-based)
				char %8 = row, (1-8 zero-based)
			if player_y:
				char /8 = col, (a-h zero-based)
				char %8 = row, (1-8 zero-based)
		bool player_x - is it Xs move?
Output:	state - The updated state of the board.
*/
state make_move(state s, unsigned char move, bool player_x) {
	if (player_x) {
		if (move < 64) { // move K
			s.K = move;
		} else { // move R
			s.R = move - 64;
		}
	} else { // player y, move k
		s.k = move;
		if (s.k == s.R) {
			s.R = 255;
		}
	}
	return s;
}


/* Checks that the king can make the proposed move. */
bool K_can_move(state s, unsigned char move) {
	state s2 = make_move(s, move, true);
	if (kings_too_close(s2) || s2.K == s.R) {
		return false;
	} else {
		return true;
	}
}


/* Determine's if the Kings are within square move of eachother. */
bool kings_too_close(state s) {
	//check for wraparound first...
	if (((s.k%8 == 0) && (s.K%8 == 7)) ||
		((s.k%8 == 7) && (s.K%8 == 0)) ||
		((s.k/8 == 0) && (s.K/8 == 7)) ||
		((s.k/8 == 7) && (s.K/8 == 0))) {
		return false;
	} else if (s.k - 9 == s.K ||
		s.k - 8 == s.K ||
		s.k - 7 == s.K ||
		s.k - 1 == s.K ||
		s.k + 1 == s.K ||
		s.k + 7 == s.K ||
		s.k + 8 == s.K ||
		s.k + 9 == s.K) {
		return true;
	} else {
		return false;
	}
}


/* Determine's if player Ys king is in check
Note: player x cannot be in check.
	and y only chechable by rook.
*/
bool y_in_check(state s) {
	if (s.R == 255) {
		return false;
	}
	unsigned char Krank = s.K % 8;
	unsigned char Rrank = s.R % 8;
	unsigned char krank = s.k % 8;
	unsigned char Kfile = s.K / 8;
	unsigned char Rfile = s.R / 8;
	unsigned char kfile = s.k / 8;
	if (krank == Rrank) {
		//if blocked by K
		if (krank == Krank) {
			if (kfile < Kfile && Kfile < Rfile) {
				return false;
			} else if (Rfile < Kfile && Kfile < kfile) {
				return false;
			} else {
				return true;
			}
		} else {
			return true;
		}
	} else if (kfile == Rfile) {
		//if blocked by K
		if (kfile == Kfile) {
			if (krank < Krank && Krank < Rrank) {
				return false;
			} else if (Rrank < Krank && Krank < krank) {
				return false;
			} else {
				return true;
			}
		} else {
			return true;
		}
	} else {
		return false;
	}
}


/* Determine's if player Ys king is in checkmate
Note: player x cannot be in check or mate...
*/
bool in_checkmate(state s) {
	//TODO: smaller footprint than list_all_moves??
	if (y_in_check(s) && list_all_moves_y(s).size() == 0) {
		if (DEBUG_VERBOSE) {
			cout << "Checkmate found!\n";
		}
		return true;
	} else {
		return false;
	}
}


/* Prints an ascii version of the board
Player X has K,R. Player Y has k.
Input:	state s - current state of the board
Output:	No return value, prints to stdout.
*/
void print_board(state s) {
	string line = "  -- -- -- -- -- -- -- --\n";
	for (int row=8; row>0; row--) {
		cout << line << row;
		for (int col=0; col<8; col++) {
			if (s.K == col*8 + row-1) {
				cout << "| K";
			} else if (s.R == col*8 + row-1) {
				cout << "| R";
			} else if (s.k == col*8 + row-1) {
				cout << "| k";
			} else {
				cout << "|  ";
			}
		}
		cout << "|\n";
	}
	cout << line << "  a  b  c  d  e  f  g  h\n";
}


/* Determine whether we are running a test */
bool get_is_test() {
	string response;
	cout << "Is this a test (y/n): ";
	getline(cin, response);
	if (response.length() == 0 ||
		response == "\n") {
		err("Invalid input.");
	}
	char y_or_n = tolower(response[0]);
	if (y_or_n == 'y') {
		return true;
	} else if (y_or_n == 'n') {
		return false;
	} else {
		err("Invalid input.");
	}
	//suppress warning...
	return true;
}


/* Ask for the max moves (turns) */
unsigned int get_max_turns() {
	unsigned int num_turns;
	string response;
	cout << "Enter the maximum # moves (default: 35): ";
	getline(cin, response);
	try {
		if (response == "") {
			num_turns = 35;
		} else {
			stringstream(response) >> num_turns;
		}
	} catch (exception& e) {
		err("Invalid input.");
	}
	return num_turns;
}


/* Return initial board state. */
state get_initial_state() {
	string response;
	cout << "Read from file (y/n): ";
	getline(cin, response);
	if (response.length() == 0 ||
		response == "\n") {
		err("Invalid input.");
	}
	char y_or_n = tolower(response[0]);
	if (y_or_n == 'y') {
		return get_state_from_file();
	} else if (y_or_n == 'n') {
		return get_state_from_stdin();
	} else {
		err("Invalid input.");
		//suppress warning...
		return state(0,0,0);
	}
}


/* Read initial state from file. */
state get_state_from_file() {
	if (VERBOSE_RESULTS) {
		cout << "\n---------------------------------------\n";
		cout << "Loading initial game-state from file...\n";
	}
	ifstream infile;
	infile.open(INPUT_FILE);
	string line;
	getline(infile, line);

	int a,b,c,d,e,f;
	sscanf(line.c_str(),"x.K(%d,%d),x.R(%d,%d),y.K(%d,%d)",&a,&b,&c,&d,&e,&f);

	char K = (char)((a-1)*8 + (b-1));
	char R = (char)((c-1)*8 + (d-1));
	char k = (char)((e-1)*8 + (f-1));

	state s(K, R, k);
	if (!s.is_valid()) {
		err("Invalid board configuration.");
	} else if (VERBOSE_RESULTS) {
		cout << "Loaded game:\n" << line << endl;
		cout << "---------------------------------------\n";
	}
	infile.close();
	print_board(s);
	return s;
}


/* Read initial state from stdin. */
state get_state_from_stdin() {
	string response;
	unsigned char K, R, k;
	cout << "--------------------------------------\n";
	cout << "Player X King position (ex: a1 or e5): ";
	getline(cin, response);
	K = convert_PGN_to_char(response);
	cout << "Player X Rook position (ex: e2 or c6): ";
	getline(cin, response);
	R = convert_PGN_to_char(response);
	cout << "Player Y King position (ex: h8 or b3): ";
	getline(cin, response);
	k = convert_PGN_to_char(response);

	state s(K, R, k);
	if (!s.is_valid()) {
		err("Invalid board configuration.");
	} else {
		cout << "Loaded game.\n";
		cout << "--------------------------------------\n";
		if (VERBOSE_RESULTS) {
			print_board(s);
		}
	}
	return s;
}


/* Ask if they want to be player X */
bool ask_x() {
	string response;
	cout << "Are you player X (X goes first) (y/n)? ";
	getline(cin, response);
	if (response.length() == 0 ||
		response == "\n") {
		err("Invalid input.");
	}
	char y_or_n = tolower(response[0]);
	if (y_or_n == 'y') {
		return true;
	} else if (y_or_n == 'n') {
		return false;
	} else {
		err("Invalid input.");
		//suppress warning...
		return false;
	}
}


/* Converter for reading piece position from stdin
Input:	string square - Two char notation for rank & file
			ex: "a1", "e2", "h8" ...
Output:	unsigned char - Index of board square as defined in description.
*/
unsigned char convert_PGN_to_char(string square) {
	if (square.length() < 2) {
		err("Unable to convert coordinate.");
	}
	unsigned char c;
	char rank, file;
	file = square[0] - 'a';
	rank = square[1] - '1';
	if (file < 'a' || file > 'h' ||
		rank < '1' || rank > '8') {
		err("Invalid coordinate.");
	}
	c = (unsigned char)(file*8 + rank);
	return c;
}


/* Converter for reading player moves from stdin
Input:	string move_str - Three char notation for piece, rank, & file
			ex: "Ka1", "Re2", "kh8" ...
Output:	unsigned char - move.
*/
unsigned char convert_PGN_to_move(string move_str, bool player_x) {
	if (move_str.length() < 3) {
		cout << "Unable to parse move: " << move_str << endl;
		return 255;
	}
	unsigned char c;
	char piece, rank, file;
	piece = move_str[0];
	file = move_str[1] - 'a';
	rank = move_str[2] - '1';
	if (piece != 'K' && piece != 'R' && piece != 'k') {
		cout << "Invalid piece: " << piece << endl;
		return 255;
	} else if ((player_x && piece == 'k') || (!player_x && piece != 'k')) {
		cout << "Cannot move opposing player's piece.\n";
		return 255;
	}
	if (file < 0 || file > 7 ||
		rank < 0 || rank > 7) {
		cout << "Invalid coordinate.\n";
		return 255;
	}
	c = (unsigned char)(file*8 + rank);
	if (piece == 'R') {
		c += 64;
	}
	return c;
}


/* Converter for writing PGN from moves
Note: this is not a reverse of the previous function, piece is given too.
Input:	state s - current state of board (before move)
			Note: state is used to see if check, capture, mate...
		unsigned char - Index of board square as defined in description.
			Note: For rook, char value is +64
		bool player_x - is it player X's move?
Output:	string - Three or more char notation for rank & file
			ex: "Ka1", "Re2+", "kxh8" ...
Additional note: only R..+ check and kxR capture are considered.
	This function may be given invalid moves such as a king moving into
	the other king's move space. This will not show as a check.
*/
string convert_move_to_PGN(state s, unsigned char move, bool player_x) {
	string move_str;
	char rank, file;
	file = (char)(move / 8) + 'a';
	rank = (char)(move % 8) + '1';
	if (player_x) {
		if (move < 64) { //King
			move_str += "K";
			move_str += file;
			move_str += rank;
			if (in_checkmate(make_move(s, move, true))) {
				move_str += "#";
			}
		} else { //Rook
			file -= 8;
			move_str += "R";
			move_str += file;
			move_str += rank;
			if (y_in_check(make_move(s, move, true))) {
				if (in_checkmate(make_move(s, move, true))) {
					move_str += "#";
				} else {
					move_str += "+";
				}
			}
		}
	} else { //y king
		move_str += "k";
		if (s.R == move) {
			move_str += "x";
		}
		move_str += file;
		move_str += rank;
	}
	return move_str;
}


// end of helper.cpp
