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
#include "KRk.h"
using namespace std;

#define VERBOSE_RESULTS true
#define INPUT_FILE "testCase.txt"


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


/* Checks that the suggested move is valid
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
bool is_valid_move(state s, unsigned char move, bool player_x) {
	if (move >= 128) {
		return false;
	} else if (!player_x && move >= 64) {
		return false;
	} else if (player_x) {
		if (move < 64) { // move K
			// see if move is adjecent to s.K
			// and not wrapped around board edge

			//TODO
			s.K = move;
		} else { // move R
			// see if move row or col == s.R
			move -= 64;

			//TODO
			s.R = move;
		}
	} else { // player y, move k
		// see if move is adjecent to s.K
		// and not wrapped around board edge

		//TODO
		s.k = move;
	}

	//See if pieces are overlapping or off the board:
	return s.is_valid();
}


/* Move's the piece and returns the new state.
Caution: Only call this function after calling is_valid_move()
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
	}
	return s;
}


/* Lists all valid moves for player

*/
vector<unsigned char> list_all_moves(state s, bool player_x) {
	vector<unsigned char> moves;
	if (player_x) {
		//TODO
	} else { // player y
		//TODO
	}
	return moves;
}


/* Determine's if player Ys king is in check
Note: player x cannot be in check.
*/
bool in_check(state s) {
	//TODO

	return false;
}


/* Determine's if player Ys king is in checkmate
Note: player x cannot be in check or mate...
*/
bool in_checkmate(state s) {
	//TODO

	return false;
}


/* Game controller for competition
Controls the play of the game for player vs computer
	-computer vs other program by inputting other program's output...
Input:	state s - initial state of the board
		int max_turns - maximum moves per player allowed.
Output:	int - outcome of the game
*/
int play(state s, int max_turns) {

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


/* String translator for moves
Converts move chars to a Portable-Game-Notation string
Input:	char x - player Xs move
		char y - player Ys move
Output:	returns a Portable-Game-Notation string for the turn
*/
string translate_moves_to_PGN(unsigned char x, unsigned char y) {

//TODO: if check add +, if mate add #
	// and add endgame stuff?? - 1/2-1/2, ...

	string s;
	char row, col;
	if (x < 64) {
		s += "K";
	} else {
		s += "R";
		x -= 64;
	}
	col = 'a' + (char)(x/8);
	row = '1' + (char)(x%8);
	s += col;
	s += row;
	s += " k";
	col = 'a' + (char)(y/8);
	row = '1' + (char)(y%8);
	s += col;
	s += row;
	return s;
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
	cout << "Length: " << response.length() << endl;
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
	cout << "Length: " << response.length() << endl;
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
		if (VERBOSE_RESULTS) {
			print_board(s);
		}
	}
	return s;
}


/* Converter for reading piece position from stdin

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


/* Main function

*/
int main(int argc, char** argv) {
	
	// command line args?

	bool is_test = get_is_test();
	unsigned int max_turns = get_max_turns();
	state s = get_initial_state();

	int outcome;
	if (is_test) {
		int outcome = test_play(s, max_turns);
	} else { //competition play
		int outcome = play(s, max_turns);
	}

	cout << "Number of moves made: " << outcome << endl;


	/*
//Test to see if printing and state work as expected...
	state s(7, 63, 56);
//	cout << "Valid? " << s.is_valid() << endl;
	print_board(s);
	*/

	return 0;
}

