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
    Point(int x=0, int y=0, int depth_modifier=0) : x(x), y(y), depth_modifier(depth_modifier) {
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

    Point left() const {
        return Point(x, y-1);
    }
    Point right() const {
        return Point(x, y+1);
    }
    Point up() const {
        return Point(x-1, y);
    }
    Point down() const {
        return Point(x+1, y);
    }

    int x;
    int y;
    int depth_modifier;
};

struct Node {
    Node(const string& portal, const Point& loc, int distance, int depth) : loc(loc), distance(distance), depth(depth), portal(portal) {

    }

    int distance;
    int depth;
    Point loc;
    string portal;
};

struct NodeCompare {
    bool operator()(const Node& lhs, const Node& rhs) const {
        // For the max queu, return true if lhs has worse priority than rhs
        if(lhs.distance != rhs.distance) {
            return lhs.distance > rhs.distance;
        }
        if(lhs.depth != rhs.depth) {
            return lhs.depth < rhs.depth;
        }

        return lhs.loc < rhs.loc;
    }
};

class Maze {
public:
    Maze(){

    }

    char find_it(const Point& p) const {
        if(p.x < 0 || p.x >= board.size()) {
            return ' ';
        }
        if(p.y < 0 || p.y >= board[p.x].size()) {
            return ' ';
        }

        return board[p.x][p.y];
    }

    bool is_inside(const Point& point) const {
        int x_edge_dist = min(static_cast<size_t>(point.x), board.size() - point.x);
        int y_edge_dist = min(point.y, y_size - point.y);

        return !(x_edge_dist < 5 || y_edge_dist < 4);

    }

    pair<string, Point> find_string(const Point& loc) const {
        string temp = "";
        char c = board[loc.x][loc.y];
        char up = find_it(loc.up());
        char down = find_it(loc.down());
        char left = find_it(loc.left());
        char right = find_it(loc.right());
        Point portal;
        if(right == '.') {
            temp.push_back(left);
            temp.push_back(c);
            portal = loc.right();
        }
        else if(left == '.') {
            temp.push_back(c);
            temp.push_back(right);
            portal = loc.left();
        }
        else if(down == '.') {
            temp.push_back(up);
            temp.push_back(c);
            portal = loc.down();
        }
        else if(up == '.') {
            temp.push_back(c);
            temp.push_back(down);
            portal = loc.up();
        }

        // Uncomment to do part 1
        if(is_inside(portal)) {
            portal.depth_modifier = 1;
        }
        else {
            portal.depth_modifier = -1;
        }

        return pair<string, Point>(temp, portal);
    }

    bool visitable(const Point& p, const vector<vector<bool>>& visited) const {
        if(p.x < 0 || p.x >= visited.size()) {
            return false;
        }
        if(p.y < 0 || p.y >= visited[p.x].size()) {
            return false;
        }

        char c = board[p.x][p.y];
        if(isalpha(c)) {
            return false;
        }
        if(c == '#') {
            return false;
        }

        return !visited[p.x][p.y];
    }

    // No portals. Gonna use this as a subroutine to djikstras
    int distance(const Point& from_, const Point& to_) const {
        Point from = from_ < to_ ? from_ : to_;
        Point to = from_ < to_ ? to_ : from_;
        static map<pair<Point, Point>, int> memo;
        auto temp = memo.find(pair<Point, Point>(from, to));
        if(temp != memo.end()) {
            return temp->second;
        }

        // Not using portals
        queue<pair<Point, int>> frontier;
        frontier.push(pair<Point, int>(from, 0));
        vector<vector<bool>> visited;
        for(int i=0; i<board.size(); i++) {
            vector<bool> working;
            for(int j=0; j<board[i].size(); j++) {
                working.push_back(false);
            }
            visited.push_back(working);
        }

//        printf("Searching from: %i %i to %i %i\n", from.x, from.y, to.x, to.y);
        while(!frontier.empty()) {
            auto curr = frontier.front();
            frontier.pop();

//            printf("    Popping %i, %i -- %i\n", curr.first.x, curr.first.y, curr.second);

            if(curr.first == to) {
                // Found it
                memo[pair<Point, Point>(from, to)] = curr.second;
                return curr.second;
            }

            visited[curr.first.x][curr.first.y] = true;

            Point left = curr.first.left();
            Point right = curr.first.right();
            Point up = curr.first.up();
            Point down = curr.first.down();

            if(visitable(left, visited)) {
                frontier.push(pair<Point, int>(left, curr.second+1));
            }
            if(visitable(right, visited)) {
                frontier.push(pair<Point, int>(right, curr.second+1));
            }
            if(visitable(up, visited)) {
                frontier.push(pair<Point, int>(up, curr.second+1));
            }
            if(visitable(down, visited)) {
                frontier.push(pair<Point, int>(down, curr.second+1));
            }
        }

        memo[pair<Point, Point>(from, to)] = -1;
        return -1;
    }

    void initialize(const vector<vector<char>>& board_) {
        board = board_;
        portals.clear();
        map<string, Point> temp_portals;
        y_size = 0;
        for(int i=0; i<board.size(); i++) {
            if(y_size < board[i].size()) {
                y_size = board[i].size();
            }
            for(int j=0; j<board[i].size(); j++) {
                char c = board[i][j];
                if(isalpha(c)) {
                    auto temp = find_string(Point(i, j));
                    string s = temp.first;
                    Point loc = temp.second;
                    if(s.size() == 0) {
                        continue;
                    }
                    printf("Found portal: %s\n", s.c_str());
                    if(s == "AA") {
                        loc.depth_modifier = -100;
                        start = loc;
                        portals[s] = pair<Point, Point>(loc, loc);
                    }
                    else if(s == "ZZ") {
                        loc.depth_modifier = -100;
                        end = loc;
                        portals[s] = pair<Point, Point>(loc, loc);
                    }
                    else if(temp_portals.find(s) != temp_portals.end()) {
                        // Already here
                        portals[s] = pair<Point, Point>(loc, temp_portals[s]);
                        board[loc.x][loc.y] = loc.depth_modifier > 0 ? '+' : '-';
                    }
                    else {
                        temp_portals[s] = loc;
                        board[loc.x][loc.y] = loc.depth_modifier > 0 ? '+' : '-';
                    }
                }
            }
        }
    }

    void search_it() const {
        // Gonna be a version of djikstra's algorithm
        priority_queue<Node, vector<Node>, NodeCompare> frontier;

        frontier.push(Node("AA", start, 0, 0));

        set<pair<Point, int>> visited;

        int distance_no_portals = distance(start, end);
        int distance_with_portals = 0;

        bool found = false;
        while(!found && !frontier.empty()) {
            auto curr = frontier.top();
            frontier.pop();

            if(visited.find(pair<Point, int>(curr.loc, curr.depth)) != visited.end()) {
                continue;
            }

            visited.insert(pair<Point, int>(curr.loc, curr.depth));

//            printf("Visiting Node: %s%+i, depth: %i, dist: %i\n", curr.portal.c_str(), curr.loc.depth_modifier, curr.depth, curr.distance);

            if(curr.loc == end && curr.depth == 0) {
                // Found it!
                found = true;
                distance_with_portals = curr.distance;
                break;
            }

            // Add all neighbors to the que with a nice decrease key
            for(auto temp : portals) {
                if(temp.first == "AA") {
                    continue;
                }
                if(temp.second.first == curr.loc) {
                    int depth = curr.depth + temp.second.first.depth_modifier;
                    if(depth < portals.size() && depth >= 0) {
//                        printf("    Pushing node: %s%+i, new depth: %i\n", temp.first.c_str(), temp.second.second.depth_modifier, depth);
                        frontier.push(Node(temp.first, temp.second.second, curr.distance + 1, depth));
                    }
                    continue;
                }
                if(temp.second.second == curr.loc) {
                    int depth = curr.depth + temp.second.second.depth_modifier;
                    if(depth < portals.size() && depth >= 0) {
//                        printf("    Pushing node: %s%+i, new depth: %i\n", temp.first.c_str(), temp.second.first.depth_modifier, depth);
                        frontier.push(Node(temp.first, temp.second.first, curr.distance + 1, depth));
                    }
                    continue;
                }

                int distance_1 = distance(curr.loc, temp.second.first);
                int distance_2 = distance(curr.loc, temp.second.second);
                if(distance_1 >= 0) {
                    // Its a valid node
//                    printf("    Pushing node: %s%+i, new distance: %i\n", temp.first.c_str(), temp.second.first.depth_modifier, curr.distance + distance_1);
                    frontier.push(Node(temp.first, temp.second.first, curr.distance + distance_1, curr.depth));
                }
                if(distance_2 >= 0) {
                    // Its a valid node
//                    printf("    Pushing node: %s%+i, new distance: %i\n", temp.first.c_str(), temp.second.second.depth_modifier, curr.distance + distance_2);
                    frontier.push(Node(temp.first, temp.second.second, curr.distance + distance_2, curr.depth));
                }
            }
        }

        if(!found) {
            printf("No path found with portals\b");
            printf("  Non portal path is: %i\n", distance_no_portals);
        }
        else if(distance_no_portals < 0 || distance_with_portals < distance_no_portals) {
            printf("Distance best using portals: %i\n", distance_with_portals);
        }
        else {
            printf("Distance best without portals: %i\n", distance_no_portals);
        }
    }

    void print_board() const {
        for(int i=0; i<board.size(); i++) {
            for(int j=0; j<board[i].size(); j++) {
                if(Point(i,j) == start || Point(i,j) == end) {
                    printf("+");
                }
                else {
                    printf("%c", board[i][j]);
                }
            }
            printf("\n");
        }
        printf("Num portals: %li\n", portals.size());
    }

    vector<vector<char>> board;
    int y_size;
    map<string, pair<Point, Point>> portals;
    Point start;
    Point end;
};


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

    // Find the portals
    Maze maze;
    maze.initialize(board);
    maze.print_board();
    maze.search_it();

    return 0;
}