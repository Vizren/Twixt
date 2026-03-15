#ifndef BOARD_H
#define BOARD_H

#define BOARD_ROWS 24
#define BOARD_COLS 24

typedef struct {
    int cells[BOARD_ROWS + 1][BOARD_COLS + 1];
} board;

void boardset(board *b);
int boardinbounds(int row, int col);
int boardcheck(const board *b, int row, int col);
int board_place_peg(board *b, int row, int col, int player);
void boardprint(const board *b);

#endif
