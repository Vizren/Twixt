#include "twixt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int KNIGHT_DRS[8] = { 1, 1, -1, -1, 2, 2, -2, -2 };
static const int KNIGHT_DCS[8] = { 2, -2, 2, -2, 1, -1, 1, -1 };

void help(void){
    printf("\nCommands:\n");
    printf("  place row col  - place a peg at (row,column)\n");
    printf("  help           - help (display these commands)\n");
    printf("  quit           - quit\n");
    printf("\nPlayer 1 cannot play in leftmost and rightmost rows\nPlayer 2 cannot play in top and bottom columns\n");
}

void createlink(board *b, LinkSet *links, int row, int col, int player){
    int k;
    for (k = 0; k < 8; ++k) {
        int nr = row + KNIGHT_DRS[k];
        int nc = col + KNIGHT_DCS[k];

        if (!boardinbounds(nr, nc)){
            continue;
        }
        if (boardcheck(b, nr, nc) == player){
            linkadd(links, row, col, nr, nc, player);
        }
    }
}

void twixt(void){
    board b;
    LinkSet links;
    int current = PLAYER_ONE;
    char line[256];

    boardset(&b);
    linkset(&links);

    help();

    while (1) {
        printf("\n");
        boardprint(&b);
        printf("\nPlayer 1 Links:\n");
        linkprint(&links, 1);
        printf("\nPlayer 2 Links:\n");
        linkprint(&links, 2);

        printf("\nPlayer %d> ", current);

        if (!fgets(line, sizeof(line), stdin)){
            break;
        }
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "quit") == 0){
            printf("\nPlayer %d resigned!",current);
            current = (current == PLAYER_ONE ? PLAYER_TWO : PLAYER_ONE);
            printf("\nPlayer %d wins!\n\n",current);
            break;
        } else if (strcmp(line, "help") == 0){
            help();
        } else if (strncmp(line, "place",5) == 0) {
            int r, c;
            if (sscanf(line + 5, "%d %d", &r, &c) != 2) {
                printf("Usage: place row col\n");
                continue;
            }
            if (!board_place_peg(&b, r, c, current)) {
                printf("Invalid placement.\n");
                continue;
            }

            createlink(&b, &links, r, c, current);
            if(link_player_wins(&links,current,BOARD_ROWS,BOARD_COLS)){
                boardprint(&b);
                printf("\nPlayer 1 Links:\n");
                linkprint(&links, 1);
                printf("\n");
                printf("\nPlayer 2 Links:\n");
                linkprint(&links, 2);
                printf("\nPlayer %d wins!\n\n",current);
                break;
            }

            current = (current == PLAYER_ONE ? PLAYER_TWO : PLAYER_ONE);
        } else {
            printf("Unknown command.\n");
        }
    }
}