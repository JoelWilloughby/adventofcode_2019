#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unordered_map>

using namespace std;

long long inv_mod(long long base, long long val) {
    // Do euclid's algorithm
    vector<long long> r;
    vector<long long> q;
    r.push_back(base);
    r.push_back(val);
    q.push_back(0);
    q.push_back(0);

    int n = 1;
    while(r[n]) {
        n++;
        long long next_q = r[n-2] / r[n-1];
        long long next_r = r[n-2] % r[n-1];

        r.push_back(next_r);
        q.push_back(next_q);
    }

    n--;

    if(r[n] != 1) {
        // Not coprime
        return 0;
    }

    long long w = 1;
    long long v = 0;
    // Backtrack
    for(int i=0; i<=n-2; i++) {
        long long temp = v;
        v = w;
        w = temp - q[n-i] * w;
    }

    if((w * val) % base != 1 && (w * val) % base != -(base - 1)) {
        printf("Failure of inv: %lli %lli %lli\n", base, val, w);
    }

    // Return the inverse
    return w;
}

class Shuffle {
public:
    Shuffle(long long base) : base(base), m(1), b(0) {

    }

    long long at(long long position) const {
        return (base + ((m * position + b) % base)) % base;
    }

    void fixup() {
        m %= base;
        b %= base;
    }

    void print() const {
        printf("y = %llix + %lli mod %lli\n", m, b, base);
    }

    long long inv(long long y) {
        long long minv = inv_mod(base, m); //TBD
        return (base + (minv * (y - b)) % base) % base;
    }

    long long base;
    long long m;
    long long b;
};

class Operation {
public:
    virtual void op(Shuffle& card) const = 0;
    virtual void print() const = 0;
};

class Cut : public Operation {
public:
    Cut(int n) : n(n) {

    }

    void op(Shuffle& f) const {
        f.b += f.m*n;
        f.fixup();
    }

    void print() const {
        printf("CUT %i\n", n);
    }

    int n;
};

class NewStack : public Operation {
public:

    void op(Shuffle& f) const {
        long long m = f.m;
        long long b = f.b;
        f.m = -m;
        f.b = b - m;
        f.fixup();
    }

    void print() const {
        printf("STCK:\n");
    }
};

class Incr : public Operation {
public:
    Incr(int n) : n(n) {

    }
    void op(Shuffle& f) const {
        long long m = f.m;
        f.m *= inv_mod(f.base, n);
        f.fixup();
    }

    void print() const {
        printf("INCR %i\n", n);
    }

    int n;
};

int main(int argc, char ** argv) {
    FILE * file = fopen(argv[1], "r");
    char* buf = NULL;
    size_t len = 0;
    vector<Operation*> actions;
    while(getline(&buf, &len, file) > 0) {
        int temp;
        if(strcmp(buf, "deal into new stack\n") == 0) {
            actions.push_back(new NewStack());
        }
        else if(sscanf(buf, "deal with increment %i", &temp) > 0) {
            actions.push_back(new Incr(temp));
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

    Shuffle shuff(10007);
    for(auto p : actions) {
        p->op(shuff);
    }
    shuff.print();

    printf("Find 2019 = %lli\n", shuff.inv(2019));
}