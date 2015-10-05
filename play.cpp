/* Contains the play and test_play harnesses.
Author: Phillip Stewart

play() is used for pitting a user against the AI.
test_play() runs AI for both players.

Each function calls the move search functions, of which I wrote many.
	the calls may be swapped out to let the AI use a different search method.
*/


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cctype>
#include <sstream>
#include "helper.h"
#include "move.h"
#include "play.h"
using namespace std;



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
	string initial_board = board_string(s);
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
			//move = moveY(s);
			move = additive_minimax_moveY(s, DEPTH);
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
	save_results(initial_board, summary);
}


/* Game controller for tests
Controls the play of the game. Runs automatically choosing best plays.
Input:	state s - initial state of the board
		int max_turns - maximum moves per player allowed.
*/
void test_play(state s, int max_turns) {
	bool MATE = false;
	int num_turns = 0;
	unsigned char move;
	string x_move_str, y_move_str;
	vector<string> summary;
	stringstream ss;
	string initial_board = board_string(s);
	while (num_turns < max_turns) {
		//Player X goes first.
		move = moveX(s);
		//move = ex_minimax_moveX(s, DEPTH);
		//move = maximax_moveX(s, DEPTH);
		x_move_str = convert_move_to_PGN(s, move, true);
		s = make_move(s, move, true);
		if (VERBOSE_RESULTS) {
			cout << "\nPlayer X: " << x_move_str << endl;
			print_board(s);
		}

		//move = moveY(s);
		//move = minimax_moveY(s, DEPTH);
		move = additive_minimax_moveY(s, DEPTH);
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
	save_results(initial_board, summary);
	return;// num_turns;
}


void save_results(string initial_board, vector<string> summary) {
	ofstream ofile;
	ofile.open("gameResults.txt");
	ofile << "Initial board:\n" << initial_board << "\nGame summary:\n";
	for (int i=0; i < (int)summary.size(); i++) {
		ofile << summary[i] << endl;
	}
	ofile.close();
}

