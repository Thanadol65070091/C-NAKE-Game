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
int score = 0;
int skullx = -1, skully;
int skullPresent = 0;
int prevSkullx = -1, prevSkully;
void drawScore() {
    printf("\e[%iB\e[%iC Score : %d", height + 2, width / 2 - 5, score);
    printf("\e[%iF", height + 2);
}
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
                    printf("\e[%iB\e[%iC◆", diamondy + 1, diamondx + 1);
                    printf("\e[%iF", diamondy + 1);
                }
            }
            if (!skullPresent && skullx < 0 && rand() % 10 == 0) {
                do {
                    skullx = rand() % (height - 2) + 1;
                    skully = rand() % (width - 2) + 1;
                } while ((skullx <= 0 || skully <= 0) || (skullx == diamondx && skully == diamondy));

                printf("\e[%iB\e[%iC💀", skully + 1, skullx + 1);
                printf("\e[%iF", skully + 1);
                skullPresent = 1;
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
                printf("·"); 
            } 
        } 
        printf("|\n");
    } 
} 
void resetGame() {
    head = 0;
    tail = 0;
    gameover = 0;
    diamondx = -1;
    skullx = -1;
    xdir = 1;
    ydir = 0;
    quit = 0;
    score = 0;
    skullPresent = 0;
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
            if (skullx > 0) {
                    printf("\e[%iB\e[%iC💀", skully + 1, skullx + 1);
                    printf("\e[%iF", skully + 1);
            }
            printf("\e[%iB\e[%iC·", y[tail] + 1, x[tail] + 1);
            printf("\e[%iF", y[tail] + 1);

            if (x[head] == diamondx && y[head] == diamondy) {
                diamondx = -1;
                printf("\a");
                if (score >= 0 && skullx != -1) {
                    score++;
                    drawScore();
                }
                else if (score >= 0 && skullx == -1) {
                    score += 1;
                    drawScore();
                }
            } 
            else if (x[head] == skullx && y[head] == skully) {
                skullx = -1;
                printf("\a");
                if (score > 0) {
                    score--;
                    drawScore();
                    skullPresent = 0;
                } else {
                    skullx = -1;
                    skullPresent = 0;
                    diamond();
                }
            }else{
                tail = (tail + 1) % 1000;}

            int newhead = (head + 1) % 1000;
            x[newhead] = (x[head] + xdir + height) % height;
            y[newhead] = (y[head] + ydir + width) % width;
            head = newhead;

            for (int i = tail; i != head; i = (i + 1) % 1000){
                if (x[i] == x[head] && y[i] == y[head]){
                    gameover = 1;}}

            printf("\e[%iB\e[%iC●", y[head] + 1, x[head] + 1);
            printf("\e[%iF", y[head] + 1);
            fflush(stdout);

            usleep(5 * 1000000 / 50);

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
            printf("Your Score : %d\n", score);
            fflush(stdout);
            getchar();
            resetGame();
        }
    }
    printf("\e[?25h");
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}
