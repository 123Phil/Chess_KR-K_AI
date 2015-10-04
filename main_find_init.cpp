/* KRk initial board positions brute forcer.
Author: Phillip Stewart

This module is used to find initial board states which yield the highest moves
	for player X to mate. There are two main functions, one is called when no
	command line arguments are given, the other takes testCases.txt as a
	command line argument. (or any other file with a list of test cases)

To compile and run this module:
$ make main_find
$ ./main_find
> displays the top 32 states from all possible
$ ./main_find testCases.txt
> displays results for the states given in the file

By modifying the moveX and moveY calls in the stripped_test_play function,
	you can quickly test differing search methods for finding moves.
Also, modifying the defined DEPTH in helper.h will yield different results.
*/


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility>
#include "helper.h"
#include "move.h"
#include "play.h"
using namespace std;


/* Functions specific to this module */
int stripped_test_play(state s, int max_turns);
void print_state(state s);
void print_states(vector< pair<int, state> > ranked_boards);
void save_states_to_file(vector< pair<int, state> > ranked_boards);
void run_finder();
vector<state> get_states_from_file(string filename);
void run_tester(string filename);


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
		//move = maximax_moveX(s, DEPTH);
		//move = ex_minimax_moveX(s, DEPTH);
		s = make_move(s, move, true);

		//move = moveY(s);
		//move = minimax_moveY(s, DEPTH);
		move = additive_minimax_moveY(s, DEPTH);
		
		if (move == 255) {
			MATE = true;
			num_turns++;
			break;
		}
		s = make_move(s, move, false);
		if (s.R == 255) {
			break;
		}
		num_turns++;
	}
	return num_turns;
}


/* Prints a single state to stdout */
void print_state(state s) {
	char buf[80];
	int a,b,c,d,e,f;
	a = s.K/8 + 1;
	b = s.K%8 + 1;
	c = s.R/8 + 1;
	d = s.R%8 + 1;
	e = s.k/8 + 1;
	f = s.k%8 + 1;
	sprintf(buf,"x.K(%d,%d),x.R(%d,%d),y.K(%d,%d)\n",a,b,c,d,e,f);
	cout << buf;
}


/* Prints the ranked states to stdout */
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


/* Saves states to file.
Used for saving problematic start cases so that they can be quickly tested
	without having to retest all cases again.
*/
void save_states_to_file(vector< pair<int, state> > ranked_boards) {
	ofstream ofile;
	ofile.open("testCases_worst.txt");
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


/* Test runner when no command line arguments
Runs test_play on all possible starting boards.
-Actually tests 1/4 of all boards since it was the easiest reduction
	while still testing all variations.
Before running test_play, checks to see if the board is valid.
This is very useful for finding problems with the heuristic and search functions.
It will show the longest running tests (capped at 32 for now)
*/
void run_finder() {
	state s;
	int turns = 0;
	int lower_bound = 1;
	vector< pair<int, state> > ranked_boards;

	for (int i=0; i<64; i++) {
		for (int j=0; j<64; j++) {
			for (int k=0; k<28; k++) {
				s = state(i, j, k);
				if (k%8 == 4) {
					k += 4;
				}
				if (!s.is_valid()) {
					continue;
				} else if (kings_too_close(s) || y_in_check(s)) {
					continue;
				}
				turns = stripped_test_play(s, 35);
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


/* Read states from file and return a vector of states */
vector<state> get_states_from_file(string filename) {
	ifstream infile;
	infile.open(filename);
	string line;
	vector<state> states;
	while (getline(infile, line)) {
		int a,b,c,d,e,f,good;
		good = sscanf(line.c_str(),"x.K(%d,%d),x.R(%d,%d),y.K(%d,%d)",&a,&b,&c,&d,&e,&f);
		if (!good) {break;}
		char K = (char)((a-1)*8 + (b-1));
		char R = (char)((c-1)*8 + (d-1));
		char k = (char)((e-1)*8 + (f-1));
		state s(K, R, k);
		if (s.is_valid()) {
			states.push_back(s);
		}
	}
	infile.close();
	return states;
}


/* Test runner when supplied a file of test cases to check. 
Similar to run_finder above, but instead of testing all boards,
	just tests the boards defined in the test case file.
This is useful when testing problematic starting positions
	with searching at great depth.
*/
void run_tester(string filename) {
	vector<state> states = get_states_from_file(filename);
	state s;
	int turns = 0;
	vector< pair<int, state> > ranked_boards;

	for (int i=0; i<(int)states.size(); i++) {
		s = states[i];
		if (!s.is_valid()) {
			continue;
		} else if (kings_too_close(s) || y_in_check(s)) {
			continue;
		}
		turns = stripped_test_play(s, 35);
		ranked_boards.push_back(make_pair(turns, s));
	}

	sort(ranked_boards.begin(), ranked_boards.end());
	reverse(ranked_boards.begin(), ranked_boards.end());

	print_states(ranked_boards);
	//save_states_to_file(ranked_boards);
}


/* Main function
If supplied a command-line argument <testcase.txt>,
	it will try to run the tests defined there.
Otherwise, run tests on all states.
*/
int main(int argc, char** argv) {
	if (argc == 1) {
		run_finder();
	} else {
		run_tester(argv[1]);
	}
	return 0;
}


