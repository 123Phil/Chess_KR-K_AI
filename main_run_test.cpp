/* Main handler for testing and verifying functions
Author: Phillip Stewart

Some of the functions in other modules were a little crazy,
	so I wrote wrappers here that call them and show their output
	so that I could verify their operation.

For example, the test_heuristics() is helpful for seeing the
	heuristic values of all possible moves givan a game-state.
*/


#include <iostream>
#include "helper.h"
#include "heuristic.h"
using namespace std;


/* Functions specific to this module */
void verify_lam(state s);
void test_heuristics();
void test_orient(state s);


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


/* Look at all possible moves from a state and show their value */
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


/* Just wanted to make sure that orient was working */
void test_orient(state s) {
	int dir = RIGHT;//LEFT, DOWN...
	s = orient(s, dir);
	cout << "Should be: " << UP << " -- " << dir << endl;
	print_board(s);
	err("Finished with orient test.");
}


/* Calls test functions... */
int main() {
	test_heuristics();
	//test_orient(get_state_from_file());
	//verify_lam(get_state_from_file());
}


