/* KRk initial board positions brute forcer.
Author: Phillip Stewart

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

#define TRY_ADD_MOVE_Y s2=make_move(s,move,false);if(!y_in_check(s2)&&!kings_too_close(s2)){moves.push_back(move);}
#define TRY_ADD_KING if(K_can_move(s,move)){moves.push_back(move);}



void print_states(vector< pair<int, state> > ranked_boards);
void save_states_to_file(vector< pair<int, state> > ranked_boards);
void run_finder();

bool operator<(const state& a, const state& b) {
	return a.k<b.k;//return false...
}

/* Game controller for tests
Controls the play of the game. Runs automatically choosing best plays.
Input:	state s - initial state of the board
		int max_turns - maximum moves per player allowed.
*/
int stripped_test_play(state s, int max_turns) {
	bool MATE = false;
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
				turns = stripped_test_play(s, 50);
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

