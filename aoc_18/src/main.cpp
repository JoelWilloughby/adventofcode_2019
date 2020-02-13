#include <cstdio>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
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

    // Needed so we use this as a key in some maps
    bool operator< (const Point& other) const {
        if(x != other.x) {
            return x < other.x;
        }

        if(y != other.y) {
            return y < other.y;
        }

        return false;
    }

    // Just returns the Manhattan distance to the other point
    unsigned distance_to(const Point& p) const {
        return abs(x - p.x) + abs(y - p.y);
    }

    int x;
    int y;
};


// This struct is supposed to be a set representing the keys we have collected
// so far. Since keys are letters, there can only ever be 26 of them at most,
// so just jam them in a u32 bitfield. This allows for quick lookup of whether
// or not we have keys as well as efficient set operations (intersect, union, etc)
// should we end up needing them
struct KeyBitfield {
    KeyBitfield(bool init) {
        if(init) {
            bitfield = 0xffffffff;
        }
        else {
            bitfield = 0x00000000;
        }
    }

    // So we can use this freely in STL containers
    bool operator< (const KeyBitfield& other) const {
        return bitfield < other.bitfield;
    }

    // Puts a given key into the set
    void set(char c) {
        c = tolower(c) - 'a';
        bitfield |= (1 << c);
    }

    // Removes a given key from the set
    void clear(char c) {
        c = tolower(c) - 'a';
        bitfield &= ~(1 << c);
    }

    // Returns true iff the given key is in the set
    bool get(char c) const {
        c = tolower(c) - 'a';
        return ((1 << c) & bitfield) != 0;
    }

    // Returns true iff all the keys in other are also in this set
    bool contains(const KeyBitfield& other) const {
        return (bitfield & other.bitfield) == other.bitfield;
    }

    // Returns the number of keys in the set
    unsigned num_set() const {
        return __builtin_popcount(bitfield);
    }

    uint32_t bitfield;
};

// This is used in our main A* search
// Each node consists of the current location of the robot(s),
// the estimated fitness of the node, the keys collected so far,
// and the distance required to get to this point.
struct Node {
    Node(const char* locs, unsigned fitness, const KeyBitfield& keys, unsigned distance) :
    fitness(fitness), keys(keys), distance(distance) {
        for(int i=0; i<4; i++) {
            this->locs[i] = locs[i];
        }
    }

    // There are 4 chars because of Part 2, when we can have up to 4 robots
    // at a time. This array just represents wach of their locations
    char locs[4];
    // This is the heuristic part of the A* search. To work, the heuristic
    // must be admissible, meaning it never over-estimates the actual cost of
    // this node.
    unsigned fitness;
    // The keys collected so far at this node
    KeyBitfield keys;
    // The distance required to reach this node
    unsigned distance;
};

// Used in the priority queue later during A*. Basically just compares
// 2 nodes in such a way that the STL priority_queue will prioritize nodes
// that have lower distance + fitness
struct CompareNode  {
    // Should be true when the lhs has a worse priority than the rhs
    bool operator()(Node& lhs, Node& rhs) {
        if(lhs.distance + lhs.fitness != rhs.distance + rhs.fitness) {
            // Worse priority means more distance estimate
            return lhs.distance + lhs.fitness > rhs.distance + rhs.fitness;
        }
        if(lhs.keys.num_set() != rhs.keys.num_set()) {
            // Worse priority means less keys collected so far
            return lhs.keys.num_set() < rhs.keys.num_set();
        }
        for(int i=0; i<4; i++) {
            if(lhs.locs[i] != rhs.locs[i]) {
                // Arbitrary, just use alphabetical order
                return lhs.locs[i] < rhs.locs[i];
            }
        }
        // These nodes are definitely equal
        return false;
    }
};

// The main runner of this problem
class Maze {
public:
    Maze(vector<vector<char>>& board) : board(board), num_keys(0) {
        initialize();
    }

    // Prints the board
    void print_board() {
        for(auto row : board) {
            for (auto c : row) {
                printf("%c", c);
            }
            printf("\n");
        }

        printf("Num keys: %i\n", num_keys);
    }

    // Reads the given board and collects all of the key locations
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
                    key_locs[c] = Point(i,j);
                }
            }
        }
    }

    // Used in the lower level BFS to get pairwise distances between keys
    // A point on the board is visitable if it is not a wall and it has not
    // been visited yet and (if it is a door, we have already collected the
    // key to that door).
    bool vistable(const Point& p, const vector<vector<bool>>& visited, const KeyBitfield& keys) const {
        // Check out of bounds. Shouldn't happen due to the border but (shurg)
//        if(p.x < 0 || p.y < 0 || p.x >= board.size() || p.y >= board[0].size()) {
//            return false;
//        }

        if(visited[p.x][p.y]) {
            return false;
        }

        char c = board[p.x][p.y];
        if(c == '#') {
            // Its a wall
            return false;
        }

        // Handle keys and doors
        if(isupper(c)) {
            return keys.get(c);
        }

        // Others are . or another key, which we don't really care about.
        return true;
    }

    // Returns the distance from one node to another assuming some number of
    // keys have been collected. This functions uses a BFS to search from from
    // to to assuming keys collected. It is called a bunch of times from the
    // outer A* loop, and some attempt at memoization has been made. The jury
    // is still out on how well that performs
    // TODO: Make the memoization better. Should be able to be much smarter about
    // keys collected
    // Note that we don't worry about collecting keys in this function. This is
    // due to the fact that in our outer loop, we will also handle the case
    // that we wnet directly to that key
    map<char, int> distance_from(char from, const KeyBitfield& keys) const {
        queue<pair<Point, int>> frontier;
        frontier.push(pair<Point, int>(key_locs[from], 0));
        map<Point, char> targets;
        for(int i=0; i<num_keys; i++) {
            char c = 'a' + i;
            if(c == from || keys.get(c)) {
                continue;
            }
            targets[key_locs[c]] = c;
        }

        vector<vector<bool>> visited;
        for(int i=0; i<board.size(); i++) {
            vector<bool> temp;
            for(int j=0; j<board[i].size(); j++) {
                temp.push_back(false);
            }
            visited.push_back(temp);
        }

        map<char, int> distances;

        while(!frontier.empty()) {
            pair<Point, int> curr = frontier.front();
            frontier.pop();

            auto temp = targets.find(curr.first);
            if(temp != targets.end()) {
                distances[temp->second] = curr.second;
            }

            // Mark visited
            visited[curr.first.x][curr.first.y] = true;

            // Put all the nodes neighbors in the frontier
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

        return distances;
    }

    // Heuristic for evaluating estimated cost to goal
    unsigned guesstimate(char c, const KeyBitfield& seen, int quadrant=-1) const {
        // Just going to use the max distance to all remaining keys. Shrug
        unsigned max = 0;
        for(int i=0; i<num_keys; i++) {
            char temp = i + 'a';
            if(seen.get(temp)) {
                continue;
            }
            bool should_skip = false;
            if(quadrant != -1) {
                // This helps the four robot case in part 2. To remain admissible,
                // we need to use only the other keys in our quadrant to estimate
                // fitness.
                int x_dir = quadrant % 2 ? 1 : -1;
                int y_dir = quadrant / 2 ? 1 : -1;

                int x = key_locs[temp].x;
                int y = key_locs[temp].y;

                if(x_dir * (x - board.size() / 2) < 0) {
                    should_skip = true;
                }

                if(y_dir * (y - board.size() / 2) < 0) {
                    should_skip = true;
                }
            }
            if(should_skip) {
                // Not in our quadrant
                continue;
            }

            unsigned distance = key_locs[c].distance_to(key_locs[temp]);
            if(distance > max) {
                max = distance;
            }
        }

        return max;
    }

    // Main runner of the problem. Runs A* on the keys collected. Can handle up to
    // 4 robots. If wanting to do 4 bots, should call split beforehand.
    void search_it(int num_bots=1) const {
        // The frontier, prioritized by minimum (distance_so_far + fitness)
        priority_queue<Node, vector<Node>, CompareNode> frontier;
        // Can be used to prune early. If visited keys already contains a key that
        // you are currently on, you don't have to search due to the optimaility of
        // BFS/A*
        vector<KeyBitfield> visited_keys[255][num_bots];

        // Prime the pump. Ordering of the characters matters! Quadrant is important
        frontier.push(Node("@!$*", 0, KeyBitfield(false), 0));

        // Do the search
        while(!frontier.empty()) {
            // Pop off the head.
            Node curr = frontier.top();
            frontier.pop();

            KeyBitfield new_keys = curr.keys;

            // Check to see if this node is a strictly worse node than we have seen
            // before
            bool key_found = false;
            for (int i = 0; i < num_bots; i++) {
                if (isalpha(curr.locs[i])) {
                    // Visit this new location
                    new_keys.set(curr.locs[i]);
                }
            }
            for (int i = 0; i < num_bots; i++) {
                for (auto key : visited_keys[curr.locs[i]][i]) {
                    if (key.contains(new_keys)) {
                        // This set of keys is a direct subset of an already found
                        // set of keys and we are at the same location.
                        key_found = true;
                    }
                }

                if (!key_found) {
                    // Add it to our cache
                    visited_keys[curr.locs[i]][i].push_back(new_keys);
                }
            }

            if (key_found) {
                continue;
            }

            // Debugging proves useful
//            printf("Handling: %c%c%c%c, %i, %i, %08x\n", curr.locs[0], curr.locs[1], curr.locs[2], curr.locs[3], curr.distance, curr.fitness, curr.keys.bitfield);

            if (new_keys.num_set() == num_keys) {
                // We did it! We found all the keys!
                printf("We did it: %i\n", curr.distance);
                return;
            }

            // Gather the fitness of this new node.
            for (int i = 0; i < num_bots; i++) {
                char new_locs[4];
                memcpy(new_locs, curr.locs, sizeof(new_locs));
                auto distances = distance_from(curr.locs[i], new_keys);
                for (auto it : distances) {
                    char neighbor = it.first;
                    int distance = it.second;
                    if (new_keys.get(neighbor)) {
                        continue;
                    }
                    if(distance <= 0) {
                        continue;
                    }

                    unsigned fitness;
                    if (num_bots == 1) {
                        fitness = guesstimate(neighbor, new_keys);
                    } else {
                        fitness = guesstimate(neighbor, new_keys, i);
                    }
                    new_locs[i] = neighbor;
//                    printf("   %i Pushing: %c, %i, %i\n", i, neighbor, curr.distance + distance, fitness);
                    frontier.push(Node(new_locs, fitness, new_keys, curr.distance + distance));
                }
            }
        }

        printf("Failed to find solution\n");
    }

    // Turns the board from part 1 into the one needed for part 2
    void split() {
        for(int i=0; i<board.size(); i++) {
            for(int j=0; j<board[i].size(); j++) {
                char c = board[i][j];
                if(c == '@') {
                    // Do some replacing.
                    board[i][j] = '#';
                    board[i][j-1] = '#';
                    board[i][j+1] = '#';
                    board[i+1][j] = '#';
                    board[i-1][j] = '#';
                    board[i-1][j-1] = '@';
                    key_locs['@'] = Point(i-1, j-1);
                    board[i-1][j+1] = '!';
                    key_locs['!'] = Point(i-1, j+1);
                    board[i+1][j-1] = '$';
                    key_locs['$'] = Point(i+1, j-1);
                    board[i+1][j+1] = '*';
                    key_locs['*'] = Point(i+1, j+1);
                }
            }
        }
    }


    vector<vector<char>> board;
    // Indexable via a character
    Point key_locs[256];
    Point door_locs[256];
    int num_keys;
};

// Algo takes like a couple minutes
// We did it: 5964

// real    1m49.915s
// user    1m49.888s
// sys     0m0.008s

// Part 2
//We did it: 1996
//
//real   0m41.734s
//user   0m41.643s
//sys    0m0.016s


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
    // Uncomment to run part 1 as well. Can take a while
    maze.search_it();
    maze.split();
    maze.print_board();
    maze.search_it(4);

    return 0;
}