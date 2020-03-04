#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <set>

using namespace std;

void print_it(uint8_t * p) {
    printf("-----\n");
    for(int i=0; i<5; i++) {
        uint8_t c = *(p+i);
        for(int j=5; j>0; j--) {
            printf("%c", (c >> j) & 1 ? '#' : '.');
        }
        printf("\n");
    }
}

#define ith(x, i) ((x >> i) & 1)

void grow(uint8_t * p) {
    uint8_t t[7] = {0};
    for(int i=0; i<5; i++) {
        t[i+1] = *(p+i);
        p[i] = 0;
    }

    for(int i=1; i<6; i++) {
        for(int j=5; j>0; j--) {
            int sum = ith(t[i-1], j);
            sum += ith(t[i+1], j);
            sum += ith(t[i], j+1);
            sum += ith(t[i], j-1);

            if(ith(t[i], j)) {
                p[i-1] |= sum == 1 ? 1 : 0;
            }
            else {
                p[i-1] |= (sum && sum <= 2) ? 1 : 0;
            }

            p[i-1] <<= 1;
        }
    }
}

struct Bugs {
    Bugs() {
        for(int i=0; i<7; i++) {
            for(int j=0; j<7; j++) {
                rows_[i][j] = false;
            }
        }
        above = NULL;
        below = NULL;
    }

    Bugs(const vector<vector<bool>>& bugs) {
        for(int i=0; i<7; i++) {
            for(int j=0; j<7; j++) {
                rows_[i][j] = false;
            }
        }

        for(int i=0; i<5; i++) {
            for(int j=0; j<5; j++) {
                set(i, j, bugs[i][j]);
            }
        }

        above = new Bugs();
        below = new Bugs();
        below->above = this;
        above->below = this;
    }

    void print(int depth=0) {
        if(depth <= 0 && below != NULL) {
            below->print(depth - 1);
        }

        printf("- %i -\n", depth);
        for(int i=0; i<5; i++) {
            for(int j=0; j<5; j++) {
                printf("%c", get(i, j) ? '#' : '.');
            }
            printf("\n");
        }

        if(depth >= 0 && above != NULL) {
            above->print(depth + 1);
        }

        if(depth == 0) {
            printf("\n");
        }
    }

    void grow(int dir=0) {
        // Find your new thing
        bool has_some = false;
        bool temp[5][5] = {0};
        for(int i=0; i<5; i++) {
            for(int j=0; j<5; j++) {
                if(i == 2 && j == 2) {
                    continue;
                }
                int temp_sum = normal_sum(i, j);
                bool is_bug = get(i, j);
                if(is_bug) {
                    temp[i][j] = temp_sum == 1;
                }
                else {
                    temp[i][j] = (temp_sum > 0) && (temp_sum <= 2);
                }
                if(temp[i][j]) {
                    has_some = true;
                }
            }
        }

        if(dir >= 0) {
            if(above == NULL) {
                if(has_some) {
                    // Halt recursion
                    above = new Bugs();
                    above->below = this;
                }
            }
            else {
                above->grow(1);
            }
        }
        if(dir <= 0) {
            if(below == NULL) {
                if(has_some) {
                    below = new Bugs();
                    below->above = this;
                }
            }
            else {
                below->grow(-1);
            }
        }

        for(int i=0; i<5; i++) {
            for(int j=0; j<5; j++) {
                set(i, j, temp[i][j]);
            }
        }
    }

    int bug_sum(int depth=0) {
        int sum = 0;
        for(int i=0; i<5; i++) {
            for(int j=0; j<5; j++) {
                sum += get(i,j);
            }
        }
        if(depth >= 0 && above != NULL) {
            sum += above->bug_sum(depth+1);
        }
        if(depth <= 0 && below != NULL) {
            sum += below->bug_sum(depth-1);
        }

        return sum;
    }

    int normal_sum(int row, int col) {
        if(row == 2 && col == 2) {
            return 0;
        }
        int sum = get(row-1, col);
        sum += get(row+1, col);
        sum += get(row, col-1);
        sum += get(row, col+1);
        if(above != NULL) {
            sum += above->above_sum(row, col);
        }
        if(below != NULL) {
            sum += below->below_sum(row, col);
        }
        return sum;
    }

    int above_sum(int row, int col) {
        // Return the above sum
        int sum = 0;
        if(row == 0) {
            sum += get(1, 2);
        }
        if(col == 0) {
            sum += get(2, 1);
        }
        if(row == 4) {
            sum += get(3, 2);
        }
        if(col == 4) {
            sum += get(2, 3);
        }
        return sum;
    }

    int below_sum(int row, int col) {
        int sum = 0;
        if(row == 1 && col == 2) {
            for(int i=0; i<5; i++) {
                sum += get(0, i);
            }
        }
        else if(row == 2 && col == 1) {
            for(int i=0; i<5; i++) {
                sum += get(i, 0);
            }
        }
        else if(row == 2 && col == 3) {
            for(int i=0; i<5; i++) {
                sum += get(i, 4);
            }
        }
        else if(row == 3 && col == 2) {
            for(int i=0; i<5; i++) {
                sum += get(4, i);
            }
        }
        return sum;
    }

    int get(int i, int j) {
        if(i == 2 && j == 2) {
            return 0;
        }
        return rows_[i+1][j+1] ? 1 : 0;
    }

    void set(int i, int j, bool val) {
        rows_[i+1][j+1] = val;
    }

    bool rows_[7][7];
    Bugs * above;
    Bugs * below;
};

int main(int argc, char ** argv) {
    FILE * file = fopen(argv[1], "r");

    char c;
    vector<vector<bool>> bugs;
    vector<bool> working;
    while((c = fgetc(file)) != EOF) {
        if(c == '.') {
            working.push_back(false);
        }
        else if(c == '#') {
            working.push_back(true);
        }
        else if(c == '\n') {
            if(working.size() != 5) {
                printf("Invalid line %li\n", working.size());
            }
            else {
                bugs.push_back(working);
            }
            working.clear();
        }
        else {
            printf("Unknown char...\n");
            exit(1);
        }
    }

    if(bugs.size() != 5) {
        printf("Invalid number of lines");
        exit(1);
    }

    uint64_t board_ = 0;
    uint8_t * board = reinterpret_cast<uint8_t*>(&board_) + 1;
    for(int i=0; i<bugs.size(); i++) {
        auto row = bugs[i];
        for(int j=0; j<row.size(); j++) {
            auto bug = row[row.size() - j - 1];
            printf("%c", bug ? '1' : '0');
            board[i] |= (bug ? 1 : 0);
            board[i] <<= 1;
        }
//        board[i] >>= 1;
        printf("  %02x\n", board[i]);
    }
    uint64_t board_2_ = board_;

    set<long long> states;
    while(states.find(board_) == states.end()) {
        states.insert(board_);
        print_it(board);
        grow(board);
    }

    print_it(board);

    uint64_t sum = 0;
    for(int i=4; i>=0; i--) {
        uint8_t temp = board[i];
        sum |= (temp >> 1);
        sum <<= 5;
    }
    sum >>= 5;
    printf("Board value: %llu\n", sum);


    // Part 2 start

    Bugs the_bugs(bugs);
    the_bugs.print();

    for(int i=0; i<200; i++) {
        the_bugs.grow();
        the_bugs.print();
        printf("Num bugs after %i cycles: %i\n", i+1, the_bugs.bug_sum());
    }
}