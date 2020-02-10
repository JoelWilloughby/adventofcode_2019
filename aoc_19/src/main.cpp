#include "computer.h"
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <sys/poll.h>

using namespace std;

typedef pair<int, int> Point;

void do_part_1(Computer& puter) {
    bool field[50][50];
    for(uint32_t i=0; i<50; i++) {
        for(uint32_t j=0; j<50; j++) {
            puter.reset();
            puter.write(j);
            puter.write(i);
            puter.run_to_done();
            vector<el_type> output = puter.release_output();
            if(output.size() != 1) {
                printf("Bad output size %li\n", output.size());
                exit(1);
            }
            if(output[0]) {
                field[i][j] = true;
            }
            else {
                field[i][j] = false;
            }
        }
    }

    int count = 0;
    for(int i=0; i<50; i++) {
        for(int j=0; j<50; j++) {
            if(field[i][j]) {
                printf("#");
                count += 1;
            }
            else {
                printf(".");
            }
        }
        printf("\n");
    }

    printf("Num is %i\n", count);
}

bool query(Computer& puter, uint32_t x, uint32_t y) {
    puter.reset();
    puter.write(x);
    puter.write(y);

    puter.run_to_done();
    vector<el_type> output = puter.release_output();
    if(output.size() != 1) {
        printf("Bad output size: %i\n", output.size());
    }

    return output[0] == 1;

//    int min_x = y - y/5;
//    int max_x = y - y/9;
//    return x >= min_x && x < max_x;
}

int find_min(Computer& puter, uint32_t y, uint32_t guess) {
    // Find the start if there is one
    uint32_t x = guess;
    // Linear scan to find front
    while(x < y) {
        if(query(puter, x, y)) {
            return x;
        }
        x++;
    }

    return -1;
}

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

    do_part_1(puter);

    const int SANTA_SIZE = 100;
    int guess_y = SANTA_SIZE + 1;

    bool found = false;
    uint32_t min_x = 0;
    while(!found) {
        printf("Checking y=%i, min_x=%i\n", guess_y, min_x);
        int temp = find_min(puter, guess_y, min_x);
        if(temp > 0) {
            if(query(puter, temp + SANTA_SIZE - 1, guess_y - SANTA_SIZE + 1)) {
                printf("Found it: %i %i", temp, guess_y - SANTA_SIZE + 1);
                found = true;
            }
            min_x = temp;
        }
        else {
            printf("Min not found\n");
        }
        guess_y++;
    }

}

