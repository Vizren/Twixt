#include "board.h"
#include <stdio.h>

//set calls to empty
void boardset(board *b){
    int r, c;
    for (r = 0; r <= BOARD_ROWS; ++r){
        for (c = 0; c <= BOARD_COLS; ++c){
            b->cells[r][c] = 0;
        }
    }
}

//check if inout is within bounds
int boardinbounds(int row, int col){
    return row >= 1 && row <= BOARD_ROWS && col >= 1 && col <= BOARD_COLS;
}

//check value in cell
int boardcheck(const board *b, int row, int col){
    if (!boardinbounds(row, col)){
        return 0;
    }
    return b->cells[row][col];
}


int board_place_peg(board *b, int row, int col, int player){
    
    //invalid
    if (!boardinbounds(row, col)){
        return 0;
    }

    //corners
    if ((row == 1 || row == BOARD_ROWS) && (col == 1 || col == BOARD_COLS)){
        return 0;
    }

    //right and left
    if (player == 1 && (col == 1 || col == BOARD_COLS)){
        return 0;
    }

    //top and bottom
    if (player == 2 && (row == 1 || row == BOARD_ROWS)){
        return 0;
    }

    //peg already placed
    if (b->cells[row][col] != 0){
        return 0;
    }

    b->cells[row][col] = player;
    return 1;
}

void boardprint(const board *b){
    int r, c;

    printf("    ");
    for (c = 1; c <= BOARD_COLS; ++c){
        printf(" %2d  ", c);
        if(c==1 || c==23){
            printf(" ");
        }
    }
    printf("\n\n");

    for (r = 1; r <= BOARD_ROWS; ++r) {
        printf("%2d  ", r);
        for (c = 1; c <= BOARD_COLS; ++c) {
            if ((r == 1 || r == BOARD_ROWS) && (c == 1)) {
                printf("\033[94m     |\033[0m");
                continue;
            }
            if ((r == 1 || r == BOARD_ROWS) && (c == BOARD_COLS)) {
                printf("    ");
                continue;
            }

            int cell = b->cells[r][c];
            char ch = '.';
            if (cell == 1){
                ch = 'X';
                printf("\033[91m  %c  \033[0m", ch);
            } else if (cell == 2){
                ch = 'O';
                printf("\033[94m  %c  \033[0m", ch);
            } else{
                printf("  %c  ", ch);
            }
            if(c==1 || c==23){
                printf("\033[94m|\033[0m");
            }
        }
        printf(" \n");
        if(r==1 || r==23){
            printf("     ");
            for (int i = 1; i <= BOARD_COLS; ++i){                
                printf("\033[91m-----\033[0m");
            }
            printf("\n");
        } else if(r!=24 && r!=1 && r!=23){
            printf("\033[94m         |\033[0m");
            for (int i = 1; i <= BOARD_COLS-2; ++i){                
                printf("     ");
            }
            printf("\033[94m|\033[0m\n");
        }
    }
}