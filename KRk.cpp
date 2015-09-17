/*


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
using namespace std;


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
		if (K == R ||
			R == k ||
			K == k ||
			K > 63 ||
			R > 63 ||
			k > 63) {
			return false;
		} else {
			return true;
		}
	}
};


/* Move function for player X (KR)
Input:	state s - current state of the board
Output:	returns a char, indicating the best move
			if char < 64, move K. else move R.
			char /8 = col, (a-h zero-based)
			char %8 = row, (1-8 zero-based)
*/
char moveX(state s) {
	return 7;
}


/* Move function for player Y (k)
Input:	state s - current state of the board
Output:	returns a char, indicating the best move
			char /8 = col, (a-h zero-based)
			char %8 = row, (1-8 zero-based)
*/
char moveY(state s) {
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


	return h;
}


/* Game controller
Controls the play of the game. Allows player vs heuristic or auto-play.
Input:	state s - initial state of the board
Output:	int - outcome of the game
*/
int play(state s) {
	return 0;
}

/* String translator for moves
Converts move chars to a Portable-Game-Notation string
Input:	char x - player Xs move
		char y - player Ys move
Output:	returns a Portable-Game-Notation string for the turn
*/
string translate_move(unsigned char x, unsigned char y) {
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


/* Main function

*/
int main(int argc, char** argv) {
	// prompt for test
	// max moves

	//play

	state s(7, 63, 56);
	print_board(s);

	return 0;
}

