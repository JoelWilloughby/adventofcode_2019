#include <cstdio>
#include <cctype>
#include <cstdint>
#include <cmath>

#include <vector>

using namespace std;

vector<uint8_t> phase(const vector<uint8_t>& in) {
    vector<uint8_t> temp;
    temp.reserve(in.size());
    for (size_t i = 0; i < in.size(); i++) {
        // Build up phase i
        int val = 0;
        for (size_t j = 0; j < in.size(); j++) {
            int t = (j + 1) / (i + 1);
            t = t % 4;
            if (t == 1) {
                val += in[j];
            } else if (t == 3) {
                val -= in[j];
            }
        }
        val = abs(val);
        val %= 10;
        temp.push_back(val);
    }

    return temp;
}

int main(int argc, char ** argv) {
    FILE * file = fopen(argv[1], "r");

    vector<uint8_t> nums;
    char c;
    while((c = fgetc(file)) != EOF) {
        if(!isdigit(c)) {
            continue;
        }
        nums.push_back(c - '0');
    }

    vector<uint8_t> stuff;
    stuff.reserve(10000 * nums.size());

    for(int i=0; i<10000; i++) {
        stuff.insert(stuff.end(), nums.begin(), nums.end());
    }


    int temp = 0;
    for(int i=0; i<7; i++) {
        temp *= 10;
        temp += nums[i];
    }

    if(temp < stuff.size() / 2) {
        printf("Message index is over half\n");
        exit(1);
    }

    for(int i=0; i<100; i++) {
        nums = phase(nums);
        printf("Phase %i\n", i+1);
        for(auto num : nums) {
            printf("%i", num);
        }

        printf("\n");


        int count = 0;
        for(int j=stuff.size() - 1; j>=temp - 1; j--) {
            count += stuff[j];
            count = count % 10;
            stuff[j] = count;
        }
    }

    printf("Message: ");
    for(int i=0; i<8; i++) {
        // compute the nums
        printf("%i", stuff[i + temp]);
    }

    printf("\n");

    printf("Nums size: %li\n", nums.size());
}