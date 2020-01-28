#include "computer.h"
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <map>
#include <set>
#include <vector>
#include <sys/poll.h>

#define EMPTY 0
#define WALL 1
#define BLOCK 2
#define PADDLE 3
#define BALL 4

typedef std::pair<int, int> Point;

int main(int argc, char** argv) {
    FILE *file = fopen(argv[1], "r");

    el_type temp;
    std::vector<el_type> mem;
    fscanf(file, "%li", &temp);
    mem.push_back(temp);
    while (fscanf(file, ",%li", &temp) == 1) {
        mem.push_back(temp);
    }
    printf("Nums read: %i\n", mem.size());

    Computer puter;

    puter.load(mem);
    puter.reset();
    puter.run_to_done();

    std::vector<el_type> output = puter.release_output();
    std::map<Point, int> screen;

    auto iter = output.begin();
    while(iter != output.end()) {
        int x = *iter;
        iter++;
        int y = *iter;
        iter++;
        int id = *iter;
        iter++;

        screen[Point(x,y)] = id;
    }

    unsigned count = 0;
    int max_x = 0;
    int max_y = 0;
    for(auto map_iter = screen.begin(); map_iter != screen.end(); map_iter++) {
        max_x = std::max(max_x, map_iter->first.first);
        max_y = std::max(max_y, map_iter->first.second);

        if(map_iter->second == BLOCK) {
            count++;
        }
    }

    printf("Num Blocks: %i, max_x is: %i, max_y is: %i\n", count, max_x, max_y);
    for(int y=0; y<max_y; y++) {
        for(int x=0; x<max_x; x++) {
            char c = ' ';
            auto check = screen.find(Point(x, y));

            if(check != screen.end()) {
                if(check->second == BALL) {
                    c = 'O';
                }
                else if(check->second == WALL) {
                    c = 'W';
                }
                else if(check->second == PADDLE) {
                    c = '=';
                }
                else if(check->second == BLOCK) {
                    c = '#';
                }
            }
            printf("%c", c);
        }
        printf("\n");
    }

    mem[0] = 2;
    puter.load(mem);
    puter.reset();

    screen.clear();
    int score = 0;
    while(!puter.is_done()) {
        puter.run_to_done();
        output = puter.release_output();
        iter = output.begin();
        while(iter != output.end()) {
            int x = *iter;
            iter++;
            int y = *iter;
            iter++;
            int id = *iter;
            iter++;

            screen[Point(x,y)] = id;
        }
        // Draw the board;
        Point ball;
        Point paddle;
        system("clear");
        for(int y=0; y<max_y; y++) {
            for(int x=0; x<max_x; x++) {
                char c = ' ';
                auto check = screen.find(Point(x, y));

                if(check != screen.end()) {
                    if(check->second == BALL) {
                        ball = Point(x, y);
                        c = 'O';
                    }
                    else if(check->second == WALL) {
                        c = 'W';
                    }
                    else if(check->second == PADDLE) {
                        paddle = Point(x,y);
                        c = '=';
                    }
                    else if(check->second == BLOCK) {
                        c = '#';
                    }
                }
                printf("%c", c);
            }
            printf("\n");
        }

        printf("Score = %i\n", screen[Point(-1, 0)]);
        int choice = -2;
        usleep(1000);
        if(ball.first < paddle.first) {
            choice = -1;
        }
        else if(ball.first > paddle.first) {
            choice = 1;
        }
        else {
            choice = 0;
        }
        puter.write(choice);
    }


    printf("Score = %i\n", screen[Point(-1, 0)]);
}

