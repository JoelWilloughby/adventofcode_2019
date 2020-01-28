#include <cstdio>
#include <cstdint>

bool meets_thing(uint32_t n) {
    bool double_found = false;
    uint8_t curr_digit_count = 1;
    uint8_t last_digit = n % 10;
    n /= 10;
    for(int i=0; i<5; i++) {
        uint8_t digit = n % 10;
        if(digit > last_digit) {
            return false;
        }
        if(digit == last_digit) {
            curr_digit_count++;
        }
        else {
            if(curr_digit_count == 2) {
                double_found = true;
            }
            curr_digit_count = 1;
        }
        last_digit = digit;

        n /= 10;
    }
    if(curr_digit_count == 2) {
        return true;
    }

    return double_found;
}


int main(int argc, char ** argv) {
    FILE * file = fopen(argv[1], "r");
    uint32_t min, max;
    fscanf(file, "%i-%i", &min, &max);

    uint32_t count = 0;

    for(int i=min; i<max; i++) {
        if(meets_thing(i)) count++;
    }

    printf("Count is: %i\n", count);
    return 0;
}
