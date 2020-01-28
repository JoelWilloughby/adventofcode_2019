#include <cstring>
#include <cstdio>
#include <vector>
#include <sys/poll.h>

class Computer {
public:
    Computer();

    void load(std::vector<int> &mem);
    void reset();
    bool step();
    bool is_done();
    bool needs_input();
    void run_to_done();
    void run_to_done(std::vector<int>& input);

    void write(int input);

    int value_at(int index);
    std::vector<int> get_output();
    std::vector<int> release_output();
private:
    std::vector<int> memory;
    std::vector<int> rom;
    std::vector<int> input;
    std::vector<int> output;
    int input_idx;
    int pc;
};

Computer::Computer(): pc(0), memory(), input(), output() {

}

void Computer::load(std::vector<int> &mem) {
    this->rom = mem;
}

void Computer::reset() {
    this->memory = this->rom;
    this->input.clear();
    this->input_idx = 0;
    this->output.clear();
    this->pc = 0;
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

int Computer::value_at(int index) {
    return memory[index];
}

bool Computer::step() {
    int op = memory[pc];
    int opcode = op % 100;
    op /= 100;
    int a1i = op % 10 == 1 ? pc+1 : memory[pc+1];
    op /= 10;
    int a2i = op % 10 == 1 ? pc+2 : memory[pc+2];
    op /= 10;
    int a3i = op % 10 == 1 ? pc+3 : memory[pc+3];
    int op_size;
    switch(opcode) {
        case 99:
            return true;
        case 1:
            memory[a3i] = memory[a1i] + memory[a2i];
            pc += 4;
            break;
        case 2:
            memory[a3i] = memory[a1i] * memory[a2i];
            pc += 4;
            break;
        case 3:
            // input
            if(input_idx >= input.size()) {
                return true;
            }
            memory[a1i] = input[input_idx++];
            pc += 2;
            break;
        case 4:
            // output
            output.push_back(memory[a1i]);
            pc += 2;
            break;
        case 5:
            if(memory[a1i] != 0) {
                pc = memory[a2i];
            }
            else {
                pc += 3;
            }
            break;
        case 6:
            if(memory[a1i] == 0) {
                pc = memory[a2i];
            }
            else {
                pc += 3;
            }
            break;
        case 7:
            memory[a3i] = memory[a1i] < memory[a2i] ? 1 : 0;
            pc += 4;
            break;
        case 8:
            memory[a3i] = memory[a1i] == memory[a2i] ? 1 : 0;
            pc += 4;
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
        while(!this->step());
    }
}

void Computer::run_to_done(std::vector<int>& input) {
    for(int i=0; i<input.size(); i++) {
        this->input.push_back(input[i]);
    }
    this->run_to_done();
}

std::vector<int> Computer::get_output() {
    return output;
}

std::vector<int> Computer::release_output() {
    std::vector<int> temp = get_output();
    output.clear();
    return temp;
}

bool stdin_good() {
    struct pollfd fds;
    int ret;
    fds.fd = 0; /* this is STDIN */
    fds.events = POLLIN;
    ret = poll(&fds, 1, 0);
    if(ret == 1)
        return true;

    return false;
}

int main(int argc, char** argv) {
    FILE* file = fopen(argv[1], "r");

    int temp;
    std::vector<int> mem;
    fscanf(file, "%i", &temp);
    while(fscanf(file, ",") == 0) {
        mem.push_back(temp);
        fscanf(file, "%i", &temp);
    }
    mem.push_back(temp);
    printf("Nums read: %i\n", mem.size());

    Computer puter;
    puter.load(mem);

    Computer puters[5];
    for(int i=0; i<5; i++) {
        puters[i].load(mem);
    }

    int max = 0;
    int amp_max = 0;
    bool done = false;
    while(!done && stdin_good()){
        int perm[5];
        for(int i=0; i<5; i++) {
            if(scanf("%i", &perm[i]) != 1) {
                done = true;
                break;
            }
        }

        printf("Sequence: ");
        for(int i=0; i<5; i++) printf("%i ", perm[i]);
        printf("\n");

        if(done) {
            break;
        }

        int in = 0;
        for(int i=0; i<5; i++) {
            std::vector<int> input;
            puter.reset();
            input.push_back(perm[i]);
            input.push_back(in);
            puter.run_to_done(input);

            in = puter.get_output()[puter.get_output().size() - 1];
        }

        if (in > max) {
            max = in;
        }

        for(int i=0; i<5; i++) {
            puters[i].reset();
            puters[i].write(perm[i]+5);
        }
        in = 0;
        int count =0;
        while(!puters[0].is_done()) {
            count++;
            for(int i=0; i<5; i++) {
                if(puters[i].is_done()) {
                    printf("Early done: %i\n", i);
                }
                puters[i].write(in);
                puters[i].run_to_done();
                std::vector<int> last_out = puters[i].release_output();
                in = last_out[last_out.size() - 1];
            }
        }

        if(in > amp_max) {
            printf("New amp max found: %i\n", in);
            amp_max = in;
        }

    }
    printf("Max: %i Amp max: %i\n", max, amp_max);
}


