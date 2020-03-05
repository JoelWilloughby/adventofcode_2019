#include "computer.h"
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <regex>
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

const string items[] = {
        "spool of cat6",
        "fuel cell",
        "jam",
        "loom",
        "mug",
        "prime number",
        "food ration",
        "manifold",
};

const char gather_all[] =
        "east\n"
        "east\n"
        "east\n"
        "north\n"
        "north\n"
        "take fuel cell\n"
        "south\n"
        "east\n"
        "take spool of cat6\n"
        "west\n"
        "south\n"
        "east\n"
        "take jam\n"
        "west\n"
        "west\n"
        "take manifold\n"
        "west\n"
        "south\n"
        "take prime number\n"
        "north\n"
        "take food ration\n"
        "west\n"
        "north\n"
        "north\n"
        "west\n"
        "take mug\n"
        "east\n"
        "north\n"
        "east\n"
        "east\n"
        "take loom\n"
        "west\n"
        "west\n"
        "south\n"
        "south\n"
        "west\n"
        "north\n"
        "west\n"
        "inv\n";

string next_string(uint32_t count) {
    ostringstream stream;
    for(int i=0; i<8; i++) {
        if(count & (1 << i)) {
            stream << "take ";
        }
        else {
            stream << "drop ";
        }
        stream << items[i] << "\n";
    }

    stream << "inv\n";
    stream << "north\n";

    return stream.str();
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

    for(int i=0; i<strlen(gather_all); i++) {
        puter.write(gather_all[i]);
    }

    bool done = false;
    uint32_t count = 0;
    set<string> seen;
    for(unsigned count=0; count < 256; count++) {
        string s = next_string(count);
        for(int i=0; i<s.size(); i++) {
            puter.write(s[i]);
        }

        ostringstream os;
        puter.run_to_done();
        auto output = puter.release_output();
        for(int i=0; i<output.size(); i++) {
            if(output[i] < 255) {
                os << static_cast<char>(output[i]);
                printf("%c", static_cast<char>(output[i]));
            }
        }

        string out = os.str();
        regex reg("== [a-zA-Z0-9 \\-]+ ==");
        regex alert_re("Alert! Droids on this ship are .+ than the detected value!");
        auto places_begin = sregex_iterator(out.begin(), out.end(), reg);
        for(sregex_iterator iter = places_begin; iter != sregex_iterator(); iter++) {
            seen.insert(iter->str());
        }

//        for(auto place : seen) {
//            printf("Saw: %s\n", place.c_str());
//        }
        printf("%li\n", seen.size());

        printf("Just tried 0x%02x\n", count);
        printf("%s", s.c_str());
        smatch re_match;
        if(!regex_search(out, re_match, alert_re)) {
            return 1;
        }
        else {
            printf("%s\n", re_match.str().c_str());
        }

//        char buf[256];
//        int i = 0;
//        while((buf[i++] = fgetc(stdin)) != '\n') {
//            ;
//        }
//        for(int ii=0; ii<i; ii++) {
//            puter.write(buf[ii]);
//        }
    }
//    for(int i=0; i<output.size(); i++) {
//        if(output[i] < 255) {
//            printf("%c", static_cast<char>(output[i]));
//        }
//    }
//
//    printf("Output: %li\n", output[output.size()-1]);
}

