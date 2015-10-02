/* KR-k program for chess end-games with king & rook vs king
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

state is a class defined in "helper.h"

The player peices are defined as upper-case K and R for player X,
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

The game summary is printed or saved to file in Portable Game Notation.

To compile and run:
$ make main
$ ./main
-You will then be prompted with a few questions, and the program will run.
To simply test the case defined in testCase.txt, run:
$ ./main <in

*/


#include <iostream>
#include "helper.h"
#include "play.h"


int main() {
	//Is this a test?
	bool is_test = get_is_test();
	bool x;
	if (!is_test) {
		//If it's not a test, what side are you playing?
		x = ask_x();
	}
	//Maximum number of turns?
	unsigned int max_turns = get_max_turns();
	//Gets either from stdin or file...
	state s = get_initial_state();
	if (kings_too_close(s)) {
		std::cout << "Invalid initial board...\n";
	}

	if (is_test) {
		//Computer runs both sides
		test_play(s, max_turns);
	} else {
		//Competition play
		play(s, max_turns, !x);
	}
	return 0;
}


