/* KRk program for chess end-games with king & rook vs king
Author: Phillip Stewart

TODO:
test play function
improve heuristics
recurse, minimax (using top few and h(n) for probability!)
const params that shouldn't change?


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
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <exception>
#include <utility>
using namespace std;

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

bool operator<(const state& a, const state& b) {
	return false;
}

unsigned char moveX(state s);
unsigned char moveY(state s);
int heuristicX(state s);
int heuristicY(state s);
bool is_valid_move(state s, unsigned char move, bool player_x);
state make_move(state s, unsigned char move, bool player_x);
vector<unsigned char> list_all_moves_x(state s);
vector<unsigned char> list_all_moves_y(state s);
bool K_can_move(state s, unsigned char move);
bool kings_too_close(state s);
bool y_in_check(state s);
bool in_checkmate(state s);
int test_play(state s, int max_turns);
void print_states(vector< pair<int, state> > ranked_boards);
void save_states_to_file(vector< pair<int, state> > ranked_boards);
void run_finder();


/* Move function for player X (KR)
Input:	state s - current state of the board
Output:	returns a char, indicating the best move
			if char < 64, move K. else move R.
			char /8 = col, (a-h zero-based)
			char %8 = row, (1-8 zero-based)
*/
unsigned char moveX(state s) {
	unsigned char move = 0;
	vector<unsigned char> moves = list_all_moves_x(s);

	vector< pair<int, unsigned char> > ranked_moves;
	int rank;
	for (int i=0; i < (int)moves.size(); i++) {
		rank = heuristicX(make_move(s, moves[i], true));
		ranked_moves.push_back(make_pair(rank, moves[i]));
	}
	
	sort(ranked_moves.begin(), ranked_moves.end());
	reverse(ranked_moves.begin(), ranked_moves.end());

	move = ranked_moves[0].second;
	return move;
}


/* Move function for player Y (k)
Input:	state s - current state of the board
Output:	returns a char, indicating the best move
			char /8 = col, (a-h zero-based)
			char %8 = row, (1-8 zero-based)
*/
unsigned char moveY(state s) {
	unsigned char move = 0;
	vector<unsigned char> moves = list_all_moves_y(s);
	if (moves.size() == 0) {
		// if (VERBOSE_RESULTS) {
		// 	cout << "No moves found for Y...\n";
		// }
		//TODO: where return to - check for ==255 (mate).
		return 255;
	}
	vector< pair<int, unsigned char> > ranked_moves;
	int rank;
	for (int i=0; i < (int)moves.size(); i++) {
		rank = heuristicY(make_move(s, moves[i], false));
		ranked_moves.push_back(make_pair(rank, moves[i]));
	}
	
	sort(ranked_moves.begin(), ranked_moves.end());
	reverse(ranked_moves.begin(), ranked_moves.end());

	move = ranked_moves[0].second;
	return move;
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
	unsigned char Krank = s.K % 8;
	unsigned char Rrank = s.R % 8;
	unsigned char krank = s.k % 8;
	unsigned char Kfile = s.K / 8;
	unsigned char Rfile = s.R / 8;
	unsigned char kfile = s.k / 8;

	//R should be 1 rank or file from k, and far from k on that row
	//and the optimal row forces k outside most...
	int R_factor = 0;
	//K should be close to k, (K-space-k over rook line is best)
	int K_factor = 0;
	int pin_factor = 0;
	int rd = krank - Rrank;
	int fd = kfile - Rfile;
	int rdk = krank - Krank;
	int fdk = kfile - Kfile;

	//If k can capture R, return 0. If checkmate, return 2^16
	if ((rd == -1 || rd == 0 || rd == 1) &&
		(fd == -1 || fd == 0 || fd == 1)) {
		//TODO: unless K adj to R... (protected check)
		return 0;
	} else if ((Rrank == krank && Rrank == 0 && Krank == 2 && Kfile == kfile) ||
		(Rrank == krank && Rrank == 7 && Krank == 5 && Kfile == kfile) ||
		(Rfile == kfile && Rfile == 0 && Kfile == 2 && Krank == krank) ||
		(Rfile == kfile && Rfile == 7 && Kfile == 5 && Krank == krank)) {
		//Checkmate!
		return 65536;
	} else if ((Rrank == krank && Krank == krank - 2 && Kfile == kfile) ||
		(Rrank == krank && Krank == krank + 2 && Kfile == kfile) ||
		(Rfile == kfile && Kfile == kfile - 2 && Krank == krank) ||
		(Rfile == kfile && Kfile == kfile + 2 && Krank == krank)) {
		//Force k to edge.
		return 32768;
	}

	//All this below assumes R is one row from k.
	//if K on wrong side, add 0
	//if K on line - add 100 if between R&k, else add 500
	//if K on correct side - 3k + rdk stuff
	int R_on_edge_factor = 0;
	if (krank >= 4) {//Push to top...
		if (Rrank == krank-1) {
			if (Rfile == 0) {
				R_on_edge_factor = 50;
				fdk--;
			} else if (Rfile == 7) {
				R_on_edge_factor = 50;
				fdk++;
			}
			rdk = (krank-2) - Krank;
			if (Rrank == Krank) {
				//If the king is in the way...
				if ((kfile < Kfile && Kfile < Rfile) ||
					(kfile > Kfile && Kfile > Rfile)) {
					K_factor = 100;
				} else {
					R_factor = 1000 * Rrank;//1k more for each row up.
					R_factor += fd*fd;
					K_factor = 500;
				}
			} else {
				R_factor = 1000 * Rrank;//1k more for each row up.
				R_factor += fd*fd;
				// K on the right side?
				if (Krank > Rrank) {
					K_factor = 0;
				} else {
					K_factor = 1000;
				}
			}
			if (Krank == krank-2 && Kfile == kfile) {
				pin_factor = 6;
			}
		}
	} else {//Push to bottom...
		if (Rrank == krank+1) {
			if (Rfile == 0) {
				R_on_edge_factor = 50;
				fdk--;
			} else if (Rfile == 7) {
				R_on_edge_factor = 50;
				fdk++;
			}
			rdk = (krank+2) - Krank;
			if (Rrank == Krank) {
				if ((kfile < Kfile && Kfile < Rfile) ||
					(kfile > Kfile && Kfile > Rfile)) {
					K_factor = 100;
				} else {
					R_factor = 1000 * (7-Rrank);//1k more for each row down.
					R_factor += fd*fd;
					K_factor = 500;
				}
			} else {
				R_factor = 1000 * (7-Rrank);//1k more for each row down.
				R_factor += fd*fd;
				// K on the right side?
				if (Krank < Rrank) {
					K_factor = 0;
				} else {
					K_factor = 1000;
				}
			}
			if (Krank == krank+2 && Kfile == kfile) {
				pin_factor = 6;
			}
		}
	}
	if (kfile < 4) {//Push to left...
		if (Rfile == kfile+1) {
			if (Rrank == 0) {
				R_on_edge_factor = 50;
				rdk--;
			} else if (Rrank == 7) {
				R_on_edge_factor = 50;
				rdk++;
			}
			fdk = (kfile+2) - Kfile;
			if (Rfile == Kfile) {
				if ((krank < Krank && Krank < Rrank) ||
					(krank > Krank && Krank > Rrank)) {
					K_factor = 100;
				} else {
					R_factor = 1000 * (7-Rfile);//1k more for each row left.
					R_factor += rd*rd;
					K_factor = 500;
				}
			} else {
				R_factor = 1000 * (7-Rfile);//1k more for each row left.
				R_factor += rd*rd;
				// K on the right side?
				if (Kfile < Rfile) {
					K_factor = 0;
				} else {
					K_factor = 1000;
				}
			}
			if (Kfile == kfile+2 && Krank == krank) {
				pin_factor = 6;
			}
		}
	} else {//Push to right...
		if (Rfile == kfile-1) {
			if (Rrank == 0) {
				R_on_edge_factor = 50;
				rdk--;
			} else if (Rrank == 7) {
				R_on_edge_factor = 50;
				rdk++;
			}
			fdk = (kfile-2) - Kfile;
			if (Rfile == Kfile) {
				if ((krank < Krank && Krank < Rrank) ||
					(krank > Krank && Krank > Rrank)) {
					K_factor = 100;
				} else {
					R_factor = 1000 * Rfile;//1k more for each row right.
					R_factor += rd*rd;
					K_factor = 500;
				}
			} else {
				R_factor = 1000 * Rfile;//1k more for each row right.
				R_factor += rd*rd;
				// K on the right side?
				if (Kfile > Rfile) {
					K_factor = 0;
				} else {
					K_factor = 1000;
				}
			}
			if (Kfile == kfile-2 && Krank == krank) {
				pin_factor = 6;
			}
		}
	}

	if (rdk > 0) {
		K_factor += (10-rdk)*(10-rdk);
	} else {
		K_factor += (10+rdk)*(10+rdk);
	} if (fdk > 0) {
		K_factor += (10-fdk)*(10-fdk);
	} else {
		K_factor += (10+fdk)*(10+fdk);
	}
	
	h = R_factor + K_factor + R_on_edge_factor + pin_factor;
	return h;
}


/* Another attempt at Heuristic for player Y
Input:	state s - current state of the board
Output:	int - the value of the board for player Y
			A higher value means the board state is more desireable.
			Bad positions should return a relatively low number
			and good positions should return a high number.
Capture Rook: 65536
Rook block: 1000 + (30 to 200, more when closer to rook)
King block && one away from rook: 1200
King block && safe rook: 10
Rook block and king close to edge: 50 to 250, more close to center
No rook block: 3000 + (50 to 250, more close to center)
In rook row && check??: 50
In rook row && check && pin: 0 //Push & mate
In rook row, no check: 1500
*/
int heuristicY(state s) {
	//Always capture rook if possible:
	if (s.R == 255) {
		return 65536;
	}
	unsigned char Krank = s.K % 8;
	unsigned char Rrank = s.R % 8;
	unsigned char krank = s.k % 8;
	unsigned char Kfile = s.K / 8;
	unsigned char Rfile = s.R / 8;
	unsigned char kfile = s.k / 8;
	unsigned char rank2 = krank * 2;
	unsigned char file2 = kfile * 2;

	// If blocked by rook move toward rook (unless king trap)
	if (krank < 3 && Rrank == krank + 1) {//Rook above
		if (Krank == krank + 2 && kfile == Kfile) {//trap
			if (Rfile == kfile + 1 || Rfile == kfile - 1) {
				return 1200;
			} else {
				return 10;
			}
		} else {//Move toward rook
			if (Rfile < kfile) {
				return 1000 + (10 - (kfile - Rfile)) * 20;
			} else {
				return 1000 + (10 - (Rfile - kfile)) * 20;
			}
		}
	} else if (krank > 4 && Rrank == krank - 1) {//Rook below
		if (Krank == krank - 2 && kfile == Kfile) {//trap
			if (Rfile == kfile + 1 || Rfile == kfile - 1) {
				return 1200;
			} else {
				return 10;
			}
		} else {//Move toward rook
			if (Rfile < kfile) {
				return 1000 + (10 - (kfile - Rfile)) * 20;
			} else {
				return 1000 + (10 - (Rfile - kfile)) * 20;
			}
		}
	} else if (kfile < 3 && Rfile == kfile + 1) {//Rook on right
		if (Kfile == kfile + 2 && krank == Krank) {//trap
			if (Rrank == krank + 1 || Rrank == krank - 1) {
				return 1200;
			} else {
				return 10;
			}
		} else {//Move toward rook
			if (Rrank < krank) {
				return 1000 + (10 - (krank - Rrank)) * 20;
			} else {
				return 1000 + (10 - (Rrank - krank)) * 20;
			}
		}
	} else if (kfile > 4 && Rfile == kfile - 1) {//Rook on left
		if (Kfile == kfile - 2 && krank == Krank) {//trap
			if (Rrank == krank + 1 || Rrank == krank - 1) {
				return 1200;
			} else {
				return 10;
			}
		} else {//Move toward rook
			if (Rrank < krank) {
				return 1000 + (10 - (krank - Rrank)) * 20;
			} else {
				return 1000 + (10 - (Rrank - krank)) * 20;
			}
		}
	} else if (Rrank == krank) {//same rank as rook:
		if (y_in_check(s)) {
			//essentially dead code?
			// if trap:
			if (krank > 3) {
				if (kfile == Kfile && Krank == krank-2) {
					return 0;
				} else {
					return 50;
				}
			} else {
				if (kfile == Kfile && Krank == krank+2) {
					return 0;
				} else {
					return 50;
				}
			}
		} else {
			return 1500;
		}
	} else if (Rfile == kfile) {//same file as rook:
		if (y_in_check(s)) {
			//essentially dead code?
			// if trap:
			if (kfile > 3) {
				if (krank == Krank && Kfile == kfile-2) {
					return 0;
				} else {
					return 50;
				}
			} else {
				if (krank == Krank && Kfile == kfile+2) {
					return 0;
				} else {
					return 50;
				}
			}
		} else {
			return 1500;
		}
	}

	//move toward center.
	int t_dist = 0;
	int dist_from_center = 0;
	if (rank2 > 7) {
		t_dist = rank2 - 7;
	} else {
		t_dist = 7 - rank2;
	}
	dist_from_center += t_dist * t_dist;
	if (file2 > 7) {
		t_dist = file2 - 7;
	} else {
		t_dist = 7 - file2;
	}
	dist_from_center += t_dist * t_dist;

	//backzone gets no bonus.
	if ((krank > 4 && Rrank > 3 && Rrank < krank) ||
		(krank < 3 && Rrank < 4 && Rrank > krank) ||
		(kfile > 4 && Rfile > 3 && Rfile < kfile) ||
		(kfile < 3 && Rfile < 4 && Rfile > kfile)) {
		return 250 - dist_from_center;
	} else {
		return 3000 + (250-dist_from_center);		
	}
}

/* Called to validate player input.

*/
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

	//This stuff shouldn't be necessary as long as list_all_moves is good.
	// state s2 = make_move(s);
	// if (s2.K == s2.k ||
	// 	s2.K == s2.R ||
	// 	s2.K > 63 ||
	// 	s2.R > 63 ||
	// 	s2.k > 63 ||
	// 	kings_too_close(s2)) {
	// 	return false;
	// }

	// if (!player_x) {
	// 	//Player X can put Y in check, but Y cannot move into check.
	// 	if (y_in_check(s2)) {
	// 		return false;
	// 	}
	// }

	// // see if peice can go s->s2 legally.

	// return true;
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
		//TODO: check where?? if s.k == s.R (capture)
		if (s.k == s.R) {
			s.R = 255;
		}
	}
	return s;
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
		return true;
	} else {
		return false;
	}
}


/* Game controller for tests
Controls the play of the game. Runs automatically choosing best plays.
Input:	state s - initial state of the board
		int max_turns - maximum moves per player allowed.
*/
int test_play(state s, int max_turns) {
	int num_turns = 0;
	unsigned char move;

	while (num_turns < max_turns) {
		//Player X goes first.
		move = moveX(s);
		s = make_move(s, move, true);

		move = moveY(s);
		if (move == 255) {
			// if (in_checkmate(s)) {
			// 	cout << "Checkmate.\n";
			// } else {
			// 	cout << "Stalemate.\n";
			// }
			MATE = true;
			break;
		}
		s = make_move(s, move, false);
		if (s.R == 255) {
			// cout << "Draw. Checkmate no longer possible.\n";
			// if (VERBOSE_RESULTS) {
			// 	cout << "Player Y got the Rook!.\n";
			// }
			break;
		}
		num_turns++;
	}

	if (MATE) {
		num_turns++;
		//cout << "Mate on turn " << num_turns << ".\n";
	}// else {
		//cout << "Game concluded in draw after " << num_turns << " full turns.\n";
	//}

	return num_turns;
}


void print_states(vector< pair<int, state> > ranked_boards) {
	char buf[80];
	state s;
	int a,b,c,d,e,f;

	for (int i=0; i<(int)ranked_boards.size(); i++) {
		s = ranked_boards[i].second;
		a = s.K/8 + 1;
		b = s.K%8 + 1;
		c = s.R/8 + 1;
		d = s.R%8 + 1;
		e = s.k/8 + 1;
		f = s.k%8 + 1;
		sprintf(buf,"x.K(%d,%d),x.R(%d,%d),y.K(%d,%d)\n",a,b,c,d,e,f);
		cout << "Rank: " << ranked_boards[i].first << "\t Board: " << buf;
	}

}

void save_states_to_file(vector< pair<int, state> > ranked_boards) {
	ofstream ofile;
	ofile.open("testCases.txt");
	char buf[80];
	state s;
	int a,b,c,d,e,f, len;

	for (int i=0; i<(int)ranked_boards.size(); i++) {
		s = ranked_boards[i].second;
		a = s.K/8 + 1;
		b = s.K%8 + 1;
		c = s.R/8 + 1;
		d = s.R%8 + 1;
		e = s.k/8 + 1;
		f = s.k%8 + 1;
		len = sprintf(buf,"x.K(%d,%d),x.R(%d,%d),y.K(%d,%d)\n",a,b,c,d,e,f);
		ofile.write(buf, len);
	}
	ofile.close();
}


void run_finder() {
	state s;
	int turns = 0;
	int lower_bound = 1;
	vector< pair<int, state> > ranked_boards;

	for (int i=0; i<64; i++) {
		for (int j=0; j<64; j++) {
			for (int k=0; k<4; k++) {// Let's just try up to 4 for now...
				//In fact, can we limit to quadrant 1 and assume all configs met??
				s = state(i, j, k);
				if (!s.is_valid()) {
					continue;
				} else if (kings_too_close(s) || y_in_check(s)) {
					continue;
				}
				turns = test_play(s, 50);
				if (turns > lower_bound) {
					ranked_boards.push_back(make_pair(turns, s));
				}
			}
			if (ranked_boards.size() > 32) {
				sort(ranked_boards.begin(), ranked_boards.end());
				reverse(ranked_boards.begin(), ranked_boards.end());
				ranked_boards.resize(32);
				lower_bound = ranked_boards[31].first;
			}
		}
	}

	sort(ranked_boards.begin(), ranked_boards.end());
	reverse(ranked_boards.begin(), ranked_boards.end());
	ranked_boards.resize(32);

	print_states(ranked_boards);
	//save_states_to_file(ranked_boards);
}


/* Main function

*/
int main() {
	run_finder();
	return 0;
}

