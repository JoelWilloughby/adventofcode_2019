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

    // Load up the computers with something
    for(auto i=0; i<NUM_PUTERS; i++) {
        puters[i].load(mem);
        puters[i].reset();
        // Give each puter its id
        puters[i].write(i);
    }

    bool done = false;
    while(!done) {
        // Loop through each computer, running it until it outputs stuff or waits for input
        for(auto i=0; i<NUM_PUTERS; i++) {
            if(puters[i].needs_input()) {
                if(inputs[i].size() < 2) {
                    // Provide it with no input
                    puters[i].write(-1);
                }
            }
            puters[i].step();
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
                    auto dest = inputs[i][0];
                    auto x = inputs[i][1];
                    auto y = inputs[i][2];
                    printf("%i -> %i: (%i, %i)\n", i, dest, x, y);
                    if(dest == 255) {
                        // The value we are looking for:
                        printf("Y value sent to 255 is: %i\n", temp);
                        done = true;
                        break;
                    }
                    puters[dest].write(x);
                    puters[dest].write(y);
                    inputs[i].clear();
                }
           }
        }
    }
}

