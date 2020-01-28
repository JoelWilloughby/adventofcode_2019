#include <stdio.h>
#include <string.h>

int decode(unsigned * bigboy, int index) {
    // Return -1 when exited
    // Return next index otherwise

    if(bigboy[index] == 99) {
        return -1;
    }
    
    unsigned a_i = index+1; 
    unsigned a_bb = bigboy[a_i];
    unsigned b_i = index+2; 
    unsigned b_bb = bigboy[b_i];
    unsigned c_i = index+3;
    unsigned c_bb = bigboy[c_i];

    switch(bigboy[index]) {
    case 99: {
        return -1;
    }
    case 1: {
        bigboy[c_bb] = bigboy[a_bb] + bigboy[b_bb];
        return index+4;
    }
    case 2: {
        bigboy[c_bb] = bigboy[a_bb] * bigboy[b_bb];
        return index+4;
    }
    default:
        fprintf(stderr, "Fucked up\n");
    }

    return -1;
}


void run(unsigned * bigboy) {
    int index = 0;
    while(index >= 0) {
        index = decode(bigboy, index);
    }
}

void confickle(unsigned * bigboy, unsigned one, unsigned two) {
    bigboy[1] = one;
    bigboy[2] = two;
}

int main() {
    FILE * f = fopen("input.txt", "r");

    unsigned working[1000];
    unsigned bigboy[1000] = { 0 };

    fscanf(f, "%i", &bigboy[0]);
    unsigned size = 1;
    while(fscanf(f, ",%i", &bigboy[size++]));

    unsigned target = 19690720;
    for(int i=0; i<100; i++) {
        for(int j=0; j<100; j++) {
            memcpy(working, bigboy, 4000);
            confickle(working, i, j);
            run(working);

            if(working[0] == target) {
                printf("i=%i, j=%i, val=%i\n", i, j, 100*i+j);
            }
        }
    }

    return 0;
    
}
