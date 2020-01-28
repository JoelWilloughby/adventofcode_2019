#include <cstring>
#include <cstdio>
#include <map>
#include <vector>
#include <string>
#include <set>
#include <iostream>

class Node {
public:
    Node() {};
    Node(std::string value) : value(value), visited(false), children() {}

    bool visited;
    int depth;
    std::string value;
    std::vector<std::string> children;
    std::string parent;
};

void setup_(Node& current, std::map<std::string, Node>& nodes, std::set<std::string>& found, int depth) {
    if(found.find(current.value) != found.end()) {
        printf("Cycle detected...\n");
        return;
    }

    for(int i=0; i<depth; i++) {
        printf("  ");
    }
    printf("%s\n", current.value.c_str());

    current.depth = depth;
    found.insert(current.value);
    for(auto iter = current.children.begin(); iter != current.children.end(); iter++) {
        setup_(nodes[*iter], nodes, found, depth + 1);
    }

    return;
}


void setup(std::map<std::string, Node>& nodes, std::string& com) {
    std::set<std::string> temp;
    return setup_(nodes[com], nodes, temp, 0);
}

int search(std::map<std::string, Node>& nodes, const std::string& a, const std::string& b) {
    std::string curr_a = a;
    std::string curr_b = b;

    int count = 0;
    while(curr_a != curr_b) {
        printf("A : %i, B: %i\n", nodes[curr_a].depth, nodes[curr_b].depth);
        if(nodes[curr_a].depth < nodes[curr_b].depth) {
            // move up on b
            curr_b = nodes[curr_b].parent;
        }
        else {
            curr_a = nodes[curr_a].parent;
        }
        count += 1;
    }

    return count;
}

int main(int argc, char** argv) {
    std::string inner;
    std::string outer;
    std::string com = "COM";
    std::map<std::string, Node> planets;
    while(std::cin.good()) {
        std::cin >> inner >> outer;
        if(!std::cin.good()) {
            break;
        }

        if(planets.find(inner) == planets.end()) {
            planets.insert(std::pair<std::string, Node>(inner, Node(inner)));
        }
        if(planets.find(outer) == planets.end()) {
            planets.insert(std::pair<std::string, Node>(outer, Node(outer)));
        }

        planets[inner].children.push_back(outer);
        planets[outer].parent = inner;
    }

    setup(planets, com);
    int count = 0;
    for(auto iter = planets.begin(); iter != planets.end(); iter++) {
        count += iter->second.depth;
    }

    printf("Count: %i\n", count);
    printf("Count: %i\n", search(planets, "YOU", "SAN"));
}


