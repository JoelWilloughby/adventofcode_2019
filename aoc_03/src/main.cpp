#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>

#include <vector>


void read_track(std::vector<std::pair<int, int>>& track, FILE* f) {
    char buf[1600];
    fgets(buf, 1600, f);
    char dir;
    uint32_t dist;
    int x = 0;
    int y = 0;
    int offset = 0;
    int readChars;
    sscanf(buf + offset, "%c%i%n", &dir, &dist, &readChars);
    offset += readChars;
    while (sscanf(buf + offset, ",%n", &readChars) != EOF) {
        offset += readChars;
        switch(dir) {
            case 'R':
                x += dist;
                break;
            case 'L':
                x -= dist;
                break;
            case 'U':
                y += dist;
                break;
            case 'D':
                y -= dist;
                break;
            default:
                break;
        }

        track.push_back(std::pair<int, int>(x, y));
        printf("(%i,%i) ", x, y);
        sscanf(buf + offset, "%c%i%n", &dir, &dist, &readChars);
        offset += readChars;
    }
    printf("\n");
}

bool is_vertical(std::pair<int, int> p1, std::pair<int, int> p2) {
    return p1.first == p2.first;
}

void twist(std::pair<int, int>& p) {
    std::swap(p.first, p.second);
}

int intersect(std::pair<int, int> p1, std::pair<int, int> p2,
              std::pair<int, int> q1, std::pair<int, int> q2,
              uint32_t& time1, uint32_t& time2, uint32_t flags = 0) {
    // Reorder so we go left to right or down to up
    if(is_vertical(p1, p2)) {
        if(p1.second > p2.second) {
            flags |= 1;
            std::swap(p1, p2);
        }
    } else {
        if(p1.first > p2.first) {
            flags |= 1;
            std::swap(p1, p2);
        }
    }

    if(is_vertical(q1, q2)) {
        if(q1.second > q2.second) {
            flags |= 2;
            std::swap(q1, q2);
        }
    } else {
        if(q1.first > q2.first) {
            flags |= 2;
            std::swap(q1, q2);
        }
    }

    if(is_vertical(p1, p2)) {
        if(is_vertical(q1, q2)) {
            if(p1.first != q1.first) {
                // No intersection
                return -1;
            }

            if(p1.second > q2.second || q1.second > p2.second) {
                // No intersection
                return -1;
            }

            int low_bound = std::max(p1.second, q1.second);
            int high_bound = std::min(p2.second, q2.second);

            if(low_bound*high_bound > 0) {
                if(abs(low_bound) < abs(high_bound)) {
                    if(flags & 1) {
                        time1 = p2.second - low_bound;
                    }
                    else {
                        time1 = low_bound - p1.second;
                    }
                    if(flags & 2) {
                        time2 = q2.second - low_bound;
                    }
                    else {
                        time2 = low_bound - q1.second;
                    }
                }
                else {
                    if(flags & 1) {
                        time1 = high_bound - p2.second;
                    }
                    else {
                        time1 = p1.second - high_bound;
                    }
                    if(flags & 2) {
                        time2 = high_bound - q2.second;
                    }
                    else {
                        time2 = q1.second - high_bound;
                    }
                }

                return abs(p1.first) + std::min(abs(low_bound), abs(high_bound));
            }

            // Meet up at 0
            if(flags & 1) {
                time1 = abs(p2.second);
            }
            else {
                time1 = abs(p1.second);
            }
            if(flags & 2) {
                time2 = abs(q2.second);
            }
            else {
                time2 = abs(q1.second);
            }
            return abs(p1.first);
        }
        else {
            // One is horizontal, one is vertical.
            if(q1.second < p1.second || q1.second > p2.second) {
                return -1;
            }
            if(p1.first < q1.first || p1.first > q2.first) {
                return -1;
            }

            if(flags & 1) {
                time1 = p2.second - q1.second;
            }
            else {
                time1 = q1.second - p1.second;
            }
            if(flags & 2) {
                time2 = q2.first - p1.first;
            }
            else {
                time2 = p1.first - q1.first;
            }

            return abs(p1.first) + abs(q1.second);
        }
    }
    else {
        twist(p1);
        twist(p2);
        twist(q1);
        twist(q2);
        return intersect(p1, p2, q1, q2, time1, time2, flags | 4);
    }

}

int main(int argc, char ** argv) {
    FILE * f = fopen(argv[1], "r");

    std::vector<std::pair<int, int>> t1, t2;
    t1.push_back(std::pair<int, int>(0,0));
    t2.push_back(std::pair<int, int>(0,0));
    read_track(t1, f);
    read_track(t2, f);

    printf("T1: %i, T2: %i\n", t1.size(), t2.size());

    uint32_t best = 1e9;
    uint32_t best2 = 1e9;
    uint32_t dist1 = abs(t1[0].first + t1[0].second);
    for(uint32_t i=1; i<t1.size(); i++) {
        uint32_t dist2 = abs(t2[0].first + t2[0].second);
        for(uint32_t j=1; j<t2.size(); j++) {
            uint32_t time1, time2;
            int temp = intersect(t1[i-1], t1[i], t2[j-1], t2[j], time1, time2);
            if(temp > 0) {
                if(dist1 + time1 + dist2 + time2 < best2) {
                    best2 = dist1 + time1 + dist2 + time2;
                    printf("BEST 2 update: %i, %i, (%i, %i) (%i, %i), (%i, %i), (%i, %i) -> %i %i %i %i %i\n", i, j,
                           t1[i-1].first, t1[i-1].second, t1[i].first, t1[i].second,
                           t2[j-1].first, t2[j-1].second, t2[j].first, t2[j].second,
                           best2, dist1, time1, dist2, time2
                    );
                }
                if(temp < best) {
                    printf("BEST 1 update: %i, %i, (%i, %i) (%i, %i), (%i, %i), (%i, %i) -> %i\n", i, j,
                            t1[i-1].first, t1[i-1].second, t1[i].first, t1[i].second,
                            t2[j-1].first, t2[j-1].second, t2[j].first, t2[j].second,
                            temp
                            );
                    best = temp;
                }
            }

            dist2 += abs(t2[j].first - t2[j-1].first) + abs(t2[j].second - t2[j-1].second);
        }
        dist1 += abs(t1[i].first - t1[i-1].first) + abs(t1[i].second - t1[i-1].second);
    }

    printf("Best is %i\n", best);
    printf("Best2 is %i\n", best2);


    return 0;
}