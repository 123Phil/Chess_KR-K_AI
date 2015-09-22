/* KRk program for chess end-games with king & rook vs king
Author: Phillip Stewart

The board state is represented by 3 chars, for K, R, and k
	the x.K, x.R and y.K positions.
	These chars hold a value from 0-63, pertaining to the square as shown
	below:

 -- -- -- -- -- -- -- --
| 7|15|23|31|39|47|55|63|
 -- -- -- -- -- -- -- --
| 6|14|22|30|38|46|54|62|
 -- -- -- -- -- -- -- --
| 5|13|21|29|37|45|53|61|
 -- -- -- -- -- -- -- --
| 4|12|20|28|36|44|52|60|
 -- -- -- -- -- -- -- --
| 3|11|19|27|35|43|51|59|
 -- -- -- -- -- -- -- --
| 2|10|18|26|34|42|50|58|
 -- -- -- -- -- -- -- --
| 1| 9|17|25|33|41|49|57|
 -- -- -- -- -- -- -- --
| 0| 8|16|24|32|40|48|56|
 -- -- -- -- -- -- -- --

The player peices are printed as upper-case K and R for player X,
	and a lower-case k for player Y.

It is assumed that player Y chooses the board setup, and player X goes first.
	That means X is white (really only useful for game notation).

The real brains of the program are in the heuristic functions,
	which are customized for KR-k play. The strategy for KR is to trap the
	opposing king in a corner, the rook pushes row by row, and the king
	ensures a rook-check forces the same direction... Eventually checkmate
	is achieved by pinning the king in the last row and blocking its escape
	with our king.
	The strategy for the sole king is to vie for position in the middle.
	We try to play cat and mouse and take advantage of any mistakes.
	The real trick is that it might be advantageous to get checked if it 
	means escaping a trap against a wall.

Moves are entered with Portable Game Notation.
	examples: Ke5, Ra1, kh2 ...
	You shouldn't enter additional information, such as + or # for check/mate
	Just <peice><col><row>

The game summary is saved to file in Portable Game Notation.

*/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <exception>
using namespace std;

#define VERBOSE_RESULTS true
#define INPUT_FILE "testCase.txt"
#define TRY_ADD_MOVE_Y s2=make_move(s,move,false);if(!y_in_check(s2)&&!kings_too_close(s2)){moves.push_back(move);}
#define TRY_ADD_KING if(K_can_move(s,move)){moves.push_back(move);}


bool MATE = false;


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
std::vector<unsigned char> list_all_moves_x(state s);
std::vector<unsigned char> list_all_moves_y(state s);
bool K_can_move(state s, unsigned char move);
bool kings_too_close(state s);
bool y_in_check(state s);
bool in_checkmate(state s);
int play(state s, int max_turns, bool x_ai);
int test_play(state s, int max_turns);
void print_board(state s);
bool get_is_test();
unsigned int get_max_turns();
state get_initial_state();
state get_state_from_file();
state get_state_from_stdin();
unsigned char convert_PGN_to_char(std::string square);
void verify_lam(state s);



/* Move function for player X (KR)
Input:	state s - current state of the board
Output:	returns a char, indicating the best move
			if char < 64, move K. else move R.
			char /8 = col, (a-h zero-based)
			char %8 = row, (1-8 zero-based)
*/
unsigned char moveX(state s) {
	// list valid moves
	// get heuristic of each move
	// rank moves
	// recurse??
	// return move

	//TODO
	return 7;
}


/* Move function for player Y (k)
Input:	state s - current state of the board
Output:	returns a char, indicating the best move
			char /8 = col, (a-h zero-based)
			char %8 = row, (1-8 zero-based)
*/
unsigned char moveY(state s) {
	// list valid moves
	// get heuristic of each move
	// rank moves
	// recurse??
	// return move

	//TODO
	return 7;
}


/* Heuristic for player X
Input:	state s - current state of the board
Output:	int - the value of the board for player Y
			A higher value means the board state is more desireable.
			Bad positions should return a relatively low number
			and good positions should return a high number.
*/
int heuristicX(state s) {
	int h = 0;

	//TODO

	return h;
}


/* Heuristic for player Y
Input:	state s - current state of the board
Output:	int - the value of the board for player Y
			A higher value means the board state is more desireable.
			Bad positions should return a relatively low number
			and good positions should return a high number.
*/
int heuristicY(state s) {
	int h = 0;

	//TODO

	return h;
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
		//TODO: check outside if s.k == s.R (capture)
	}
	return s;
}


/* Lists all valid moves for player X
I used macros for these as there is a lot of repetition...
*/
vector<unsigned char> list_all_moves_x(state s) {
	vector<unsigned char> moves;
	unsigned char move;
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

//TODO: get this right...
//We can just assume for now, R will not move to k...
	// move should never == s.k (unless game over.)
	// if (...) {
	// 	cout << "X-Rook in Y-king's row... game over??\n";
	// 	exit(EXIT_FAILURE);
	// }

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
				moves.push_back((unsigned char)(move + 64));
				move -= 8;
			}
		}
		if (file < 8) {
			move = s.R + 8;
			while (move < 64 && move != s.K) {
				moves.push_back((unsigned char)(move + 64));
				move += 8;
			}
		}
	}
	//if file is clear
	if (file != s.k/8 && file != s.K/8) {
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
		if (rank < 8) {
			move = s.R + 1;
			while (move%8 != 0 && move != s.K) {
				moves.push_back((unsigned char)(move + 64));
				move += 1;
			}
		}
	}
	return moves;
}


/* Lists all valid moves for player Y

*/
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


bool K_can_move(state s, unsigned char move) {
	state s2 = make_move(s, move, true);
	if (kings_too_close(s2) || s2.K == s.R) {
		return false;
	} else {
		return true;
	}
}


/* Determine's if the Kings are within one move of eachother.

*/
bool kings_too_close(state s) { //TODO: verify this function...
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
	if (list_all_moves_y(s).size() == 0) {
		cout << "Checkmate found\n"; //TODO: no side-effects. (omit this line)
		return true;
	} else {
		return false;
	}
}


/* Game controller for competition
Controls the play of the game for player vs computer
	-computer vs other program by inputting other program's output...
Input:	state s - initial state of the board
		int max_turns - maximum moves per player allowed.
		bool x_ai - is AI player x?
Output:	int - outcome of the game
*/
int play(state s, int max_turns, bool x_ai) {

//TODO...

	return 0;
}


/* Game controller for tests
Controls the play of the game. Runs automatically choosing best plays.
Input:	state s - initial state of the board
		int max_turns - maximum moves per player allowed.
Output:	int - outcome of the game
*/
int test_play(state s, int max_turns) {

//TODO...

	return 0;	
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


/* Determine whether we are running a test

*/
bool get_is_test() {
	string response;
	cout << "Is this a test (y/n): ";
	getline(cin, response);
	if (response.length() == 0 ||
		response == "\n") {
		cout << "Invalid input. exiting...\n";
		exit(EXIT_FAILURE);
	}
	char y_or_n = tolower(response[0]);
	if (y_or_n == 'y') {
		return true;
	} else if (y_or_n == 'n') {
		return false;
	} else {
		cout << "Invalid input. exiting...\n";
		exit(EXIT_FAILURE);
	}
}


/* Ask for the max moves (turns)

*/
unsigned int get_max_turns() {
	unsigned int response;
	cout << "Enter the maximum # moves (default = 35): ";
	try {
		cin >> response;
		cin.ignore();
	} catch (exception& e) {
		cout << "Invalid input. exiting...\n";
		exit(EXIT_FAILURE);
	}
	return response;
}


/* Return initial board state.

*/
state get_initial_state() {
	string response;
	cout << "Read from file (y/n): ";
	getline(cin, response);
	if (response.length() == 0 ||
		response == "\n") {
		cout << "Invalid input. exiting...\n";
		exit(EXIT_FAILURE);
	}
	char y_or_n = tolower(response[0]);
	if (y_or_n == 'y') {
		return get_state_from_file();//TODO refactor function...
	} else if (y_or_n == 'n') {
		return get_state_from_stdin();
	} else {
		cout << "Invalid input. exiting...\n";
		exit(EXIT_FAILURE);
	}
}


/* Read initial state from file.

*/
state get_state_from_file() {
	cout << "---------------------------------------\n";
	cout << "Loading initial game-state from file...\n";
	ifstream infile;
	//TODO: allow user defined filename??
	infile.open(INPUT_FILE);
	string line;
	getline(infile, line);

	int a,b,c,d,e,f;
	sscanf(line.c_str(),"x.K(%d,%d),x.R(%d,%d),y.K(%d,%d)",&a,&b,&c,&d,&e,&f);

	//TODO: ask prof about coordinates...
	char K = (char)((a-1)*8 + (b-1));
	char R = (char)((c-1)*8 + (d-1));
	char k = (char)((e-1)*8 + (f-1));

	state s(K, R, k);
	if (!s.is_valid()) {
		cout << "Invalid coordinates. exiting...\n";
		exit(EXIT_FAILURE);
	} else {
		cout << "Loaded game:\n" << line << endl;
		cout << "---------------------------------------\n";
		if (VERBOSE_RESULTS) {
			print_board(s);
		}
	}
	return s;
}


/* Read initial state from stdin.

*/
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
		cout << "Invalid coordinates. exiting...\n";
		exit(EXIT_FAILURE);
	} else {
		cout << "Loaded game:\n";
		//TODO: print initial coordinates...
		cout << "--------------------------------------\n";
		if (VERBOSE_RESULTS) {
			print_board(s);
		}
	}
	return s;
}


/* Converter for reading piece position from stdin
Input:	string square - Two char notation for rank & file
			ex: "a1", "e2", "h8" ...
Output:	unsigned char - Index of board square as defined in description.
*/
unsigned char convert_PGN_to_char(string square) {
	if (square.length() < 2) {
		cout << "Unable to convert coordinate. exiting...\n";
		exit(EXIT_FAILURE);
	}
	unsigned char c;
	char rank, file;
	file = square[0] - 'a';
	rank = square[1] - '1';
	c = (unsigned char)(file*8 + rank);
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
	char piece, rank, file;
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


/* Testing function to verify that list_all_moves() works... */
void verify_lam(state s) {
	vector<unsigned char> moves;
	int l;
	cout << "\nPlayer X's moves:\n";
	moves = list_all_moves_x(s);
	l = moves.size();
	if (l == 0) {
		cout << "No moves found...\n";
	} else {
		for (int i=0; i<l; i++) {
			cout << convert_move_to_PGN(s, moves[i], true) << endl;
		}
	}
	cout << "\nPlayer Y's moves:\n";
	moves = list_all_moves_y(s);
	l = moves.size();
	if (l == 0) {
		cout << "No moves found...\n";
	} else {
		for (int i=0; i<l; i++) {
			cout << convert_move_to_PGN(s, moves[i], false) << endl;
		}
	}
}

/* Main function

*/
int main(int argc, char** argv) {
	
	// command line args?

	//bool is_test = get_is_test();
	//unsigned int max_turns = get_max_turns();
	state s = get_initial_state();

	// int outcome;
	// if (is_test) {
	// 	int outcome = test_play(s, max_turns);
	// } else { //competition play
	// 	int outcome = play(s, max_turns);
	// }
	// cout << "Number of moves made: " << outcome << endl;

	//print_board(s);
	verify_lam(s);

	return 0;
}

