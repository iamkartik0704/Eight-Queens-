#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define N 8

int board[N][N];
int rowUsed[N];
int columnused[N];
int diagonal1[2 * N - 1];
int diagonal2[2 * N - 1];

// Track user moves for undo
int move_r[N];
int move_c[N];       // used to keep the track of r and c as we have undo option as well
int moveCount = 0;

// Track previous hints to avoid repetition
int hintUsed[N][N];

void setboard() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            board[i][j] = 0;         // if a cell is assigned zero it means no queens is placed there 
            hintUsed[i][j] = 0;
        }
        rowUsed[i] = columnused[i] = 0;
    }
    for (int i = 0; i < 2 * N - 1; i++)
        diagonal1[i] = diagonal2[i] = 0;
    moveCount = 0;
}

void printBoard() {
    printf("\n    ");
    for (int i = 0; i < N; i++)
        printf("%c ", 'a' + i);
    printf("\n  +-----------------\n");

    for (int i = 0; i < N; i++) {
        printf("%d | ", i + 1);
        for (int j = 0; j < N; j++) {
            printf(board[i][j] ? "Q " : ". ");
        }
        printf("|\n");
    }
    printf("  +-----------------\n");
}

int safetoplace(int r, int c) {         // if gets 1 then pops queen is there already
    if (rowUsed[r] || columnused[c] || diagonal1[r + c] || diagonal2[r - c + N - 1])
        return 0;
    return 1;
}

void placeQueen(int r, int c) {
    board[r][c] = 1;
    rowUsed[r] = columnused[c] = 1;
    diagonal1[r + c] = diagonal2[r - c + N - 1] = 1;

    move_r[moveCount] = r;
    move_c[moveCount] = c;
    moveCount++;
}

void removeQueen(int r, int c) {          // we are changing the index so that garbage value is not stored in the removed index
/*6 goes to 4 
  8 goes to 6 and so on
  */
    board[r][c] = 0;
    rowUsed[r] = columnused[c] = 0;
    diagonal1[r + c] = diagonal2[r - c + N - 1] = 0;

    // Remove from move history if present
    for (int i = 0; i < moveCount; i++) {
        if (move_r[i] == r && move_c[i] == c) {
            for (int j = i; j < moveCount - 1; j++) {
                move_r[j] = move_r[j + 1];          // this stores value where queen is placed so that we may undo if we have the track of it
                move_c[j] = move_c[j + 1];
            }
            moveCount--;
            break;
        }
    }
}

int hintsearch() {

    // ensures hint is not repeated
// how it checks: if cell has 1 leaves that

    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            if (!board[r][c] && !hintUsed[r][c] && safetoplace(r, c)) {
                hintUsed[r][c] = 1;
                printf("\nHINT: Try to place a queen at %c%d.\n", 'a' + c, r + 1);
                return 1;
            }
        }
    }
    printf("\nNo furthur Hints available.\n");
    return 0;
}

int main() {
    char input[100];
    char columnalphabet;
    int rownumber;
    int queensPlaced = 0;
    int hintsRemaining = 3;

    setboard();

    printf("*** Eight Queens Game ***\n");
    printf("Aim: Place 8 queens such that none attacks each other.\n");
    printf("\n\t\t\tGame Menu:\n");
    printf(" >>> Place a queen → e4\n");
    printf(" >>> Undo last move → undo\n");
    printf(" >>> Remove a specific queen → undo e4\n");
    printf(" >>> Get a hint  → hint\n");
    printf(" >>> Quit → quit\n");

    while (queensPlaced < N) {
        printBoard();
        printf("\nQueens placed: %d\n", queensPlaced);
        printf("Enter move: ");

        if (!fgets(input, sizeof(input), stdin))
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "quit") == 0) {
            printf("\nExiting game. Goodbye!\n");
            break;
        }

        if (strcmp(input, "hint") == 0) {
            if (hintsRemaining > 0) {
                if (hintsearch())
                    hintsRemaining--;
                else
                    printf("No hints left to give.\n");
            } else {
                printf("\nHints limit Over!\n");
            }
            continue;
        }

        if (strcmp(input, "undo") == 0) {
            if (moveCount > 0) {
                int r = move_r[moveCount - 1];
                int c = move_c[moveCount - 1];
                removeQueen(r, c);
                queensPlaced--;
                printf("\nRemoved queen from %c%d.\n", 'a' + c, r + 1);
            } else {
                printf("\nNothing to undo!\n");
            }
            continue;
        }

        if (strncmp(input, "undo ", 5) == 0) {
            if (sscanf(input + 5, "%c%d", &columnalphabet, &rownumber) == 2) {
                int c = tolower(columnalphabet) - 'a';
                int r = rownumber - 1;
                if (r < 0 || r >= N || c < 0 || c >= N) {
                    printf("\nInvalid input ; Use a1–h8.\n");
                    continue;
                }
                if (board[r][c] == 1) {
                    removeQueen(r, c);
                    queensPlaced--;
                    printf("\nRemoved queen from %c%d.\n", columnalphabet, rownumber);
                } else {
                    printf("\nNo queen found at %c%d!\n", columnalphabet, rownumber);
                }
            } else {
                printf("\nUsage: undo e4\n");
            }
            continue;
        }

        if (sscanf(input, "%c%d", &columnalphabet, &rownumber) == 2) {
            int c = tolower(columnalphabet) - 'a';
            int r = rownumber - 1;

            if (r < 0 || r >= N || c < 0 || c >= N) {
                printf("\nInvalid input ; Use a1–h8.\n");
                continue;
            }

            if (board[r][c] == 1) {
                printf("\nQueen already exists at %c%d!\n", columnalphabet, rownumber);
                continue;
            }

            if (safetoplace(r, c)) {
                placeQueen(r, c);
                queensPlaced++;
                printf("\nQueen placed at %c%d.\n", columnalphabet, rownumber);
            } else {
                printf("\nMove not allowed! %c%d is under attack.\n", columnalphabet, rownumber);
            }
        } else {
            printf("\nInvalid input. Try d5, undo, undo f6, hint, or quit.\n");
        }
    }

    if (queensPlaced == N) {
        printBoard();
        printf("\nCongratulations! Goal Achieved...\n");
    }

    return 0;
}
