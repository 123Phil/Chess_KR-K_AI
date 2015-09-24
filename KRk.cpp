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
#include "KRk.h"
using namespace std;

#define VERBOSE_RESULTS true
#define DEBUG_VERBOSE false
#define INPUT_FILE "testCase.txt"
#define TRY_ADD_MOVE_Y s2=make_move(s,move,false);if(!y_in_check(s2)&&!kings_too_close(s2)){moves.push_back(move);}
#define TRY_ADD_KING if(K_can_move(s,move)){moves.push_back(move);}


void err(string msg) {
	cerr << msg << endl << "Exiting...\n";
	exit(EXIT_FAILURE);
}


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
	if (moves.size() == 0) {
		err("No moves found for X?!?!");
	}
	vector< pair<int, unsigned char> > ranked_moves;
	int rank;
	for (int i=0; i < (int)moves.size(); i++) {
		rank = heuristicX(make_move(s, moves[i], true));
		ranked_moves.push_back(make_pair(rank, moves[i]));
	}
	
	sort(ranked_moves.begin(), ranked_moves.end());
	reverse(ranked_moves.begin(), ranked_moves.end());

	if (DEBUG_VERBOSE) {
		string move_str;
		unsigned char move;
		for (int i=0; i < (int)ranked_moves.size(); i++) {
			rank = ranked_moves[i].first;
			move = ranked_moves[i].second;
			move_str = convert_move_to_PGN(s, move, true);
			cout << "Move: " << move_str << "  h(n): " << rank << endl;
		}
	}

	// recurse??

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
		if (VERBOSE_RESULTS) {
			cout << "No moves found for Y...\n";
		}
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

	if (DEBUG_VERBOSE) {
		string move_str;
		unsigned char move;
		for (int i=0; i < (int)ranked_moves.size(); i++) {
			rank = ranked_moves[i].first;
			move = ranked_moves[i].second;
			move_str = convert_move_to_PGN(s, move, false);
			cout << "Move: " << move_str << "  h(n): " << rank << endl;
		}
	}

	// recurse??

	move = ranked_moves[0].second;
	return move;
}


//0 = No push direction
//1=up, 2=down, 3=left, 4=right
//5=UL, 6=UR, 7=DL, 8=DR
int get_push_dir(state s) {
	int dir = NONE;
	unsigned char Krank = s.K % 8;
	unsigned char Rrank = s.R % 8;
	unsigned char krank = s.k % 8;
	unsigned char Kfile = s.K / 8;
	unsigned char Rfile = s.R / 8;
	unsigned char kfile = s.k / 8;
	if (krank >= 4 && Rrank == krank-1) {//up
		if (kfile < 4 && Rfile == kfile+1)  {//left
			if (7-Krank == Kfile) {//UL
				dir = UL;
			} else if (7-Krank > Kfile) {//left more dominant
				dir = LEFT;
			} else {//up more dominant
				dir = UP;
			}
		} else if (kfile >= 4 && Rfile == kfile-1) {//right
			if (Krank == Kfile) {//UR
				dir = UR;
			} else if (Krank < Kfile) {//right more dominant
				dir = RIGHT;
			} else {//up more dominant
				dir = UP;
			}
		} else {//no left-right, just up
			dir = UP;
		}
	} else if (krank < 4 && Rrank == krank+1) {//down
		if (kfile < 4 && Rfile == kfile+1)  {//left
			if (Krank == Kfile) {//DL
				dir = DL;
			} else if (Krank > Kfile) {//left more dominant
				dir = LEFT;
			} else {//down more dominant
				dir = DOWN;
			}
		} else if (kfile >= 4 && Rfile == kfile-1) {//right
			if (Krank == 7-Kfile) {//DR
				dir = DR;
			} else if (Krank > 7-Kfile) {//right more dominant
				dir = RIGHT;
			} else {//down more dominant
				dir = DOWN;
			}
		} else {//no left-right, just down
			dir = DOWN;
		}
	} else if (kfile < 4 && Rfile == kfile+1) {//left only
		dir = LEFT;
	} else if (kfile >= 4 && Rfile == kfile-1) {//right only
		dir = RIGHT;
	}
	return dir;
}


//return the state so that we are pushing up or UR
state orient(state s, int& dir) {
	if (dir == UP || dir == UR || dir == NONE) {
		return s;
	}
	switch (dir) {
		case DOWN:
		case DL:
			s.k = (7 - s.k%8) + (7 - s.k/8)*8;
			s.K = (7 - s.K%8) + (7 - s.K/8)*8;
			s.R = (7 - s.R%8) + (7 - s.R/8)*8;
			break;
		case LEFT:
		case UL:
			s.k = (7 - s.k/8) + (s.k%8)*8;
			s.K = (7 - s.K/8) + (s.K%8)*8;
			s.R = (7 - s.R/8) + (s.R%8)*8;
			break;
		case RIGHT:
		case DR:
			s.k = (s.k/8) + (7-s.k%8)*8;
			s.K = (s.K/8) + (7-s.K%8)*8;
			s.R = (s.R/8) + (7-s.R%8)*8;
			break;
		default:
			err("oops.");
	}
	if (dir == DOWN || dir == LEFT || dir == RIGHT) {
		dir = UP;
	} else {
		dir = UR;
	}
	return s;
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
	int dir = get_push_dir(s);
	//orient fixes dir and points up or UR (or none)
	s = orient(s, dir);

	unsigned char Krank = s.K % 8;
	unsigned char Rrank = s.R % 8;
	unsigned char krank = s.k % 8;
	unsigned char Kfile = s.K / 8;
	unsigned char Rfile = s.R / 8;
	unsigned char kfile = s.k / 8;

	//R should be 1 rank or file from k, and far from k on that row
	//and the optimal row forces k outside most...
	//K should be close to k, (K-space-k over rook line is best)
	int R_factor = 0;
	int K_factor = 0;
	int R_on_edge_factor = 0;
	int pin_factor = 0;
	int prot_factor = 0;
	int rd = krank - Rrank;
	int fd = kfile - Rfile;
	int rdk = krank - Krank;
	int fdk = kfile - Kfile;

	//If k can capture R, return 0. If checkmate, return 2^16
	if ((rd == -1 || rd == 0 || rd == 1) &&
		(fd == -1 || fd == 0 || fd == 1)) {
		//TODO: unless K adj to R... (protected check)
		if ((Kfile == Rfile-1 || Kfile == Rfile || Kfile == Rfile+1) &&
			(Krank == Rrank-1 || Krank == Rrank || Krank == Rrank+1)) {
			//find dist to K target...
			int r_target = krank;
			int f_target = kfile;
			//TODO: not just if up,down,l,r... which is closer to edge?
			if (Rrank == krank-1 && Rfile == kfile-1) {//k is u-r
				if (Rrank < Rfile) {
					r_target -= 2;
				} else if (Rrank > Rfile) {
					f_target -= 2;
				}
			} else if (Rrank == krank-1 && Rfile == kfile+1) {//k is u-l
				if (Rrank < Rfile) {
					r_target -= 2;
				} else if (Rrank > Rfile) {
					f_target += 2;
				}
			} else if (Rrank == krank+1 && Rfile == kfile+1) {//k is d-l
				if (Rrank < Rfile) {
					r_target += 2;
				} else if (Rrank > Rfile) {
					f_target += 2;
				}
			} else if (Rrank == krank+1 && Rfile == kfile-1) {//k is d-r
				if (Rrank < Rfile) {
					r_target += 2;
				} else if (Rrank > Rfile) {
					f_target -= 2;
				}
			}
			int r_dist = Krank - r_target;
			if (r_dist < 0) {
				r_dist = 0 - r_dist;
			}
			int f_dist = Kfile - f_target;
			if (f_dist < 0) {
				f_dist = 0 - f_dist;
			}
			prot_factor = 10 - (r_dist+f_dist);
			prot_factor = prot_factor*prot_factor;
			//return prot_factor;
		} else {
			return 0;
		}
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

	//if K on wrong side, add 0
	//if K on line - add 100 if between R&k, else add 500
	//if K on correct side - 3k + rdk stuff
	if (dir == 0) {
		//
	} else if (dir == 1) {//Push to top...
		rdk = krank - Krank;
		fdk = kfile - Kfile;
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
	} else if (dir == 5) {//UL
		rdk = krank - Krank;
		fdk = kfile - Kfile;
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
	} else {
		err("double oops...");
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

	h = R_factor + K_factor + R_on_edge_factor + pin_factor + prot_factor;
	return rdk;//h;
}


/* Heuristic for player Y
Input:	state s - current state of the board
Output:	int - the value of the board for player Y
			A higher value means the board state is more desireable.
			Bad positions should return a relatively low number
			and good positions should return a high number.
*/
int heuristicY_old(state s) {
	//Always capture rook if possible:
	if (s.R == 255) {
		return 65536;
	}
	int h = 0;
	unsigned char Krank = s.K % 8;
	unsigned char Rrank = s.R % 8;
	unsigned char krank = s.k % 8;
	unsigned char Kfile = s.K / 8;
	unsigned char Rfile = s.R / 8;
	unsigned char kfile = s.k / 8;
	unsigned char rank2 = krank * 2;
	unsigned char file2 = kfile * 2;
	int t_dist = 0;
	int dist_from_center = 0;
	//Try to move toward the center.
	if (rank2 > 7) {
		t_dist = rank2 - 5;
	} else {
		t_dist = 9 - rank2;
	}
	dist_from_center += t_dist * t_dist;
	if (file2 > 7) {
		t_dist = file2 - 5;
	} else {
		t_dist = 9 - file2;
	}
	dist_from_center += t_dist * t_dist;
	
	//Try to annoy the Rook. -but don't walk into trap.
	//TODO: initially high if on center side of rook, low if trapped
	int rook_factor = 2;

	if (Rrank == krank + 1) {//Rook above
		if (krank > 4) {
			rook_factor = 0;
		} else if (Krank == krank + 2 && kfile == Kfile) {//trap
			rook_factor = 0;
		} else {//Move toward rook
			if (Rfile < kfile) {
				rook_factor = (8 - (kfile - Rfile)) * 2;
			} else {
				rook_factor = (8 - (Rfile - kfile)) * 2;
			}
		}
	} else if (Rrank == krank - 1) {//Rook below
		if (krank < 3) {
			rook_factor = 0;
		} else if (Krank == krank - 2 && kfile == Kfile) {//trap
			rook_factor = 0;
		} else {//Move toward rook
			if (Rfile < kfile) {
				rook_factor = (8 - (kfile - Rfile)) * 2;
			} else {
				rook_factor = (8 - (Rfile - kfile)) * 2;
			}
		}
	} else if (Rfile == kfile - 1) {//Rook on left
		if (kfile < 3) {
			rook_factor = 0;
		} else if (Kfile == kfile - 2 && krank == Krank) {//trap
			rook_factor = 0;
		} else {//Move toward rook
			if (Rrank < krank) {
				rook_factor = (8 - (krank - Rrank)) * 2;
			} else {
				rook_factor = (8 - (Rrank - krank)) * 2;
			}
		}
	} else if (Rfile == kfile + 1) {//Rook on right
		if (kfile > 4) {
			rook_factor = 0;
		} else if (Kfile == kfile + 2 && krank == Krank) {//trap
			rook_factor = 0;
		} else {//Move toward rook
			if (Rrank < krank) {
				rook_factor = (8 - (krank - Rrank)) * 2;
			} else {
				rook_factor = (8 - (Rrank - krank)) * 2;
			}
		}
	} else { //same row as rook...
		if (y_in_check(s)) {
			rook_factor = 0;
		} else {
			rook_factor = 10;
		}
	}

	//K factor...
	// Add more if K on same side as k
	// Add a lot if K blocks and allows escape - but more if escaped...
	//If the king is blocking you from center, lower the K_factor...
	//Direct blocks (k-space-K) are bad...
	int K_factor = 30;
	if (krank > 4 && Krank > 3 && krank > Krank) {
		if (Kfile == kfile) {
			K_factor -= (5 - (krank - Krank)) * 10;
		} else {
			//TODO: depends on how close...
			K_factor = 20;
		}
	} else if (krank < 3 && Krank < 4 && krank < Krank) {
		if (Kfile == kfile) {
			K_factor -= (5 - (Krank - krank)) * 10;
		} else {
			K_factor = 20;
		}
	} else if (kfile > 4 && Kfile > 3 && kfile > Kfile) {
		if (Krank == krank) {
			K_factor -= (5 - (kfile - Kfile)) * 10;
		} else {
			K_factor = 20;
		}
	} else if (kfile < 3 && Kfile < 4 && kfile < Kfile) {
		if (Krank == krank) {
			K_factor -= (5 - (Kfile - kfile)) * 10;
		} else {
			K_factor = 20;
		}
	}

	h = (200 - dist_from_center) + rook_factor*rook_factor;// + K_factor;
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

	//TODO: rewrite for orient...
	//  which means, make a get_Y_dir()...

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
		if (DEBUG_VERBOSE) {
			cout << "Checkmate found!\n";
		}
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
void play(state s, int max_turns, bool x_ai) {
	int num_turns = 0;
	unsigned char move;
	string x_move_str, y_move_str, response;
	vector<string> summary;
	stringstream ss;
	while (num_turns < max_turns) {
		//Player X goes first.
		if (x_ai) {
			move = moveX(s);
		} else {
			move = 255;
			while (move == 255) {
				cout << "Enter a valid move: ";
				cin >> response;
				move = convert_PGN_to_move(response, true);
				if (!is_valid_move(s, move, true)) {
					cout << "Invalid move, try again.\n";
					move = 255;
				}
			}
		}

		x_move_str = convert_move_to_PGN(s, move, true);
		s = make_move(s, move, true);
		if (VERBOSE_RESULTS) {
			cout << "\nPlayer X: " << x_move_str << endl;
			print_board(s);
		}

		//Player Y's turn:
		if (!x_ai) {
			move = moveY(s);
		} else {
			if (in_checkmate(s)) {
				cout << "Checkmate.\n";
				ss << right << setw(2) << num_turns + 1;
				ss << ". " << x_move_str << " {Checkmate. Player X wins.}";
				summary.push_back(ss.str());
				break;
			}
			move = 255;
			while (move == 255) {
				cout << "Enter a valid move: ";
				cin >> response;
				move = convert_PGN_to_move(response, false);
				if (!is_valid_move(s, move, false)) {
					cout << "Invalid move, try again.\n";
					move = 255;
				}
			}
		}
		if (move == 255) {
			if (in_checkmate(s)) {
				cout << "Checkmate.\n";
			} else {
				cout << "Stalemate.\n";
			}
			ss << right << setw(2) << num_turns + 1;
			ss << ". " << x_move_str << " {Checkmate. Player X wins.}";
			summary.push_back(ss.str());
			break;
		}

		y_move_str = convert_move_to_PGN(s, move, false);
		ss << right << setw(2) << num_turns + 1;
		ss << ". " << x_move_str << " " << y_move_str;
		summary.push_back(ss.str());
		ss.str(string());

		s = make_move(s, move, false);
		if (VERBOSE_RESULTS) {
			cout << "\nPlayer Y: " << y_move_str << endl;
			print_board(s);
		}

		if (s.R == 255) {
			cout << "Draw. Checkmate no longer possible.\n";
			if (VERBOSE_RESULTS) {
				cout << "Player Y got the Rook!.\n";
			}
			break;
		}
		num_turns++;
	}

	cout << "Game ended after " << num_turns << " full turns.\n";

	if (VERBOSE_RESULTS) {
		cout << "Game summary:\n";
		for (int i=0; i < (int)summary.size(); i++) {
			cout << summary[i] << endl;
		}
	}
}


/* Game controller for tests
Controls the play of the game. Runs automatically choosing best plays.
Input:	state s - initial state of the board
		int max_turns - maximum moves per player allowed.
*/
int test_play(state s, int max_turns) {
	bool MATE = false;
	int num_turns = 0;
	unsigned char move;
	string x_move_str, y_move_str;
	vector<string> summary;
	stringstream ss;
	while (num_turns < max_turns) {
		//Player X goes first.
		move = moveX(s);
		x_move_str = convert_move_to_PGN(s, move, true);
		s = make_move(s, move, true);
		if (VERBOSE_RESULTS) {
			cout << "\nPlayer X: " << x_move_str << endl;
			print_board(s);
		}

		move = moveY(s);
		if (move == 255) {
			if (in_checkmate(s)) {
				cout << "Checkmate.\n";
			} else {
				cout << "Stalemate.\n";
			}
			ss << right << setw(2) << num_turns + 1;
			ss << ". " << x_move_str << " {Checkmate. Player X wins.}";
			summary.push_back(ss.str());
			MATE = true;
			break;
		}

		y_move_str = convert_move_to_PGN(s, move, false);
		ss << right << setw(2) << num_turns + 1;
		ss << ". " << x_move_str << " " << y_move_str;
		summary.push_back(ss.str());
		ss.str(string());

		s = make_move(s, move, false);
		if (VERBOSE_RESULTS) {
			cout << "\nPlayer Y: " << y_move_str << endl;
			print_board(s);
		}

		if (s.R == 255) {
			cout << "Draw. Checkmate no longer possible.\n";
			if (VERBOSE_RESULTS) {
				cout << "Player Y got the Rook!.\n";
			}
			break;
		}
		num_turns++;
	}

	if (MATE) {
		num_turns++;
		cout << "Mate on turn " << num_turns << ".\n";
	} else {
		cout << "Game concluded in draw after " << num_turns << " full turns.\n";
	}

	if (VERBOSE_RESULTS) {
		cout << "Game summary:\n";
		for (int i=0; i < (int)summary.size(); i++) {
			cout << summary[i] << endl;
		}
	}
	return num_turns;
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


/* Ask for the max moves (turns)

*/
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


/* Return initial board state.

*/
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
		return get_state_from_file();//TODO refactor function...
	} else if (y_or_n == 'n') {
		return get_state_from_stdin();
	} else {
		err("Invalid input.");
		//suppress warning...
		return state(0,0,0);
	}
}


/* Read initial state from file.

*/
state get_state_from_file() {
	cout << "\n---------------------------------------\n";
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
		err("Invalid board configuration.");
	} else {
		cout << "Loaded game:\n" << line << endl;
		cout << "---------------------------------------\n";
		if (VERBOSE_RESULTS) {
			print_board(s);
		}
	}
	infile.close();
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
		err("Invalid board configuration.");
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


/* Ask if they want to be player X
*/
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


void test_heuristics() {
	state s = get_state_from_file();
	vector<unsigned char> moves;
	moves = list_all_moves_x(s);
	unsigned char move;
	string move_str;
	vector< pair<int, unsigned char> > ranked_moves;
	int i, rank;

	for (i=0; i < (int)moves.size(); i++) {
		rank = heuristicX(make_move(s, moves[i], true));
		ranked_moves.push_back(make_pair(rank, moves[i]));
	}
	sort(ranked_moves.begin(), ranked_moves.end());
	reverse(ranked_moves.begin(), ranked_moves.end());

	cout << "Player X's moves:\n";
	for (i=0; i < (int)ranked_moves.size(); i++) {
		rank = ranked_moves[i].first;
		move = ranked_moves[i].second;
		move_str = convert_move_to_PGN(s, move, true);
		cout << "Move: " << move_str << "  h(n): " << rank << endl;
	}

	moves.clear();
	ranked_moves.clear();
	moves = list_all_moves_y(s);
	for (i=0; i < (int)moves.size(); i++) {
		rank = heuristicY(make_move(s, moves[i], false));
		ranked_moves.push_back(make_pair(rank, moves[i]));
	}
	sort(ranked_moves.begin(), ranked_moves.end());
	reverse(ranked_moves.begin(), ranked_moves.end());

	cout << "\nPlayer Y's moves:\n";
	for (int i=0; i < (int)ranked_moves.size(); i++) {
		rank = ranked_moves[i].first;
		move = ranked_moves[i].second;
		move_str = convert_move_to_PGN(s, move, false);
		cout << "Move: " << move_str << "  h(n): " << rank << endl;
	}
	err("Finished with heuristics test.");
}


void test_orient(state s) {
	int dir = RIGHT;//LEFT, DOWN...
	s = orient(s, dir);
	cout << "Should be: " << UP << " -- " << dir << endl;
	print_board(s);
	err("Finished with orient test.");
}




