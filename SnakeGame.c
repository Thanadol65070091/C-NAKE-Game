#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <time.h>
int i, j, height = 30, width = 30; 
int head = 0, tail = 0;
int gameover = 0;
int xdir = 1, ydir = 0;
int diamondx = -1, diamondy;
int x[1000], y[1000];
int quit = 0;
void diamond(){
    if (diamondx < 0) {
                srand(time(NULL));
                do {
                    diamondx = rand() % (height - 2) + 1;
                    diamondy = rand() % (width - 2) + 1;
                } while (diamondx <= 0 || diamondy <= 0);

                for (int i = tail; i != head; i = (i + 1) % 1000)
                    if (x[i] == diamondx && y[i] == diamondy)
                        diamondx = -1;

                if (diamondx >= 0) {
                    printf("\e[%iB\e[%iC⟡", diamondy + 1, diamondx + 1);
                    printf("\e[%iF", diamondy + 1);
                }
            }
        }
void draw() 
{  
    for (i = 0; i < height; i++) { 
         printf("|");
        for (j = 0; j < width; j++) { 
            if (i == 0 || i == width - 1
                ) { 
                printf("-"); 
            } 
            else { 
                printf("."); 
            } 
        } 
        printf("|\n");
    } 
} 
int main() {
    printf("\e[?25l");
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while (!quit) {
        draw();
        printf("\e[%iA", width+1);
        x[head] = height / 2;
        y[head] = width / 2;
        while (!quit && !gameover) {
            diamond();
            printf("\e[%iB\e[%iC·", y[tail] + 1, x[tail] + 1);
            printf("\e[%iF", y[tail] + 1);

            if (x[head] == diamondx && y[head] == diamondy) {
                diamondx = -1;
                printf("\a");
            } else{
                tail = (tail + 1) % 1000;}

            int newhead = (head + 1) % 1000;
            x[newhead] = (x[head] + xdir + height) % height;
            y[newhead] = (y[head] + ydir + width) % width;
            head = newhead;

            for (int i = tail; i != head; i = (i + 1) % 1000){
                if (x[i] == x[head] && y[i] == y[head]){
                    gameover = 1;}}

            printf("\e[%iB\e[%iC0", y[head] + 1, x[head] + 1);
            printf("\e[%iF", y[head] + 1);
            fflush(stdout);

            usleep(5 * 1000000 / 40);

            struct timeval tv;
            fd_set fds;
            tv.tv_sec = 0;
            tv.tv_usec = 0;

            FD_ZERO(&fds);
            FD_SET(STDIN_FILENO, &fds);
            select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
            if (FD_ISSET(STDIN_FILENO, &fds)) {
                int ch = getchar();
                if (ch == 27 || ch == 'q') {
                    quit = 1;
                } else if (ch == 'a' && xdir != 1) {
                    xdir = -1;
                    ydir = 0;
                } else if (ch == 'd' && xdir != -1) {
                    xdir = 1;
                    ydir = 0;
                } else if (ch == 's' && ydir != -1) {
                    xdir = 0;
                    ydir = 1;
                } else if (ch == 'w' && ydir != 1) {
                    xdir = 0;
                    ydir = -1;
                }
            }
        }
        if (!quit) {
            printf("\e[%iB\e[%iC Game Over! ", width / 2, (height / 2) - 5);
            printf("\e[%iF", width / 2);
            fflush(stdout);
            getchar();
            }
    }
    printf("\e[?25h");
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}