#include "computer.h"
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <sys/poll.h>

typedef std::pair<int, int> Point;
#define OPEN 1
#define WALL 0
#define OXY 2

struct Node {
    Node(): visited(false), type(OPEN), depth(0) {}

    int depth;
    int type;
    bool visited;
};

#define NORTH 1
#define SOUTH 2
#define WEST 3
#define EAST 4

int reverse(int dir) {
    if(dir == NORTH) return SOUTH;
    if(dir == SOUTH) return NORTH;
    if(dir == EAST) return WEST;
    if(dir == WEST) return EAST;

    fprintf(stderr, "Failed direction");
    exit(1);
}

void dfs(Computer& puter, std::map<Point, Node>& dungeon, Point curr, int dir) {
    if(dungeon[curr].visited) {
        return;
    }

    if(dir != -1) {
        // Move the robot
        puter.write(dir);
        puter.run_to_done();
        std::vector<el_type> output = puter.release_output();

        dungeon[curr].type = output[0];
    }

    // Mark visited
    dungeon[curr].visited = true;
    if(dungeon[curr].type == WALL) {
        // Dont have to move back
        return;
    }

    dfs(puter, dungeon, Point(curr.first, curr.second+1), NORTH);
    dfs(puter, dungeon, Point(curr.first+1, curr.second), EAST);
    dfs(puter, dungeon, Point(curr.first, curr.second-1), SOUTH);
    dfs(puter, dungeon, Point(curr.first-1, curr.second), WEST);

    if(dir != -1) {
        // Go back
        puter.write(reverse(dir));
        puter.run_to_done();
        puter.release_output();
    }
}

int bfs(std::map<Point, Node>& dungeon, Point start) {
    for(auto iter=dungeon.begin(); iter != dungeon.end(); iter++) {
        iter->second.visited = false;
        iter->second.depth = -1;
    }

    std::queue<std::pair<Point, int>> frontier;
    frontier.push(std::pair<Point, int>(start, 0));
    int ret = -1;

    while(!frontier.empty()) {
        std::pair<Point, int> temp = frontier.front();
        Point curr = temp.first;
        int depth = temp.second;
        frontier.pop();

        Node& curr_node = dungeon.at(curr);
        if(curr_node.visited || curr_node.type == WALL) {
            continue;
        }
        curr_node.depth = depth;
        curr_node.visited = true;
        if(curr_node.type == OXY) {
            ret = depth;
        }

        // Add all of its neighbors
        frontier.push(std::pair<Point, int>(Point(curr.first+1, curr.second), depth+1));
        frontier.push(std::pair<Point, int>(Point(curr.first-1, curr.second), depth+1));
        frontier.push(std::pair<Point, int>(Point(curr.first, curr.second+1), depth+1));
        frontier.push(std::pair<Point, int>(Point(curr.first, curr.second-1), depth+1));
    }

    return ret;
}

int main(int argc, char** argv) {
    FILE *file = fopen(argv[1], "r");

    el_type temp;
    std::vector<el_type> mem;
    fscanf(file, "%li", &temp);
    mem.push_back(temp);
    while (fscanf(file, ",%li", &temp) == 1) {
        mem.push_back(temp);
    }
    printf("Nums read: %i\n", mem.size());

    Computer puter;

    puter.load(mem);
    puter.reset();

    std::map<Point, Node> dungeon;
    Point current = Point(0, 0);
    dungeon[current] = Node();
    dfs(puter, dungeon, Point(0,0), -1);
    Point oxy_loc;
    for(auto iter = dungeon.begin(); iter != dungeon.end(); iter++) {
        if(iter->second.type == OXY) {
            oxy_loc = iter->first;
            break;
        }
    }
    printf("Done with dfs OXY is: %i, %i\n", oxy_loc.first, oxy_loc.second);

    int steps = bfs(dungeon, Point(0,0));
    printf("Steps Required: %i\n", steps);

    bfs(dungeon, oxy_loc);
    int max_depth = 0;
    for(auto iter = dungeon.begin(); iter != dungeon.end(); iter++) {
        if(iter->second.depth > max_depth) {
            max_depth = iter->second.depth;
        }
    }
    printf("Flood time: %i\n", max_depth);
}

