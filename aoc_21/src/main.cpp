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
void load_input(vector<el_type>& input, const char * str) {
    int count = 0;
    while(*str != '\0') {
        count++;
        input.push_back((el_type)*str);
        str++;
    }

    if(count > 20) {
        printf("More than 20 chars on a line\n");
        exit(1);
    }

    input.push_back((el_type)'\n');
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
    printf("Nums read: %li\n", mem.size());

    Computer puter;

    // Put it in the right mode
    puter.load(mem);
    puter.reset();

    vector<el_type> input;
    FILE * in = fopen(argv[2], "r");
    char c;
    bool commented = false;
    while((c = fgetc(in)) != EOF) {
        if(c == '#') {
            commented = true;
            continue;
        }
        if(commented && c != '\n') {
            continue;
        }
        commented = false;
        input.push_back(c);
    }

    puter.run_to_done(input);
    auto output = puter.release_output();

    for(int i=0; i<output.size(); i++) {
        if(output[i] < 255) {
            printf("%c", static_cast<char>(output[i]));
        }
    }

    printf("Output: %li\n", output[output.size()-1]);
}

