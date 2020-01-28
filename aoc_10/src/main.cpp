#include <cmath>
#include <cstdio>
#include <map>
#include <numeric>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>

struct Node {
    int covered_count = 0;
};


int gcd(int a, int b)
{
    if(a < 0) {
        return gcd(-a, b);
    }
    if(b < 0) {
        return gcd(a, -b);
    }

    // Everything divides 0
    if (a == 0)
       return b;
    if (b == 0)
       return a;

    // base case
    if (a == b)
        return a;

    // a is greater
    if (a > b)
        return gcd(a-b, b);
    return gcd(a, b-a);
}

class Field {
public:
    Field(std::vector<std::vector<char>>& input) {
        this->input = input;
        for(int i=0; i<input.size(); i++) {
            for(int j=0; j<input[i].size(); j++) {
                if(input[i][j] == '#') {
                    asteroids.push_back(std::pair<int, int>(i,j));
                    blocked_counts[std::pair<int, int>(i,j)] = 0;
                }
            }
        }
    }

    std::pair<int, int> get_stride(std::pair<int, int>& a, std::pair<int, int>& b) {
        int x_stride = b.first - a.first;
        int y_stride = b.second - a.second;

        if(x_stride != 0 || y_stride != 0) {
            int gcd_ = gcd(x_stride, y_stride);
            x_stride /= gcd_;
            y_stride /= gcd_;
        }

        return  std::pair<int,int>(x_stride, y_stride);
    }

    std::vector<std::pair<int, int>> search_single(std::pair<int, int> a, std::pair<int, int> b) {
        auto stride = get_stride(a, b);
        int x_stride = stride.first;
        int y_stride = stride.second;

        std::vector<std::pair<int, int>> ret;
        if(x_stride == 0 && y_stride == 0) {
            return ret;
        }

        int curr_x = b.first + x_stride;
        int curr_y = b.second + y_stride;

//        printf("A: (%i,%i), B: (%i,%i), stride: %i, %i: ", a.first, a.second, b.first, b.second, x_stride, y_stride);
        while(curr_x >= 0 && curr_x < input.size() && curr_y >= 0 && curr_y < input[0].size()) {
//           printf("(%i,%i) ", curr_x, curr_y);
           if(input[curr_x][curr_y] == '#') {
               ret.push_back(std::pair<int, int>(curr_x, curr_y));
           }
           curr_x += x_stride;
           curr_y += y_stride;
        }

        return ret;
//        printf("\n");
    }

struct AngleCmp {
    int quad(int x, int y) const {
        if(x == 0 && y == 0) {
            return 0;
        }
        if(x < 0 && y == 0) {
            return 1;
        }
        if(x == 0 && y < 0) {
            return 3;
        }
        if(x > 0 && y == 0) {
            return 5;
        }
        if(x == 0 && y > 0) {
            return 7;
        }

        if(x < 0 && y > 0) {
            return 2;
        }
        if(x > 0 && y > 0) {
            return 4;
        }
        if(x > 0 && y < 0) {
            return 6;
        }
        if(x < 0 && y < 0) {
            return 8;
        }
        return 9;
    }

    bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) const {
        if(a == b) return false;

        int a_x = a.first;
        int a_y = a.second;
        int b_x = b.first;
        int b_y = b.second;

        if(quad(a_x, a_y) != quad(b_x, b_y)) {
            return quad(a_x, a_y) < quad(b_x, b_y);
        }

//        printf("Comparing %i %i to %i %i with base %i %i -- %i\n", a.first, a.second, b.first, b.second, base.first, base.second, quad(a_x, a_y));

        switch(quad(a_x, a_y)) {
            case 0:
            case 1:
            case 3:
            case 5:
            case 7:
                return false;
            case 2:
            case 4:
            case 6:
            case 8:
                return (float)a_x/(float)a_y < (float)b_x/(float)b_y;
//                return (float)a_y/(float)a_x > (float)b_y/(float)b_x;
            default:
                break;
        }

        return false;
    }
};

    struct DistCompare {
        DistCompare(std::pair<int,int>& base) : base(base) {

        }
        bool operator()(const std::pair<int,int>& a, const std::pair<int, int>& b) const {
            int a_x_dist = a.first - base.first;
            int a_y_dist = a.second - base.second;

            int b_x_dist = b.first - base.first;
            int b_y_dist = b.second - base.second;

            return a_x_dist * a_x_dist + a_y_dist * a_y_dist < b_x_dist * b_x_dist + b_y_dist * b_y_dist;
        }
        std::pair<int, int> base;
    };

    void destroy(std::pair<int, int>& base) {
        std::vector<std::pair<int, int>> destroyed;
        AngleCmp cmp;
        DistCompare distcmp(base);
        std::vector<std::vector<std::pair<int, int>>> stuff;
        std::vector<int> current;
        std::map<std::pair<int, int>, std::set<std::pair<int, int>>> cache;

        for(int i=0; i<asteroids.size(); i++) {
            if(asteroids[i] == base) continue;
            auto stride = get_stride(base, asteroids[i]);
            cache[stride].insert(asteroids[i]);
        }

        std::vector<std::pair<int, int>> tempit;
        for(auto iter=cache.begin(); iter != cache.end(); iter++) {
            tempit.push_back(iter->first);
        }
        std::sort(tempit.begin(), tempit.end(), cmp);
//        for(int i=0; i<tempit.size(); i++) {
//            printf("%4i %4i %4i\n", tempit[i].first, tempit[i].second, cmp.quad(tempit[i].first, tempit[i].second));
//        }

        for(auto iter = tempit.begin(); iter != tempit.end(); iter++) {
            std::vector<std::pair<int, int>> tempq;
            for(auto inner_iter = cache[*iter].begin(); inner_iter != cache[*iter].end(); inner_iter++) {
                tempq.push_back(*inner_iter);
            }
            std::sort(tempq.begin(), tempq.end(), distcmp);
            stuff.push_back(tempq);
            current.push_back(0);
        }

        int count = 0;
        int i=0;

        printf("Num stuff: %i\n", stuff.size());
        while(count < 200) {
            if(stuff[i].size() != 0) {
                std::pair<int, int> a;
                a = stuff[i][current[i]++];
                std::pair<int, int> stride = get_stride(base, a);

                printf("Stride %i %i -- Destroying %ith: %i %i (%i %i), rem is %i\n", stride.first, stride.second, count, a.first, a.second, a.first - base.first, a.second - base.second, stuff[i].size() - current[i]);
                destroyed.push_back(a);
                count++;
            }

            i = (i+1) % stuff.size();
        }

        auto last_destroyed = destroyed[destroyed.size()-1];
        printf("The 200th thing destroyed is: %i, %i\n", last_destroyed.first, last_destroyed.second);
    }

    void search() {
        for(int i=0; i<asteroids.size(); i++) {
            std::set<std::pair<int, int>> seen;
            for(int j=0; j<asteroids.size(); j++) {
                if(i == j) {
                    continue;
                }
                auto blocked = search_single(asteroids[i], asteroids[j]);
                for(int i=0; i<blocked.size(); i++) {
                    seen.insert(blocked[i]);
                }
            }
            blocked_counts[asteroids[i]] = seen.size();
        }

        int min_blocked = asteroids.size();
        int best_index = -1;
        for(int i=0; i<asteroids.size(); i++) {
            int curr_count = blocked_counts[asteroids[i]];
            if(curr_count < min_blocked) {
                min_blocked = curr_count;
                best_index = i;
            }
        }

        printf("Maximum found is: %i, asteroids: %i\n", asteroids.size() - min_blocked, asteroids.size());
        printf("Best loc: %i, %i\n", asteroids[best_index].first, asteroids[best_index].second);

        destroy(asteroids[best_index]);
    }

    std::vector<std::vector<char>> input;
    std::map<std::pair<int, int>, int> blocked_counts;
    std::vector<std::pair<int, int>> asteroids;
};

int main(int argc, char ** argv) {
    FILE * file = fopen(argv[1], "r");

    char *line;
    size_t len;
    bool done = false;

    std::vector<std::vector<char>> temp;

    char c = getc(file);
    while(!done) {
        std::vector<char> temper;
        while(c == '#' || c == '.') {
            temper.push_back(c);
            c = getc(file);
        }
        temp.push_back(temper);

        while(c != '#' && c != '.') {
            c = getc(file);
            if(c == EOF) {
                done = true;
                break;
            }
        }
    }

    for(int i=0; i<temp.size(); i++) {
        for(int j=0; j<temp[i].size(); j++) {
            printf("%c", temp[i][j]);
        }
        printf("\n");
    }

    Field f(temp);
    f.search();

    return 0;
}
