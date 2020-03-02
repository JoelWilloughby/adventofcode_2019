#include "computer.h"

#include <cstdio>

Computer::Computer(): pc(0), memory(), input(), output() {

}

void Computer::load(std::vector<el_type> &mem) {
    rom = mem;
}

void Computer::reset() {
    this->memory = this->rom;
    this->input.clear();
    this->input_idx = 0;
    this->output.clear();
    this->pc = 0;
    this->rel_idx = 0;
}

void Computer::write(int input) {
    this->input.push_back(input);
}

bool Computer::is_done() {
    return this->memory[pc] == 99;
}

bool Computer::needs_input() {
    return this->memory[pc] == 3 && input_idx >= input.size();
}

el_type Computer::value_at(int index) {
    return memory[index];
}

int Computer::get_arg_idx(int op, int idx) {
    op /= 100;
    for(int i=0; i<idx-1; i++) {
        op /= 10;
    }

    if(op % 10 == 0) {
        ensure(pc+idx);
        return memory[pc+idx];
    }
    if(op % 10 == 1) {
        return pc+idx;
    }
    if(op % 10 == 2) {
        ensure(pc+idx);
        return memory[pc + idx] + rel_idx;
    }

    return -1;
}

void Computer::ensure(int idx) {
    for(int i=memory.size(); i<=idx; i++) {
        memory.push_back(0);
    }
}

bool Computer::step() {
    int op = memory[pc];
    int opcode = op % 100;
    int a1i = get_arg_idx(op, 1);
    int a2i = get_arg_idx(op, 2);
    int a3i = get_arg_idx(op, 3);
    switch(opcode) {
        case 99:
            return true;
        case 1:
            // arg3 <- arg2 + arg1
            ensure(a1i); ensure(a2i); ensure(a3i);
            memory[a3i] = memory[a1i] + memory[a2i];
            pc += 4;
            break;
        case 2:
            // arg3 <- arg2 * arg1
            ensure(a1i); ensure(a2i); ensure(a3i);
            memory[a3i] = memory[a1i] * memory[a2i];
            pc += 4;
            break;
        case 3:
            // arg1 <- in
            ensure(a1i);
            if(input_idx >= input.size()) {
                return true;
            }
            memory[a1i] = input[input_idx++];
            pc += 2;
            break;
        case 4:
            // arg1 -> out
            ensure(a1i);
            output.push_back(memory[a1i]);
            pc += 2;
            break;
        case 5:
            // JEQ0 arg1 arg2
            ensure(a1i); ensure(a2i);
            if(memory[a1i] != 0) {
                pc = memory[a2i];
            }
            else {
                pc += 3;
            }
            break;
        case 6:
            // JNEQ0 arg1 arg2
            ensure(a1i); ensure(a2i);
            if(memory[a1i] == 0) {
                pc = memory[a2i];
            }
            else {
                pc += 3;
            }
            break;
        case 7:
            // arg3 <- arg1 < arg2
            ensure(a1i); ensure(a2i); ensure(a3i);
            memory[a3i] = memory[a1i] < memory[a2i] ? 1 : 0;
            pc += 4;
            break;
        case 8:
            // arg3 <- arg1 == arg2
            ensure(a1i); ensure(a2i); ensure(a3i);
            memory[a3i] = memory[a1i] == memory[a2i] ? 1 : 0;
            pc += 4;
            break;
        case 9:
            // rel_off += arg1
            ensure(a1i);
            rel_idx += memory[a1i];
            pc += 2;
            break;
        default:
            printf("Unknown opcode %i detected at pc: %i\n", opcode, pc);
            memory[pc] = 99;
            return true;
    }

    return false;
}

void Computer::run_to_done() {
    while(!is_done() && !needs_input()) {
        int count = 0;
        while(!this->step()) {
            if(count++ == 1000) {
//                print_it();
                count = 0;
            }
        }
    }
}

void Computer::run_to_done(std::vector<el_type>& input) {
    for(int i=0; i<input.size(); i++) {
        this->input.push_back(input[i]);
    }
    this->run_to_done();
}

const std::vector<el_type>& Computer::get_output() {
    return output;
}

std::vector<el_type> Computer::release_output() {
    std::vector<el_type> temp = get_output();
    output.clear();
    return temp;
}

void Computer::print_it() {
    printf("PC: %6i, REL_OFF: %6i\n", pc, rel_idx);
    for(int i=0; i<memory.size(); i++) {
        printf("%li ", memory[i]);
        if(i % 20 == 19) {
            printf("\n");
        }
    }

    printf("\n");
}
