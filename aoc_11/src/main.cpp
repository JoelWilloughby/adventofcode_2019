#include "computer.h"
#include <cstring>
#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include <sys/poll.h>

#define BLACK 0
#define WHITE 1

struct Point {
    Point(int x=0, int y=0): x(x), y(y) {

    }

    bool operator<(const Point& other) const{
        if(x < other.x) {
            return true;
        }
        if(x > other.x) {
            return false;
        }
        if(y < other.y) {
            return true;
        }
        if(y > other.y) {
            return false;
        }
        return false;
    }

    int x;
    int y;
};

struct Square {
    Square(int x=0, int y=0) : loc(x,y), painted(false), color(BLACK) {
    }

    Point loc;
    bool painted;
    int color;
};


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

    std::map<Point, Square> board;
    Point current(0,0);
    board[current] = Square(0,0);
    board[current].color = WHITE;
    int facing = 0;
    while(!puter.is_done()) {
        Square& curr = board.at(current);
        puter.write(curr.color);
        puter.run_to_done();

        std::vector<el_type> out = puter.release_output();

        curr.painted = true;
        curr.color = out[0];
        int dir = out[1];
        facing += dir==1 ? 1 : 3;
        facing = facing % 4;

        if(facing == 0) {
            current.y++;
        }
        else if(facing == 1) {
            current.x++;
        }
        else if(facing == 2) {
            current.y--;
        }
        else if(facing == 3) {
            current.x--;
        }
        else {
            printf("Bad facing: %i\n", facing);
            exit(1);
        }

        if(board.find(current) == board.end()) {
            board[current] = Square(current.x, current.y);
        }
    }

    unsigned count = 0;
    for(auto iter = board.begin(); iter != board.end(); iter++) {
        if(iter->second.painted) {
            count++;
        }
    }

    // Parameters found by twiddling some knobs
    for(int y=9; y>=0; y--) {
        for(int x=0; x<44; x++) {
            auto iter = board.find(Point(x-1, y-7));
            int color = BLACK;
            if(iter != board.end()) {
                color = iter->second.color;
            }

            if(color == BLACK) {
                printf(".");
            }
            else {
                printf("#");
            }
        }

        printf("\n");
    }


    printf("Num painted: %i\n", count);

}

