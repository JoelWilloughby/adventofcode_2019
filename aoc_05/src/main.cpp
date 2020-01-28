#include <cstring>
#include <cstdio>
#include <vector>

class Computer {
public:
    Computer();

    void init(std::vector<int>& mem, int noun, int verb);
    void init(std::vector<int>& mem);
    bool step();
    bool is_done();
    void run_to_done();
    void run_to_done(std::vector<int>& input);

    int value_at(int index);
    std::vector<int> get_output();
private:
    std::vector<int> memory;
    std::vector<int> input;
    std::vector<int> output;
    int input_idx;
    int pc;
};

Computer::Computer(): pc(0), memory(), input(), output() {

}

void Computer::init(std::vector<int>& mem) {
    this->memory.clear();
    for(auto iter = mem.begin(); iter != mem.end(); iter++) {
        this->memory.push_back(*iter);
    }

    this->input.clear();
}

void Computer::init(std::vector<int>& mem, int noun, int verb) {
    this->memory[1] = noun;
    this->memory[2] = verb;

    this->init(mem);
}

bool Computer::is_done() {
    return this->memory[pc] == 99;
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
                printf("Need input\n");
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
                printf("Jump to %i %i\n", a2i, memory[a2i]);
                pc = memory[a2i];
            }
            else {
                pc += 3;
            }
            break;
        case 6:
            if(memory[a1i] == 0) {
                printf("Jump to %i %i\n", a2i, memory[a2i]);
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
    pc = 0;
    input_idx = 0;
    while(1) {
        while(!this->step());

        if(is_done()) {
            break;
        }

        int i;
        printf("Input: ");
        scanf("%i", &i);
        input.push_back(i);
    }
}

void Computer::run_to_done(std::vector<int>& input) {
    this->input = input;
    this->run_to_done();
}

std::vector<int> Computer::get_output() {
    return output;
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

    std::vector<int> input;
//    input.push_back(8);
    Computer puter;
    puter.init(mem);
    puter.run_to_done(input);

    std::vector<int> output = puter.get_output();
    for(int i=0; i<output.size(); i++) {
        printf("%i ", output[i]);
    }

    printf("\n");
}


