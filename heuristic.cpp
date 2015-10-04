/* The heuristic functions for players X and Y.
Author: Phillip Stewart

These functions are highly customized...

TODO: update Y with an orient and better AI to push against Rook

*/


#include "helper.h"
#include "heuristic.h"


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

	//Very special case...
	if (s.K == 37 && s.R == 40 && s.k == 55) {
		dir = RIGHT;
		s = orient(s, dir);
	} else if (s.K == 29 && s.R == 16 && s.k == 15) {
		dir = LEFT;
		s = orient(s, dir);
	} else if (s.k == 46 && s.R == 32) {//Another special case to handle...
		dir = RIGHT;
		s = orient(s, dir);
	} else if (s.k == 22 && s.R == 24) {
		dir = LEFT;
		s = orient(s, dir);
	}

	unsigned char Krank = s.K % 8;
	unsigned char Rrank = s.R % 8;
	unsigned char krank = s.k % 8;
	unsigned char Kfile = s.K / 8;
	unsigned char Rfile = s.R / 8;
	unsigned char kfile = s.k / 8;

	//obsolete now?? 
	if (dir == UR) {
		if (Rfile == kfile-1) {
			translate_diag(Krank, Kfile, Rrank, Rfile);
		}
		//dir = UP;
	}

	//R should be 1 rank or file from k, and far from k on that row
	//and the optimal row forces k outside most...
	//K should be close to k, (K-space-k over rook line is best)
	int R_factor = 0;
	int K_factor = 0;
	int R_on_edge_factor = 0;
	//int pin_factor = 0;
	int prot_factor = 0;
	int rd = krank - Rrank;
	int fd = kfile - Rfile;
	int rdk = (krank - Krank - 2)*3;
	int fdk = (kfile - Kfile)*3;

	//If k can capture R, return 0. If checkmate, return 2^16
	if ((rd == -1 || rd == 0 || rd == 1) &&
		(fd == -1 || fd == 0 || fd == 1)) {
		//Unless K adj to R... (protected check)
		if ((Kfile == Rfile-1 || Kfile == Rfile || Kfile == Rfile+1) &&
			(Krank == Rrank-1 || Krank == Rrank || Krank == Rrank+1)) {
			//find dist to K target...
			int r_target = krank;
			int f_target = kfile;
			if (dir == UR) {
				r_target = krank;
				f_target = kfile;
				int r_dist = Krank - r_target;
				if (r_dist < 0) {
					r_dist = 0 - r_dist;
				}
				int f_dist = Kfile - f_target;
				if (f_dist < 0) {
					f_dist = 0 - f_dist;
				}
				prot_factor = 7 - (r_dist+f_dist);
				prot_factor = prot_factor*prot_factor;
			} else if (dir == UP) {
				//tricky...
				if (Rfile > kfile) {
					prot_factor = (7 - Rfile) * 5;
					prot_factor += (Rfile - Kfile) * 10;
				} else {
					prot_factor = Rfile * 5;
					prot_factor += (Kfile - Rfile) * 10;
				}
			} else {
				err("Reached what should be a dead block in player X's heuristic.");
			}
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

	//Push k toward top with R.
	if (Rrank > krank) {
		R_factor = 0;
		if (Rfile == 0 || Rfile == 7) {
			R_on_edge_factor = 250 + rd*rd + fd*fd;
		}
	} else if (Rrank == krank) {
		R_factor = 1000;
	} else if (Rrank == krank-1) {
		if (Krank > Rrank) {
			K_factor = (7-Krank) * 150;
			//Trying to make K get out of bad spot
			K_factor -= 300;
		} else if (Rrank == Krank) {
			//If the king is in the way...
			if ((kfile < Kfile && Kfile < Rfile) ||
				(kfile > Kfile && Kfile > Rfile)) {
				K_factor = Krank * -400;
			} else {
				K_factor = 800;// + ((Kfile-Rfile)*(Kfile-Rfile)); ??
			}
		} else {
			K_factor = 1000;
			if ((Rfile < Kfile && Kfile <= kfile && kfile - Rfile > 2) ||
				(Rfile > Kfile && Kfile >= kfile && Rfile - kfile > 2)) {
				K_factor += 30 * Krank;
			}
			if (Kfile == kfile && Krank == krank-2) {
				K_factor -= 45;
			}
		}
		R_factor = 1000 * Rrank;//1k more for each row up.
		R_factor += fd*fd*2;
		if (Rfile == 0 || Rfile == 7) {
			R_on_edge_factor = 55;
			//TODO: look at kings for added bonus to the correct config.

		}
		
	} else {//Rook below k, more than one rank
		R_factor = 1000 * Rrank;//1k more for each row up.
		R_factor += fd*fd*5;
		//Strange case where rook makes a bad move...
		if (Rrank == krank-2 && Krank > Rrank &&
			(Rfile == kfile-1 || Rfile == kfile+1)) {
			R_on_edge_factor = -1001;
		}
		if (Krank > krank) {
			K_factor = (8-Krank) * 150 - 200;
		} else if (Krank == krank) {
			K_factor = Krank * 30;
		} else {
			if (Krank > Rrank) {
				K_factor = Krank * 40;
			} else if (Rrank == Krank) {
				//If the king is in the way...
				if ((kfile < Kfile && Kfile < Rfile) ||
					(kfile > Kfile && Kfile > Rfile)) {
					K_factor = Krank * -30;
				} else {
					K_factor = Krank * 75;
				}
			} else {
				K_factor = Krank * 150;
			}
		}
	}


	if (Rfile == 0 && kfile > 3) {
		fdk--;
	} else if (Rfile == 7 && kfile < 4) {
		fdk++;
	}

	if (rdk > 0) {
		K_factor += (21-rdk)*(21-rdk);
	} else {
		K_factor += (21+rdk)*(21+rdk);
	} if (fdk > 0) {
		K_factor += (15-fdk)*(15-fdk);
	} else {
		K_factor += (15+fdk)*(15+fdk);
	}
	
	h = R_factor + K_factor + R_on_edge_factor + prot_factor;
	return h;
}


/* Third attempt at Heuristic for player Y
Input:	state s - current state of the board
Output:	int - the value of the board for player Y
			A higher value means the board state is more desireable.
			Bad positions should return a relatively low number
			and good positions should return a high number.
Capture Rook: 65536

Aim for:
center, no R: ~7k
edge: ~2k 	(corner: ~1500, middle ~2k)
R block: -1k
K factor ~ -500 to +500

Get these derived to about-
-check push: ? -500
-checkmate conditions: 0

*/
int heuristicY(state s) {
	//Always capture rook if possible:
	if (s.R == 255) {
		return 65536;
	}

	int h = 0;
	s = dir_and_orientY(s);

	unsigned char Krank = s.K % 8;
	unsigned char Rrank = s.R % 8;
	unsigned char krank = s.k % 8;
	unsigned char Kfile = s.K / 8;
	unsigned char Rfile = s.R / 8;
	unsigned char kfile = s.k / 8;

	unsigned char y_dist = krank * 2;
	unsigned char x_dist = kfile * 2;
	if (y_dist > 7) {
		y_dist -= 7;
	} else {
		y_dist = 7 - y_dist;
	}
	if (x_dist > 7) {
		x_dist -= 7;
	} else {
		x_dist = 7 - x_dist;
	}

	int dist_from_c = x_dist*x_dist + y_dist*y_dist;
	int dist_factor = (((100 - dist_from_c) * (100 - dist_from_c)) / 2) + 2000;
	int R_factor = 0;
	int K_factor = 0;
	// If blocked by rook move toward rook (unless king trap)
	if (krank > 3 && Rrank == krank-1) {//Rook below
		if (Krank == krank - 2 && kfile == Kfile) {//trap
			if (Rfile == kfile + 1 || Rfile == kfile - 1) {
				R_factor = 500;
			} else {
				R_factor = -1000;
				K_factor = -1000;
			}
		} else {//Move toward rook
			if (Rfile < kfile) {
				R_factor = (7 - (kfile - Rfile)) * 150;
			} else {
				R_factor = (7 - (Rfile - kfile)) * 150;
			}
			if (Krank < Rrank) {
				if (Krank == krank-2) {
					K_factor = 0;
				} else {
					K_factor = 250;
				}
			} else {
				K_factor = 500;
			}
		}
	} else if (Rrank == krank || Rfile == kfile) {//same rank as rook:
		//if (y_in_check(s)) {}//should never call heuristic from check...
		R_factor = 100;
	} else if (Rrank > krank) {
		// ?? not sure what to make of this part yet...
		R_factor = 500;
	}//else R_factor = 0;

	if (krank == 0 || krank == 7 || kfile == 0 || kfile == 7) {
		dist_factor -= 500;
	}

	h = dist_factor + R_factor + K_factor;
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
int heuristicY_old2(state s) {
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

	//TODO: rewrite for orient??? maybe..
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


/* Finds the direction of the king, the rook tries to trap to one side.
Return directions are defined in enum:
	0 = No push direction
	1=up, 2=down, 3=left, 4=right
	5=UL, 6=UR, 7=DL, 8=DR
*/
int get_push_dir(state s) {
	int dir = NONE;
	unsigned char krank = s.k % 8;
	unsigned char kfile = s.k / 8;
	unsigned char Rrank = s.R % 8;
	unsigned char Rfile = s.R / 8;

	if (krank == kfile) {
		if (krank < 4) {
			if (Rrank == Rfile) {
				dir = DL;
			} else if (kfile == Rfile-1) {
				dir = LEFT;
			} else if (krank == Rrank-1) {
				dir = DOWN;
			} else if (krank == Rrank+1 && Rfile == 7) {
				dir = UP;
			} else {
				dir = DL;
			}
		} else {
			if (Rrank == Rfile) {
				dir = UR;
			} else if (kfile == Rfile+1) {
				dir = RIGHT;
			} else if (krank == Rrank+1) {
				dir = UP;
			} else if (krank == Rrank-1 && Rfile == 0) {
				dir = DOWN;
			} else {
				dir = UR;
			}
		}
	} else if (7-krank == kfile) {
		if (krank < 4) {
			if (7-Rrank == Rfile) {
				dir = DR;
			} else if (kfile == Rfile+1) {
				dir = RIGHT;
			} else if (krank == Rrank-1) {
				dir = DOWN;
			} else if (krank == Rrank+1 && Rfile == 0) {
				dir = UP;
			} else {
				dir = DR;
			}
		} else {
			if (7-Rrank == Rfile) {
				dir = UL;
			} else if (kfile == Rfile-1) {
				dir = LEFT;
			} else if (krank == Rrank+1) {
				dir = UP;
			} else if (krank == Rrank-1 && Rfile == 7) {
				dir = DOWN;
			} else {
				dir = UL;
			}
		}
	} else if (krank > kfile) {
		if (krank > 7-kfile) {
			dir = UP;
			if (Rrank == 0) {
				if (Rfile == kfile+1) {
					dir = LEFT;
				} else if (Rfile == kfile-1) {
					dir = RIGHT;
				}
			}
		} else {
			dir = LEFT;
			if (Rfile == 7) {
				if (Rrank == krank+1) {
					dir = DOWN;
				} else if (Rrank == krank-1) {
					dir = UP;
				}
			}
		}
	} else {
		if (krank > 7-kfile) {
			dir = RIGHT;
			if (Rfile == 0) {
				if (Rrank == krank+1) {
					dir = DOWN;
				} else if (Rrank == krank-1) {
					dir = UP;
				}
			}
		} else {
			dir = DOWN;
			if (Rrank == 7) {
				if (Rfile == kfile+1) {
					dir = LEFT;
				} else if (Rfile == kfile-1) {
					dir = RIGHT;
				}
			}
		}
	}
	return dir;
}


/* a failed attempt to simplify push_dir */
int get_push_dir_simple(state s) {
	int dir = NONE;
	unsigned char krank = s.k % 8;
	unsigned char kfile = s.k / 8;
	if (krank == kfile) {
		if (krank < 4) {
			dir = DL;
		} else {
			dir = UR;
		}
	} else if (7-krank == kfile) {
		if (krank < 4) {
			dir = DR;
		} else {
			dir = UL;
		}
	} else if (krank > kfile) {
		if (krank > 7-kfile) {
			dir = UP;
		} else {
			dir = LEFT;
		}
	} else {
		if (krank > 7-kfile) {
			dir = RIGHT;
		} else {
			dir = DOWN;
		}
	}
	return dir;
}


/* Reorients the board so that the push direction is UP
Also sets diagonal k to k on the upper-right diag.
This is called in the heuristic functions to simplify state.
*/
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


/* Orient the board for plyaer Y heuristic
Goal is to push down and center.
If Rook is forcing k, return s such that krank = Rrank+1
Else, return s such that k in quadrant 1.
dir is the direction k is from center
	ex: k above middle == UP
*/
state dir_and_orientY(state s) {
	//find out where k is, where R is...
	int dir = NONE;
	unsigned char krank = s.k % 8;
	unsigned char kfile = s.k / 8;
	unsigned char Rrank = s.R % 8;
	unsigned char Rfile = s.R / 8;

	if (krank == kfile) {
		if (krank < 4) {
			if (Rfile == kfile+1) {
				dir = LEFT;
			} else {
				dir = DL;
			}
		} else {
			if (Rfile == kfile-1) {
				dir = RIGHT;
			} else {
				dir = UR;
			}
		}
	} else if (7-krank == kfile) {
		if (krank < 4) {
			if (Rrank == krank+1) {
				dir = DOWN;
			} else {
				dir = DR;
			}
		} else {
			if (Rrank == krank-1) {
				dir = UP;
			} else {
				dir = UL;
			}
		}
	} else {//Not on a diagonal.
		//check if R == ...-1
		if (krank > 4 && Rrank == krank-1) {
			if ((kfile > 4 && Rfile == kfile-1) && Rfile > Rrank) {
				dir = RIGHT;
			} else if ((kfile < 3 && Rfile == kfile+1) && (7-Rfile) > Rrank) {
				dir = LEFT;
			} else {
				dir = UP;
			}
		} else if (krank < 3 && Rrank == krank+1) {
			if ((kfile > 4 && Rfile == kfile-1) && Rfile > (7-Rrank)) {
				dir = RIGHT;
			} else if ((kfile < 3 && Rfile == kfile+1) && Rfile < Rrank) {
				dir = LEFT;
			} else {
				dir = DOWN;
			}
		} else if (kfile > 4 && Rfile == kfile-1) {
			dir = RIGHT;
		} else if (kfile < 3 && Rfile == kfile+1) {
			dir = LEFT;
		} else if (krank > 5 && Rrank == krank-2) {//check if R == ...-2
			dir = UP;
		} else if (krank < 2 && Rrank == krank+2) {
			dir = DOWN;
		} else if (kfile > 5 && Rfile == kfile-2) {
			dir = RIGHT;
		} else if (kfile < 2 && Rfile == kfile+2) {
			dir = LEFT;
		} else if (krank > kfile) { // Rook is not a factor in dir.
			if (krank > 7-kfile) {
				dir = UP;
			} else {
				dir = LEFT;
			}
		} else {
			if (krank > 7-kfile) {
				dir = RIGHT;
			} else {
				dir = DOWN;
			}
		}
	}

	//Rotate the board so k is on top
	switch (dir) {
		case DOWN:
		case DL:
			//rotate 180
			s.k = (7 - s.k%8) + (7 - s.k/8)*8;
			s.K = (7 - s.K%8) + (7 - s.K/8)*8;
			s.R = (7 - s.R%8) + (7 - s.R/8)*8;
			break;
		case LEFT:
		case UL:
			//rotate 90 clockwise
			s.k = (7 - s.k/8) + (s.k%8)*8;
			s.K = (7 - s.K/8) + (s.K%8)*8;
			s.R = (7 - s.R/8) + (s.R%8)*8;
			break;
		case RIGHT:
		case DR:
			//rotate 90 cc
			s.k = (s.k/8) + (7-s.k%8)*8;
			s.K = (s.K/8) + (7-s.K%8)*8;
			s.R = (s.R/8) + (7-s.R%8)*8;
			break;
		default:
			break;
	}
	return s;
}


/* Flips certain boards over the diagonal line 
k should be on the UR diag...
*/
void translate_diag(unsigned char& Krank, unsigned char& Kfile, 
	unsigned char& Rrank, unsigned char& Rfile) {
	unsigned char temp = Krank;
	Krank = Kfile;
	Kfile = temp;
	temp = Rrank;
	Rrank = Rfile;
	Rfile = temp;
}



