#ifndef HEUR_H
#define HEUR_H

#include "helper.h"

int heuristicX(state s);
int heuristicY(state s);
int get_push_dir(state s);
state orient(state s, int& dir);
void translate_diag(unsigned char& Krank, unsigned char& Kfile,
					unsigned char& Rrank, unsigned char& Rfile);

#endif