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
};

struct Node {
    Node(const string& value, const string& parent, int distance) : value(value), parent(parent), distance(distance) {

    }

    int distance;
    string parent;
    string value;
};

struct NodeCompare {
    bool operator()(const Node& lhs, const Node& rhs) const {
        // For the max queu, return true if lhs has worse priority than rhs
        if(lhs.distance != rhs.distance) {
            return lhs.distance > rhs.distance;
        }

        return lhs.parent < rhs.parent;
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

    int string_distance(const string& a, const string& b) const{
        auto a_points = portals.at(a);
        auto b_points = portals.at(b);
        int dist1 = distance(a_points.first, b_points.first);
        int dist2 = distance(a_points.first, b_points.second);
        int dist3 = distance(a_points.second, b_points.first);
        int dist4 = distance(a_points.second, b_points.second);

        bool found = false;
        int min = 100000000;
        if(dist1 >= 0) {
            found = true;
            min = min < dist1 ? min : dist1;
        }
        if(dist2 >= 0) {
            found = true;
            min = min < dist2 ? min : dist2;
        }
        if(dist3 >= 0) {
            found = true;
            min = min < dist3 ? min : dist3;
        }
        if(dist4 >= 0) {
            found = true;
            min = min < dist4 ? min : dist4;
        }

        if(found) {
            return min;
        }

        return -1;
    }

    void initialize(const vector<vector<char>>& board_) {
        board = board_;
        portals.clear();
        map<string, Point> temp_portals;

        for(int i=0; i<board.size(); i++) {
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
                        start = loc;
                        portals[s] = pair<Point, Point>(loc, loc);
                    }
                    else if(s == "ZZ") {
                        end = loc;
                        portals[s] = pair<Point, Point>(loc, loc);
                    }
                    else if(temp_portals.find(s) != temp_portals.end()) {
                        // Already here
                        portals[s] = pair<Point, Point>(loc, temp_portals[s]);
                        board[loc.x][loc.y] = '*';
                    }
                    else {
                        temp_portals[s] = loc;
                        board[loc.x][loc.y] = '*';
                    }
                }
            }
        }
    }

    void search_it() const {
        // Gonna be a version of djikstra's algorithm
        priority_queue<Node, vector<Node>, NodeCompare> frontier;

        frontier.push(Node("AA", "", 0));

        set<string> visited;

        int distance_no_portals = distance(start, end);
        int distance_with_portals = 0;

        bool found = false;
        while(!found && !frontier.empty()) {
            auto curr = frontier.top();
            frontier.pop();

            if(visited.find(curr.value) != visited.end()) {
                continue;
            }

            visited.insert(curr.value);

            printf("Visiting Node: %s, parent: %s, dist: %i\n", curr.value.c_str(), curr.parent.c_str(), curr.distance);

            if(curr.value == "ZZ") {
                // Found it!
                found = true;
                distance_with_portals = curr.distance;
                break;
            }


            Point a = portals.at(curr.value).first;
            Point b = portals.at(curr.value).second;
            // Add all neighbors to the que with a nice decrease key
            for(auto temp : portals) {
                if(temp.first == curr.value) {
                    continue;
                }
                int distance = string_distance(curr.value, temp.first);
                if(distance < 0) {
                    continue;
                }

                printf("  Pushing %s, %i\n", temp.first.c_str(), curr.distance + distance + 1);
                // Plus one to count the move through the portal
                frontier.push(Node(temp.first, curr.value, curr.distance + distance + 1));
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