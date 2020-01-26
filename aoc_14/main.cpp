#include <cstdio>
#include <string>
#include <vector>
#include <cstring>
#include <cctype>
#include <map>
#include <set>

struct Resource {
    Resource(const std::string& name, unsigned count): name(name), count(count) {

    }

    std::string name;
    unsigned count;
};

struct Rule {
    Rule(): output("", 0) {

    }

    std::vector<Resource> inputs;
    Resource output;
};

Rule* read_rule(FILE* file) {
    char c;
    while (std::isspace(c = fgetc(file))) {
    }

    if(c == EOF) {
        return NULL;
    }

    ungetc(c, file);
    std::vector<Resource> resources;

    char buf[256];
    fgets(buf, sizeof(buf), file);

    char * tok;
    char * ptr;
    tok = strtok_r(buf, " ,=>\n", &ptr);
    while(tok != NULL) {
        unsigned count;
        sscanf(tok, "%i", &count);
        tok = strtok_r(NULL, " ,=>\n", &ptr);
        resources.push_back(Resource(std::string(tok), count));
        tok = strtok_r(NULL, " ,=>\n", &ptr);
    }


    Rule * rule = new Rule();
    for(int i=0; i<resources.size() - 1; i++) {
        rule->inputs.push_back(resources[i]);
    }

    rule->output = resources[resources.size() -1];

    return rule;
}

static std::map<std::string, unsigned> produced;
unsigned total = 0;
void search_it(const std::map<std::string, Rule*>& rules, const std::string& target, unsigned target_count) {
    if(produced[target] >= target_count) {
        produced[target] -= target_count;
        return;
    }

    target_count -= produced[target];
    produced[target] = 0;

    if(target == "ORE") {
        total += target_count;
        return;
    }

    unsigned num_needed = 1 + (target_count - 1) / rules.at(target)->output.count;

    for(auto input = rules.at(target)->inputs.begin(); input != rules.at(target)->inputs.end(); input++) {
        search_it(rules, input->name, num_needed * input->count);
    }

    produced[target] += num_needed * rules.at(target)->output.count - target_count;
}

int main(int argc, char ** argv) {
    FILE * file = fopen(argv[1], "r");

    std::map<std::string, Rule*> rules;
    Rule * rule;
    while((rule = read_rule(file)) != NULL) {
        if(rules.find(rule->output.name) != rules.end()) {
            fprintf(stderr, "Duplicate output found\n");
            exit(1);
        }
        rules[rule->output.name] = rule;
    }

    for(auto iter = rules.begin(); iter != rules.end(); iter++) {
        printf("%i %s <= ", iter->second->output.count, iter->second->output.name.c_str());
        for(auto in_iter = iter->second->inputs.begin(); in_iter != iter->second->inputs.end(); in_iter++) {
            printf("%i %s, ", in_iter->count, in_iter->name.c_str());
        }
        printf("\n");
    }

    search_it(rules, "FUEL", 1);
    printf("Need %i ore\n", total);
}