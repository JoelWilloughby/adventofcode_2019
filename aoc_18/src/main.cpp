#include <cstdio>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <queue>
#include <set>
#include <vector>
#include <map>
#include <tuple>

using namespace std;

struct Point {
    Point(int x=0, int y=0) : x(x), y(y) {
    }

    bool operator== (const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator< (const Point& other) const {
        if(x < other.x) {
            return true;
        }

        if(y < other.y) {
            return true;
        }

        return false;
    }

    unsigned distance_to(const Point& p) const {
        return abs(x - p.x) + abs(y - p.y);
    }

    int x;
    int y;
};

struct KeyBitfield {
    KeyBitfield(bool init) {
        if(init) {
            bitfield = 0xffffffff;
        }
        else {
            bitfield = 0x00000000;
        }
    }

    bool operator< (const KeyBitfield& other) const {
        return bitfield < other.bitfield;
    }

    void set(char c) {
        c = tolower(c) - 'a';
        bitfield |= (1 << c);
    }

    void clear(char c) {
        c = tolower(c) - 'a';
        bitfield &= ~(1 << c);
    }

    bool get(char c) const {
        c = tolower(c) - 'a';
        return ((1 << c) & bitfield) != 0;
    }

    bool contains(const KeyBitfield& other) const {
        return (bitfield & other.bitfield) == other.bitfield;
    }

    unsigned num_set() const {
        return __builtin_popcount(bitfield);
    }

    uint32_t bitfield;
};

struct Node {
    Node(char c, unsigned fitness, const KeyBitfield& keys, unsigned distance) :
     c(c), fitness(fitness), keys(keys), distance(distance) {
    }

    char c;
    unsigned fitness;
    KeyBitfield keys;
    unsigned distance;
};

struct CompareNode  {
    // Should be true when the lhs has a worse fitness than the rhs
    bool operator()(Node& lhs, Node& rhs) {
        if(lhs.distance + lhs.fitness != rhs.distance + rhs.fitness) {
            return lhs.distance + lhs.fitness > rhs.distance + rhs.fitness;
        }
        if(lhs.keys.num_set() != rhs.keys.num_set()) {
            return lhs.keys.num_set() < rhs.keys.num_set();
        }
        return lhs.c > rhs.c;
    }
};

class Maze {
public:
    Maze(vector<vector<char>>& board) : board(board), num_keys(0), start_loc() {
        initialize();
    }

    void print_board() {
        for(auto row : board) {
            for (auto c : row) {
                printf("%c", c);
            }
            printf("\n");
        }

        printf("Num keys: %i\n", num_keys);
        printf("Start location: %i, %i\n", start_loc.x, start_loc.y);
    }

    void initialize() {
        num_keys = 0;
        for(int i=0; i<board.size(); i++) {
            auto row = board[i];
            for(int j=0; j<row.size(); j++) {
                char c = row[j];
                if(isupper(c)) {
                    door_locs[c] = Point(i,j);
                }
                else if (islower(c)) {
                    key_locs[c] = Point(i,j);
                    num_keys++;
                }
                else if (c == '@') {
                    start_loc = Point(i,j);
                    key_locs[c] = start_loc;
                }
            }
        }
    }

    bool vistable(const Point& p, const vector<vector<bool>>& visited, const KeyBitfield& keys) const {
        if(p.x < 0 || p.y < 0 || p.x >= board.size() || p.y >= board[0].size()) {
            return false;
        }

        if(visited[p.x][p.y]) {
            return false;
        }

        char c = board[p.x][p.y];
        if(c == '#') {
            return false;
        }

        // Handle keys and doors
        if(isupper(c)) {
            return keys.get(c);
        }

        // Others are . or another key, which we don't really care about.
        return true;
    }

    int distance_to(char from, char to, const KeyBitfield& keys) const {
        static map<tuple<char, char>, map<KeyBitfield, int>> memo;
//        tuple<char, char, unsigned> memo_index(from, to, keys.bitfield);
        tuple<char, char> memo_index(from, to);
        unsigned bitfield = keys.bitfield;
        auto res = memo.find(memo_index);
        if(res != memo.end()) {
            for(auto temp : res->second) {
                if(temp.first.contains(keys)) {
                    printf("Used memo!\n");
                    return temp.second;
                }
            }
        }

        // Bredth first search. This should also be memoized most likely.
        queue<pair<Point, int>> frontier;
        frontier.push(pair<Point, int>(key_locs[from], 0));
        Point target = key_locs[to];

        vector<vector<bool>> visited;
        for(int i=0; i<board.size(); i++) {
            vector<bool> temp;
            for(int j=0; j<board[i].size(); j++) {
                temp.push_back(false);
            }
            visited.push_back(temp);
        }

        KeyBitfield doors(false);
        // Must search over cells. This is quite annoying and why we need to cache!
        while(!frontier.empty()) {
            pair<Point, int> curr = frontier.front();
            frontier.pop();

            if(curr.first == target) {
                memo[memo_index][keys] = curr.second;
                return curr.second;
            }

            // Mark visited
            visited[curr.first.x][curr.first.y] = true;

            // Visit this node.
            Point up(curr.first.x, curr.first.y - 1);
            Point down(curr.first.x, curr.first.y + 1);
            Point left(curr.first.x - 1, curr.first.y);
            Point right(curr.first.x + 1, curr.first.y);
            // Check if it is good
            if(vistable(up, visited, keys)) {
                frontier.push(pair<Point, int>(up, curr.second+1));
            }
            if(vistable(down, visited, keys)) {
                frontier.push(pair<Point, int>(down, curr.second+1));
            }
            if(vistable(left, visited, keys)) {
                frontier.push(pair<Point, int>(left, curr.second+1));
            }
            if(vistable(right, visited, keys)) {
                frontier.push(pair<Point, int>(right, curr.second+1));
            }
        }

        memo[memo_index][keys] = -1;
        return -1;
    }

    unsigned guesstimate(char c, const KeyBitfield& seen) const {
        // Just going to use the max distance to all remaining keys. Shrug
        unsigned max = 0;
        for(int i=0; i<num_keys; i++) {
            char temp = i + 'a';
            if(seen.get(temp)) {
                continue;
            }
            unsigned distance = key_locs[c].distance_to(key_locs[temp]);
            if(distance > max) {
                max = distance;
            }
        }

        return max;
    }

    void search_it() const {
        priority_queue<Node, vector<Node>, CompareNode> frontier;
        set<pair<char, unsigned>> visited;
        vector<KeyBitfield> visited_keys[255];

        // Prime the pump
        frontier.push(Node('@', 0, KeyBitfield(false), 0));

        // Do the search
        while(!frontier.empty()) {
            // Pop off the head.
            Node curr = frontier.top();
            frontier.pop();
            KeyBitfield new_keys = curr.keys;
            if(curr.c != '@') {
                new_keys.set(curr.c);
            }
            bool key_found = false;
            for(auto key : visited_keys[curr.c]) {
                if(key.contains(new_keys)) {
                    // Skip this key
                    key_found = true;
                }
            }
//            if(visited.find(pair<char, unsigned>(curr.c, curr.keys.bitfield)) != visited.end()) {
            if(key_found) {
                continue;
            }
            visited.insert(pair<char, unsigned>(curr.c, curr.keys.bitfield));
            visited_keys[curr.c].push_back(new_keys);

//            printf("Handling: %c, %i, %i, %08x\n", curr.c, curr.distance, curr.fitness, curr.keys.bitfield);

            if(new_keys.num_set() == num_keys) {
                // We did it!
                printf("We did it: %i\n", curr.distance);
                return;
            }

            for(int i=0; i<num_keys; i++) {
                // Search through all the keys
                char neighbor = 'a' + i;
                if(new_keys.get(neighbor)) {
                    // Already visited this node on this path
                    continue;
                }

                // Gather the fitness of this new node.
                int distance = distance_to(curr.c, neighbor, new_keys);
                if(distance >= 0) {
                    unsigned fitness = guesstimate(neighbor, new_keys);
//                    printf("    Pushing: %c, %i, %i\n", neighbor, curr.distance + distance, fitness);
                    frontier.push(Node(neighbor, fitness, new_keys, curr.distance + distance));
                }
            }
        }

        printf("Failed to find solution\n");
    }

    vector<vector<char>> board;
    // Indexable via a letter
    Point key_locs[256];
    Point door_locs[256];
    Point start_loc;
    int num_keys;
};

// Algo takes like a couple minutes
// We did it: 5964

// real    1m49.915s
// user    1m49.888s
// sys     0m0.008s

int main(int argc, char ** argv) {
    FILE * file = fopen(argv[1], "r");
    vector<vector<char>> board;
    vector<char> working;
    char c;
    while((c = fgetc(file)) != EOF) {
        if(c == '\n') {
            board.push_back(working);
            working.clear();
        }
        else {
            working.push_back(c);
        }
    }
    if(!working.empty()) {
        board.push_back(working);
    }

    Maze maze(board);
    maze.print_board();
    KeyBitfield test(false);
    test.set('g');
    printf("0x%08x %i\n", test.bitfield, test.get('g'));
    printf("%i\n", maze.distance_to('@', 'p', KeyBitfield(false)));
    maze.search_it();

    return 0;
}