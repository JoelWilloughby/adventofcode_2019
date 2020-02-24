#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unordered_map>

using namespace std;

class Card {
public:
    Card(long long deck_size, long long position, long long value) : deck_size(deck_size), position(position), value(value) {

    }

    long long deck_size;
    long long position;
    long long value;
};

class Action {
public:
    virtual void op_card(Card& card) = 0;
    virtual void op_pos(Card& card) = 0;
    virtual void print() const = 0;
};

class Cut : public Action {
public:
    Cut(int n) : n(n) {

    }

    void op_card(Card& card) {
        card.position += card.deck_size - n;
        card.position %= card.deck_size;
    }

    void op_pos(Card& card) {
        card.position += card.deck_size + n;
        card.position %= card.deck_size;
    }

    void print() const {
        printf("Cut %i\n", n);
    }

    int n;
};

class NewStack : public Action {
public:
    void op_card(Card& card) {
        card.position = card.deck_size - card.position - 1;
    }

    void op_pos(Card& card) {
        card.position = card.deck_size - card.position - 1;
    }

    void print() const {
        printf("deal into new stack\n");
    }
};

class DealInto : public Action {
public:
    DealInto(int n) : n(n), cached_val(-1) {

    }

    void op_card(Card& card) {
        // May have to perform this one by one
        long long temp = n * card.position;
        card.position = temp % card.deck_size;
    }

    void op_pos(Card& card) {
        if(cache.empty() || cached_val != card.deck_size) {
            cache.clear();
            cached_val = card.deck_size;
            for(int i=0; i<n; i++) {
                cache.push_back(0);
            }
            bool done = false;
            long long rem = 0;
            long long count = 0;
            while(!done) {
                count += 1 + (card.deck_size - rem - 1) / n;
                rem = n - ((card.deck_size - rem) % n);
                rem = (rem + n) % n;
                if(rem == 0) {
                    done = true;
                    break;
                }
                if(rem < 0) {
                    printf("Whoops");
                    exit(1);
                }
                cache[rem] = count;
            }
        }

        long long rem = card.position % n;
        long long count = cache[rem];
        card.position = count + (card.position - rem) / n;
    }

    void print() const {
        printf("deal with increment %i\n", n);
    }

    vector<long long> cache;
    long long cached_val;
    int n;
};

int main(int argc, char ** argv) {
    FILE * file = fopen(argv[1], "r");
    char* buf = NULL;
    size_t len = 0;
    vector<Action*> actions;
    while(getline(&buf, &len, file) > 0) {
        int temp;
        if(strcmp(buf, "deal into new stack\n") == 0) {
            actions.push_back(new NewStack());
        }
        else if(sscanf(buf, "deal with increment %i", &temp) > 0) {
            actions.push_back(new DealInto(temp));
        }
        else if(sscanf(buf, "cut %i", &temp) > 0){
            actions.push_back(new Cut(temp));
        }
        else {
            printf("Invalid action: %s\n", buf);
        }
        len = 0;
        free(buf);
        buf = NULL;
    }

    Card card(10007, 2019, 2019);
    for(auto p : actions) {
        p->op_card(card);
    }
    printf("Find 2019 = %lli\n", card.position);

    Card test(10007, 1498, 0);
    for(auto iter = actions.rbegin(); iter != actions.rend(); iter++) {
        (*iter)->op_pos(test);
    }

    printf("Find 2019? %lli\n", test.position);

    long long total_cards = 119315717514047;
    long long total_round = 101741582076661;
    long long max_long_lo = 9223372036854775807;
    long long loop_size = -1;
    long long loop_start = -1;
    unordered_map<long long, long long> cache;
    Card pos(total_cards, 2020, 0);
    for(long long i=0; i<total_round; i++) {
        cache[pos.position] = i;

        for(auto iter = actions.rbegin(); iter != actions.rend(); iter++) {
//            (*iter)->print();
            (*iter)->op_pos(pos);
        }

        if(cache.find(pos.position) != cache.end()) {
            // We have a loop
            loop_size = i - cache[pos.position];
            loop_start = cache[pos.position];
            printf("Loop size found %lli at count: %lli, loop_size: %lli\n", pos.position, i, loop_size);
            break;
        }

//        printf("%lli\n", pos.position);
        if(i % 1000000 == 0) {
            printf("%lli\n", i);
        }
    }

    // Hopefully found outside of loop
    if(loop_size > 0) {
        long long total_rounds_left = total_round - loop_start;
        long long loop_offset = (total_rounds_left - 1) % loop_size;

        for(long long i=0; i<loop_size; i++) {
            for(auto iter = actions.rbegin(); iter != actions.rend(); iter++) {
                (*iter)->op_pos(pos);
            }
        }
    }

    printf("Find it? %lli\n", pos.position);
}