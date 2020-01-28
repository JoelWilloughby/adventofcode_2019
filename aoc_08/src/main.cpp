#include <cstdio>
#include <vector>

class Image {
public:
    void read_file(FILE* f) {

        bool done = false;
        while(!done) {
            int* layer = new int[25*6];
            for (int i = 0; i < 25*6; i++) {
                char c;
                if (fscanf(f, "%c", &c) != 1) {
                    delete [] layer;
                    return;
                }
                layer[i] = c - '0';
            }
            layers.push_back(layer);
        }
    }

    void decode(int * result) {
        for(int i=0; i<25*6; i++) {
            for(int j=0; j<layers.size(); j++) {
                if(layers[j][i] == 2) {
                    continue;
                }
                result[i] = layers[j][i];
                break;
            }
        }
    }

    std::vector<int*> layers;
};

int main(int argc, char ** argv) {
    FILE * file = fopen(argv[1], "r");

    Image image;
    image.read_file(file);

    printf("Num layers: %i\n", image.layers.size());

    int num_zeros = 1000;
    int best_val = 0;
    for(int i=0; i<image.layers.size(); i++) {
        int counts[10] = {0};
        for(int j=0; j<6; j++) {
            for(int k=0; k<25; k++) {
                counts[image.layers[i][j*25+k]]++;
            }
        }
        if(counts[0] < num_zeros) {
            best_val = counts[1] * counts[2];
            num_zeros = counts[0];
        }
    }

    printf("Best val is %i\n", best_val);

    int res[6][25];

    image.decode(&res[0][0]);

    for(int i=0; i<6; i++) {
        for(int j=0; j<25; j++) {
            printf("%c", res[i][j] ? 'X' : ' ');
        }
        printf("\n");
    }

}