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
struct Node {
    Node(): visited(false), depth(0) {}

    int depth;
    bool visited;
};

void do_part_1(vector<vector<char>>& scaffold) {
    vector<Point> points;
    for(int i=0; i<scaffold.size(); i++) {
        vector<char>& row = scaffold[i];
        for(int j=0; j<row.size(); j++) {
            if(i > 0 && i < scaffold.size()-1 && j > 0 && j < row.size()-1 && row[j] == '#') {
                if(scaffold[i-1][j] == '#' && scaffold[i+1][j] == '#' && scaffold[i][j+1] == '#' && scaffold[i][j+1] == '#') {
                    points.push_back(Point(i, j));
                    printf("O");
                }
                else {
                    printf("#");
                }
            }
            else {
                printf("%c", row[j]);
            }
        }
        printf("\n");
    }

    uint32_t total = 0;
    for(auto p : points) {
        total += p.first * p.second;
    }
    printf("Total found: %li\n", total);
}

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
    printf("Nums read: %i\n", mem.size());

    Computer puter;

    puter.load(mem);
    puter.reset();

    puter.run_to_done();
    vector<el_type> output = puter.release_output();
    unsigned original_output_size = output.size();
    vector<vector<char>> scaffold;
    vector<char> working;
    for(auto val : output) {
        printf("%c", (char)val);

        if(val == '\n') {
            if(!working.empty()) {
                scaffold.push_back(working);
                working.clear();
            }
        }
        else {
            working.push_back((char)val);
        }
    }
    do_part_1(scaffold);

    // Put it in the right mode
    mem[0] = 2;
    puter.load(mem);
    puter.reset();

    // Do a lot of thinking with a pen and paper...
    char sequence_a[] = "L,12,R,8,L,6,R,8,L,6";
    char seqeunce_b[] = "R,8,L,12,L,12,R,8";
    char sequence_c[] = "L,6,R,6,L,12";
    char meta_sequence[] = "A,B,A,A,B,C,B,C,C,B";
    char choice[] = "n";

    vector<el_type> input;
    load_input(input, meta_sequence);
    load_input(input, sequence_a);
    load_input(input, seqeunce_b);
    load_input(input, sequence_c);
    load_input(input, choice);

    puter.run_to_done(input);
    output = puter.release_output();

    for(int i=0; i<output.size(); i++) {
        if(output[i] < 255) {
            printf("%c", output[i]);
        }
    }

    printf("Output: %li\n", output[output.size()-1]);
}

