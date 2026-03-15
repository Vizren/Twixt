#ifndef TWIXT_H
#define TWIXT_H

#include "board.h"
#include "link.h"

#define PLAYER_ONE 1
#define PLAYER_TWO 2

void twixt(void);
void help(void);
void createlink(board *b, LinkSet *links, int row, int col, int player);

#endif
