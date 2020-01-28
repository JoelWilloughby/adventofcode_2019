#ifndef AOC_11_COMPUTER_H
#define AOC_11_COMPUTER_H
#include <vector>

typedef long el_type;

class Computer {
public:
    Computer();

    void load(std::vector<el_type> &mem);
    void reset();
    bool step();
    bool is_done();
    bool needs_input();
    void run_to_done();
    void run_to_done(std::vector<el_type>& input);

    void write(int input);

    el_type value_at(int index);
    std::vector<el_type> get_output();
    std::vector<el_type> release_output();

    void print_it();
private:
    int get_arg_idx(int, int);
    void ensure(int idx);

    std::vector<el_type> memory;
    std::vector<el_type> rom;
    std::vector<el_type> input;
    std::vector<el_type> output;
    int input_idx;
    int rel_idx;
    int pc;
};

#endif //AOC_11_COMPUTER_H
