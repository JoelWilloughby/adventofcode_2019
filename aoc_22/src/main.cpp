#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;

class Deck {
public:
    Deck(int n) {
        for(int i=0; i<n; i++) {
            deck.push_back(i);
        }
        top = 0;
        dir = 1;
    }

    int find(int val) const {
        int next = top;
        for(int i=0; i<deck.size(); i++) {
            if(deck[next] == val) {
                return i;
            }
            next += deck.size() + dir;
            next %= deck.size();
        }
        return -1;
    }

    void print() const {
        int next = top;
        for(int i=0; i<deck.size(); i++) {
            printf("%i ", deck[next]);
            next += deck.size() + dir;
            next %= deck.size();
        }
        printf("\n");
        printf("Top: %i, dir: %i\n", top, dir);
    }

    vector<int> deck;
    int top;
    int dir;
};

class Action {
public:
    virtual void doit(Deck& deck) = 0;
    virtual void print() const = 0;
};

class Cut : public Action {
public:
    Cut(int n) : n(n) {

    }

    void doit(Deck& deck) {
        deck.top += deck.deck.size() + deck.dir * n;
        deck.top %= deck.deck.size();
    }

    void print() const {
        printf("Cut %i\n", n);
    }

    int n;
};

class NewStack : public Action {
public:
    void doit(Deck& deck) {
        deck.dir *= -1;
        deck.top += deck.deck.size() + deck.dir;
        deck.top %= deck.deck.size();
    }

    void print() const {
        printf("deal into new stack\n");
    }
};

class DealInto : public Action {
public:
    DealInto(int n) : n(n) {

    }

    void doit(Deck& deck) {
        vector<int> temp = deck.deck;
        int index = deck.top;
        int next = 0;
        for(int i=0; i<deck.deck.size(); i++) {
            deck.deck[next] = temp[index];
            next += temp.size() + n;
            next %= temp.size();
            index += temp.size() + deck.dir;
            index %= temp.size();
        }
        deck.top = 0;
        deck.dir = 1;
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

    Deck deck(10007);
//    Deck deck(10);

    printf("Find 2019 = %i\n", deck.find(2019));
//    deck.print();

    for(auto p : actions) {
        p->doit(deck);
//        p->print();
//        deck.print();
    }

    printf("Find 2019 = %i\n", deck.find(2019));
}