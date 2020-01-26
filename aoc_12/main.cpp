#include <cstdio>
#include <cstdlib>
#include <cstring>

struct Point {
    Point(int x, int y, int z): x(x), y(y), z(z) {

    }
    Point(): x(0), y(0), z(0) {
    }

    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y, z + other.z);
    }

    Point operator-(const Point&  other) const {
        return Point(x - other.x, y - other.y, z - other.z);
    }

    unsigned abs() {
        return ::abs(x) + ::abs(y) + ::abs(z);
    }

    int x;
    int y;
    int z;
};

struct Moon {
    Moon(int x, int y, int z): pos(x,y,z), vel() {

    }
    Moon() : Moon(0,0,0) {

    }

    void apply_gravity(const Moon& other) {
        if(pos.x > other.pos.x) {
            vel.x--;
        }
        else if (pos.x < other.pos.x) {
            vel.x++;
        }
        if(pos.y > other.pos.y) {
            vel.y--;
        }
        else if (pos.y < other.pos.y) {
            vel.y++;
        }
        if(pos.z > other.pos.z) {
            vel.z--;
        }
        else if (pos.z < other.pos.z) {
            vel.z++;
        }
    }

    void move() {
        pos = pos + vel;
    }

    unsigned energy() {
        return pos.abs() * vel.abs();
    }

    Point pos;
    Point vel;
};

int main() {
    const size_t NUM_MOONS = 4;
    Moon moons[NUM_MOONS] = {
            Moon(-4, -14, 8),
            Moon(1, -8, 10),
            Moon(-15, 2 ,1),
            Moon(-17, -17, 16),
    };
//    Moon moons[NUM_MOONS] = {
//            Moon(-8, -10, 0),
//            Moon(5, 5, 10),
//            Moon(2, -7, 3),
//            Moon(9, -8, -3),
//    };
    Moon moons_back[NUM_MOONS];
    memcpy(moons_back, moons, sizeof(moons));

    bool found_x = false;
    bool found_y = false;
    bool found_z = false;
    unsigned long long count_x = 0;
    unsigned long long count_y = 0;
    unsigned long long count_z = 0;
    while(!(found_x && found_y && found_z)) {
        for(int i=0; i<NUM_MOONS; i++) {
            for(int j=0; j<NUM_MOONS; j++) {
                // Compute velocities
                moons[i].apply_gravity(moons[j]);
            }
        }

        bool good_x = true;
        bool good_y = true;
        bool good_z = true;
        for(int i=0; i<NUM_MOONS; i++) {
            moons[i].move();
            if(count_x == 0 || moons[i].vel.x != 0 || moons[i].pos.x != moons_back[i].pos.x) {
                good_x = false;
            }
            if(count_y == 0 || moons[i].vel.y != 0 || moons[i].pos.y != moons_back[i].pos.y) {
                good_y = false;
            }
            if(count_z == 0 || moons[i].vel.z != 0 || moons[i].pos.z != moons_back[i].pos.z) {
                good_z = false;
            }
        }
        for(int i=0; i<NUM_MOONS; i++) {
            if(good_x) {
                found_x = true;
            }
            if(good_y) {
                found_y = true;
            }
            if(good_z) {
                found_z = true;
            }
        }
        if(!found_x) {
            count_x++;
        }
        if(!found_y) {
            count_y++;
        }
        if(!found_z) {
            count_z++;
        }
    }

    // Add one to each of these and find the LCM
    printf("X took %i\n", count_x);
    printf("Y took %i\n", count_y);
    printf("Z took %i\n", count_z);

    unsigned energy = 0;
    for(int i=0; i<NUM_MOONS; i++) {
        unsigned temp = moons[i].energy();
        printf("Moon %i has energy %i\n", i, temp);
        energy += temp;
    }
    printf("The resulting energy is: %i\n", energy);
}