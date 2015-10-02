/* Move functions for player X and Y
Author: Phillip Stewart

Simple move functions are defined first.
	They return the move with the highest heuristic value for that player.

Attempts at search functions are included after...
	These are not exactly minimax, but variations on the idea.
	None of them performed a lot better than the simple move functions.
	This is mostly because my heuristics are based on immediate maximizing.
-There is a specific style of play which yields the best results, and my
	heuristics are designed to yield moves which adhere to natural play.
	As such, the heuristics of estimated future states only serve to cloud the
		AI's immediate logic.
	So I have elected not to use sophisticated searching in my final program;
		the play() and test_play() functions will call moveX and moveY.
*/


#include <iostream>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <cmath>
#include "helper.h"
#include "heuristic.h"
#include "move.h"
using namespace std;


state REMEMBERED, R2;


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

	move = ranked_moves[0].second;
	if (make_move(s, move, true) == R2) {
		move = ranked_moves[1].second;
	}
	R2 = REMEMBERED;
	REMEMBERED = make_move(s, move, true);
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
		if (DEBUG_VERBOSE) {
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

	move = ranked_moves[0].second;
	return move;
}


/* Same idea as moveX, but has no side-effects */
unsigned char look_moveX(state s) {
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

	move = ranked_moves[0].second;
	return move;
}


/* Expected value maximizing search function
Looks at possible moves and replies, and attempts to adjust the heuristic
	based on the probability the opponent will make a certain move and
	the heuristic values of the states beyond that move.
The numbers are fudged a little (squaring and rooting...) to attempt to improve
	the search results.
*/
unsigned char ex_minimax_moveX(state s, int depth) {
	unsigned char move = 0;

	vector<unsigned char> moves = list_all_moves_x(s);
	if (moves.size() == 0) {
		err("No moves found for X?!?!");
	}
	vector< pair<double, unsigned char> > ranked_moves;
	double rank;
	for (int i=0; i < (int)moves.size(); i++) {
		rank = (double)heuristicX(make_move(s, moves[i], true));
		if (rank > 1.0) {
			ranked_moves.push_back(make_pair(rank, moves[i]));
		}
	}

	// Keep at most 5 best moves.
	sort(ranked_moves.begin(), ranked_moves.end());
	reverse(ranked_moves.begin(), ranked_moves.end());
	ranked_moves.resize(5);

	rank = ranked_moves[0].first;
	// terminal moves should be greater than 30k
	if (rank < 30000.0 && depth > 0) {
		// go through all (rank, move) pairs
		// update rank *= (sum([y_move_prob] . [best_x_H]))
		for (int i=0; i < (int)ranked_moves.size(); i++) {
			rank = ranked_moves[i].first;
			move = ranked_moves[i].second;
			state s2 = make_move(s, move, true);
			vector<unsigned char> y_moves = list_all_moves_y(s2);
			vector< pair<double, state> > y_ranked_states;
			double rank;
			//if Y can't respond, X should use this move.
			if (y_moves.size() == 0) {
				if (DEBUG_VERBOSE) {
					cout << "Found mate in " << depth << " moves.\n";
				}
				return move;
			}
			//find player Y responses [(hY(s3), s3), (_, _), ...]
			for (int j=0; j < (int)y_moves.size(); j++) {
				state s3 = make_move(s2, y_moves[j], false);
				rank = (double)heuristicY(s3);
				if (rank > 1.0) {
					y_ranked_states.push_back(make_pair(rank, s3));
				}
			}
			//sort them by rank and keep at most 3
			sort(y_ranked_states.begin(), y_ranked_states.end());
			reverse(y_ranked_states.begin(), y_ranked_states.end());
			y_ranked_states.resize(3);
			//get the total heuristic for the moves (to make percents)
			double total = 0.0;
			for (int j=0; j < (int)y_ranked_states.size(); j++) {
				total += y_ranked_states[j].first;
			}
			//fix percentages up a little bit...
			for (int j=0; j < (int)y_ranked_states.size(); j++) {
				double temp = y_ranked_states[j].first / total;
				//decrease the probability of opponent choosing bad moves...
				y_ranked_states[j].first = temp * temp;
			}
			//renormalize to total 100%
			total = 0.0;
			for (int j=0; j < (int)y_ranked_states.size(); j++) {
				total += y_ranked_states[j].first;
			}
			for (int j=0; j < (int)y_ranked_states.size(); j++) {
				y_ranked_states[j].first /= total;
			}
			//for each Y move, get prob. Y will make move, and best response
			//multiply current heuristic by average heuristic value
			//of the state after our next move.
			double total2 = 0.0;
			for (int j=0; j < (int)y_ranked_states.size(); j++) {
				state s3 = y_ranked_states[j].second;
				unsigned char best_move = ex_minimax_moveX(s3, depth-1);
				int hX_2nd = heuristicX(make_move(s3, best_move, true));
				total2 += hX_2nd * y_ranked_states[j].first;
			}
			ranked_moves[i].first *= sqrt(total2);//try sqrt??
		}
		//sort the moves again now that the ranks have changed.
		sort(ranked_moves.begin(), ranked_moves.end());
		reverse(ranked_moves.begin(), ranked_moves.end());
	}

	if (DEBUG_VERBOSE && depth == DEPTH) {
		string move_str;
		unsigned char move;
		for (int i=0; i < (int)ranked_moves.size(); i++) {
			rank = ranked_moves[i].first;
			move = ranked_moves[i].second;
			move_str = convert_move_to_PGN(s, move, true);
			cout << "Move: " << move_str << "  h(n): " << rank << endl;
		}
	}

	move = ranked_moves[0].second;
	if (depth == DEPTH) {
		if (make_move(s, move, true) == R2 && ranked_moves.size() > 1) {
			move = ranked_moves[1].second;
		}
		R2 = REMEMBERED;
		REMEMBERED = make_move(s, move, true);
	}
	return move;
}


/* A near-minimax search function for player Y
-Does not do mini, but assumes that opponent will make their best move.
Assigns the heuristic of future states to possible moves.
*/
unsigned char minimax_moveY(state s, int depth) {
	unsigned char move = 0;
	vector<unsigned char> moves = list_all_moves_y(s);
	if (moves.size() == 0) {
		return 255;
	}
	vector< pair<int, unsigned char> > ranked_moves;
	int rank;
	for (int i=0; i < (int)moves.size(); i++) {
		rank = heuristicY(make_move(s, moves[i], false));
		ranked_moves.push_back(make_pair(rank, moves[i]));
	}
	
	//sort and keep at most best 4 moves.
	sort(ranked_moves.begin(), ranked_moves.end());
	reverse(ranked_moves.begin(), ranked_moves.end());
	if (ranked_moves.size() > 4) {
		ranked_moves.resize(4);
	}

	//Look our heuristics to X replies (best X reply...)
	int val;
	int best_val = 0;
	int best_move_index = 0;
	if (depth > 0 && ranked_moves[0].first < 30000) {
		for (int i=0; i<(int)ranked_moves.size(); i++) {
			//s2 is state after our move
			state s2 = make_move(s, ranked_moves[i].second, false);
			unsigned char reply = look_moveX(s2);
			//s3 is state after Xs best reply - skip the minimize step...
			state s3 = make_move(s2, reply, true);
			//recurse and decrement depth
			//mark above move with heuristic of best lower move
			unsigned char recurse_move = minimax_moveY(s3, depth-1);
			if (recurse_move == 255) {
				val = 0;
			} else {
				val = heuristicY(make_move(s3, recurse_move, false));
			}
			if (val > best_val) {
				best_move_index = i;
				best_val = val;
			}
		}
	}

	move = ranked_moves[best_move_index].second;
	return move;
}


/* Similar to the above function, but addition instead of assignment.
Performs maximizing of branches, but adds the heuristic of the best lower
	branch to the state following possible moves.
*/
unsigned char additive_minimax_moveY(state s, int depth) {
	unsigned char move = 0;
	vector<unsigned char> moves = list_all_moves_y(s);
	if (moves.size() == 0) {
		return 255;
	}
	vector< pair<int, unsigned char> > ranked_moves;
	int rank;
	for (int i=0; i < (int)moves.size(); i++) {
		rank = heuristicY(make_move(s, moves[i], false));
		ranked_moves.push_back(make_pair(rank, moves[i]));
	}
	
	//sort and keep at most best 4 moves.
	sort(ranked_moves.begin(), ranked_moves.end());
	reverse(ranked_moves.begin(), ranked_moves.end());
	if (ranked_moves.size() > 4) {
		ranked_moves.resize(4);
	}

	//Look our heuristics to X replies (best X reply...)
	int val;
	int best_val = 0;
	int best_move_index = 0;
	if (depth > 0 && ranked_moves[0].first < 30000) {
		for (int i=0; i<(int)ranked_moves.size(); i++) {
			//s2 is state after our move
			state s2 = make_move(s, ranked_moves[i].second, false);
			unsigned char reply = look_moveX(s2);
			//s3 is state after Xs best reply - skip the minimize step...
			state s3 = make_move(s2, reply, true);
			//recurse and decrement depth
			//mark above move with heuristic of best lower move
			unsigned char recurse_move = minimax_moveY(s3, depth-1);
			if (recurse_move == 255) {
				val = 0;
			} else {
				val = heuristicY(make_move(s3, recurse_move, false)) + ranked_moves[i].first;
			}
			if (val > best_val) {
				best_move_index = i;
				best_val = val;
			}
		}
	}
	move = ranked_moves[best_move_index].second;
	return move;
}


/* Maximizing search function for player X
Similar to the above assignment maximizer for Y
Skips minimization rounds by assuming opponent makes their best move.
*/
unsigned char maximax_moveX(state s, int depth) {
	unsigned char move = 0;

	vector<unsigned char> moves = list_all_moves_x(s);
	if (moves.size() == 0) {
		err("No moves found for X?!?!");
	}
	vector< pair<int, unsigned char> > ranked_moves;
	int s2_rank;
	for (int i=0; i < (int)moves.size(); i++) {
		s2_rank = heuristicX(make_move(s, moves[i], true));
		if (s2_rank > 1) {
			ranked_moves.push_back(make_pair(s2_rank, moves[i]));
		}
	}

	// Keep at most 5 best moves.
	sort(ranked_moves.begin(), ranked_moves.end());
	reverse(ranked_moves.begin(), ranked_moves.end());
	ranked_moves.resize(5);

	// terminal moves should be greater than 30k
	if (ranked_moves[0].first < 30000 && depth > 0) {
		// go through all (rank, move) pairs
		for (int i=0; i < (int)ranked_moves.size(); i++) {
			move = ranked_moves[i].second;
			state s2 = make_move(s, move, true);
			vector<unsigned char> y_moves = list_all_moves_y(s2);
			//if Y can't respond, X should use this move.
			if (y_moves.size() == 0) {
				if (DEBUG_VERBOSE) {
					cout << "Found mate in " << depth << " moves.\n";
				}
				return move;
			}
			//find player Y responses [(hY(s3), s3), (_, _), ...]
			int s3_rank;
			int best_rank = 0;
			state s3;
			for (int j=0; j < (int)y_moves.size(); j++) {
				state temp_state = make_move(s2, y_moves[j], false);
				s3_rank = heuristicY(s3);
				if (s3_rank > best_rank) {
					s3 = temp_state;
					best_rank = s3_rank;
				}
			}

			//get our best response, add our H val to that.
			unsigned char best_move = maximax_moveX(s3, depth-1);
			int hX_2nd = heuristicX(make_move(s3, best_move, true));

			ranked_moves[i].first = hX_2nd;
		}
		//sort the moves again now that the ranks have changed.
		sort(ranked_moves.begin(), ranked_moves.end());
		reverse(ranked_moves.begin(), ranked_moves.end());
	}

	if (DEBUG_VERBOSE && depth == DEPTH) {
		string move_str;
		unsigned char move;
		for (int i=0; i < (int)ranked_moves.size(); i++) {
			int rank = ranked_moves[i].first;
			move = ranked_moves[i].second;
			move_str = convert_move_to_PGN(s, move, true);
			cout << "Move: " << move_str << "  h(n): " << rank << endl;
		}
	}

	move = ranked_moves[0].second;
	if (depth == DEPTH) {
		if (make_move(s, move, true) == R2 && ranked_moves.size() > 1) {
			move = ranked_moves[1].second;
		}
		R2 = REMEMBERED;
		REMEMBERED = make_move(s, move, true);
	}
	return move;
}

