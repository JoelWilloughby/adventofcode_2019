#include <stdio.h>

unsigned fuel_sum(unsigned n) {
    int val = n / 3;
    val -= 2;

    if(val <= 0) {
        return 0;
    }

    return val + fuel_sum(val);
}

int main() {
    FILE * f = fopen("input.txt", "r");

    unsigned val;
    unsigned part2 = 0;
    unsigned sum = 0;
    while(EOF != fscanf(f, "%i", &val)) {
        unsigned temp = val/3 - 2;
        sum += temp;
        part2 += fuel_sum(val);
    }


    printf("Sum is %i\n", sum);
    printf("Part 2 Sum is %i\n", part2);

    return 0;
}
