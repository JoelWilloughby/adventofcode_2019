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

int main(int argc, char** argv) {
    FILE *file = fopen(argv[1], "r");

    el_type temp;
    std::vector<el_type> mem;
    fscanf(file, "%li", &temp);
    mem.push_back(temp);
    while (fscanf(file, ",%li", &temp) == 1) {
        mem.push_back(temp);
    }
    printf("Nums read: %li\n", mem.size());

    const size_t NUM_PUTERS = 50;
    Computer puters[NUM_PUTERS];
    vector<el_type> inputs[NUM_PUTERS];
    el_type nat_x;
    el_type nat_y;
    map<el_type, bool> nat_ys;

    int empty_and_waiting[NUM_PUTERS];

    // Load up the computers with something
    for(auto i=0; i<NUM_PUTERS; i++) {
        puters[i].load(mem);
        puters[i].reset();
        // Give each puter its id
        printf("Sending %i\n", i);
        puters[i].write(i);
        empty_and_waiting[i] = 0;
    }

    bool done = false;
    while(!done) {
//        printf("loop\n");
        for(auto i=0; i<NUM_PUTERS; i++) {
            if(puters[i].needs_input()) {
                // Provide it with no input
                puters[i].write(-1);
                empty_and_waiting[i]++;
            }
            puters[i].step();
        }

        bool waiting = true;
        for(int i=0; i<NUM_PUTERS; i++) {
            if(empty_and_waiting[i] < 10) {
                waiting = false;
                break;
            }
        }

        if(waiting) {
            puters[0].write(nat_x);
            puters[0].write(nat_y);
            empty_and_waiting[0] = 0;
            if(nat_ys.find(nat_y) != nat_ys.end()) {
                printf("Second occurence of %i\n", nat_y);
                done = true;
                return 0;
            }
            printf("NAT sending out %i, %i\n", nat_x, nat_y);
            nat_ys[nat_y] = true;
        }

        // Collect ouput and write it out
        for(auto i=0; i<NUM_PUTERS; i++) {
            if(!puters[i].get_output().empty()) {
                auto output = puters[i].release_output();
                auto temp = output[0];
                if(output.size() != 1) {
                    printf("More than 1 output\n");
                    exit(1);
                }
                inputs[i].push_back(temp);
                if(inputs[i].size() == 3) {
                    el_type dest = inputs[i][0];
                    el_type x = inputs[i][1];
                    el_type y = inputs[i][2];
                    printf("%i -> %i: (%i, %i)\n", i, dest, x, y);
                    if(dest == 255) {
                        // The value we are looking for:
                        printf("Y value sent to 255 is: %i\n", temp);
                        nat_x = x;
                        nat_y = y;
                    }
                    else {
                        puters[dest].write(x);
                        puters[dest].write(y);
                        empty_and_waiting[dest] = 0;
                        inputs[i].clear();
                    }
                }
           }
        }
    }
}

