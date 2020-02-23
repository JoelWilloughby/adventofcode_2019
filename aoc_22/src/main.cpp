#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;

class Card {
public:
    Card(int deck_size, int position, int value) : deck_size(deck_size), position(position), value(value) {

    }

    int deck_size;
    int position;
    int value;
};

class Action {
public:
    virtual void op_card(Card& card) = 0;
//    virtual void op_pos(Card& card) = 0;
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

    void print() const {
        printf("deal into new stack\n");
    }
};

class DealInto : public Action {
public:
    DealInto(int n) : n(n) {

    }

    void op_card(Card& card) {
        // May have to perform this one by one
        long long temp = n * card.position;
        card.position = temp % card.deck_size;
    }

    void print() const {
        printf("deal with increment %i\n", n);
    }

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
    printf("Find 2019 = %i\n", card.position);

    for(auto p : actions) {
        p->op_card(card);
//        p->print();
    }

    printf("Find 2019 = %i\n", card.position);
}