
// Tomer Grady, 205660863


#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <memory.h>
#include <stdlib.h>
#include <signal.h>

#define ERROR "Error in system call\n"
#define ERROR_SIZE strlen(ERROR)
#define STDERR 2
#define TRUE 1
#define FAIL -1

/**
 * write error to stderr
 */
void handleError() {
    write(STDERR, ERROR, ERROR_SIZE);
    exit(-1);
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}
/**
 * check if the char is a / s / d / w / q
 * @param c given char
 * @return 1 if the char is valid, 0 otherwise
 */
int isValidKey(char c) {
    switch (c) {
        case 'a':
        case 's':
        case 'd':
        case 'w':
        case 'q':
            return 1;
        default:
            return 0;
    }
}

/**
 * main function, crates a child runs draw.out
 * @return 0 or -1 when error in system call
 */
int main() {
    int Pipe[2];
    char inputChar;
    pipe(Pipe);
    int pid;
    if ((pid = fork()) < 0) {
        // error
        handleError();
    }
    //we are in the sun proccess - runs draw.out
    if (pid == 0) {
        if (dup2(Pipe[0], 0) < 0) {
            handleError();
        }
        char *args[32] = {"./draw.out", NULL};
        if (execvp(args[0], &args[0]) < 0) {
            handleError();
        }

    } else {
        while (TRUE) {
            inputChar = getch();
            if (isValidKey(inputChar)) {
                if (write(Pipe[1], &inputChar, 1) < 0) { handleError(); }
                // send sigusr2 to child
                if (kill(pid, SIGUSR2) < 0) { handleError(); }
            }
            if (inputChar == 'q') { break; };

        }
    }
}