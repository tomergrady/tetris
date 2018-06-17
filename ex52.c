//
// TOMER GRADY grady 205660863.
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#define BOARD_SIZE 20
#define ERROR "Error in system call\n"
#define ERROR_SIZE strlen(ERROR)
#define STDERR 2
#define TRUE 1
#define FAIL -1

typedef enum {
    LEFT, RIGHT, DOWN, FLIP
} Move;
typedef enum {
    STAND, LAY
} State;

typedef struct PLayer {
    int x;
    int y;
    State state;
} Player;

//global variables board and player piece
char gameBoard[BOARD_SIZE][BOARD_SIZE];
struct PLayer player;
void printBoard();
void initialBoard();
void updateMove(Move move);
void clearBoard();

/**
 * write error to stderr
 */
void handleError() {
    write(STDERR, ERROR, ERROR_SIZE);
    exit(FAIL);
}

/**
 * This method initializes the board frame.
 */
void initialBoard() {
    int i = 0, j = 0;
    for (i; i < BOARD_SIZE; i++) {
        j = 0;
        for (j; j < BOARD_SIZE; j++) {
            gameBoard[i][j] = ' ';
            if (j == 0 || j == BOARD_SIZE - 1 || i == BOARD_SIZE - 1) {
                gameBoard[i][j] = '*';
            }
        }
    }
}
/**
 * clear the board from player
 */
void clearBoard() {
    int i = 0, j = 0;
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            if (gameBoard[i][j] == '-') {
                gameBoard[i][j] = ' ';
            }
        }
    }
}

/**
 * function that handles when SIGUSER2 has been sent
 * @param sig
 */
void signal_Handle(int sig) {
    signal(SIGUSR2, signal_Handle);
    // read from pipe what sent
    char input = (char) getchar();
    // change the game piece for a,d,s,w
    switch (input) {
        case 's':
            updateMove(DOWN);
            break;
        case 'w':
            updateMove(FLIP);
            break;
        case 'd':
            updateMove(RIGHT);
            break;
        case 'a':
            updateMove(LEFT);
            break;
            // for q, close
        case 'q':
            exit(0);

        default:
            break;
    }
    printBoard();
}

/**
 * function that handles when SIGALRM  has been sent
 * @param sig
 */
void alarm_handle(int sig) {
    alarm(1);
    signal(SIGALRM, alarm_handle);
    updateMove(DOWN);
    printBoard();

}
/**
 * this function initializes the Player status
 */
void initialPlayer() {
    player.x = 0;
    player.y = BOARD_SIZE / 2;
    player.state = LAY;
}

/**
 * this function prints the global board
 */
void printBoard() {
    if (system("clear") == FAIL) {
        handleError();
    }
    clearBoard();
    // add the player to the game
    int i = player.x, j = player.y;
    gameBoard[i][j] = '-';
    if (player.state == STAND) {
        gameBoard[i - 1][j] = gameBoard[i + 1][j] = '-';
    }
    if (player.state == LAY) {
        gameBoard[i][j - 1] = gameBoard[i][j + 1] = '-';
    }
    //print the board
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; ++j) {
            printf("%c", gameBoard[i][j]);
        }
        printf("\n");
    }

    //if player reaches the end
    if (player.state == LAY && player.x >= BOARD_SIZE - 2) {
        initialPlayer();
        printBoard();
    } else if (player.state == STAND && player.x >= BOARD_SIZE - 3) {
        initialPlayer();
        printBoard();
    }

}
/**
 * this function gets input and update the player move according to it
 * @param move (enum) the player's move
 */
void updateMove(Move move) {
    switch (move) {
        case DOWN:
            if (player.state == LAY) {
                if (player.x <= BOARD_SIZE - 3) {
                    player.x += 1;
                }
            } else if (player.state == STAND) {
                if (player.x <= BOARD_SIZE - 4) {
                    player.x += 1;
                }
            }
            break;
        case RIGHT:
            if (player.state == LAY) {
                if (player.y <= BOARD_SIZE - 4) {
                    player.y += 1;
                }
            } else if (player.state == STAND) {
                if (player.y <= BOARD_SIZE - 3) {
                    player.y += 1;
                }
            }
            break;
        case LEFT:
            if (player.state == LAY) {
                if (player.y >= 3) {
                    player.y -= 1;
                }
            } else if (player.state == STAND) {
                if (player.y >= 2) {
                    player.y -= 1;
                }
            }
            break;
        case FLIP:
            if(player.state == LAY) {
                if(player.x == 0 || player.x >= BOARD_SIZE - 2) { break; }
                player.state = STAND;
            } else {
                if(player.y < 2 || player.y >= BOARD_SIZE - 2) { break; }
                player.state = LAY;
            }
            break;
    }
    printBoard();
}
/**
 * main function. initialize handlers function for signals
 * @return
 */
int main() {
    signal(SIGALRM, alarm_handle);
    //call alarm_hand every second
    //define SIGUSR2
    signal(SIGUSR2, signal_Handle);
    alarm(1);

    initialPlayer();
    initialBoard();
    clearBoard();

    while (TRUE) {
        //wait for signals
        pause();
    }
}
