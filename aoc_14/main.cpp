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

static std::map<std::string, unsigned long long> produced;
unsigned long long total = 0;
bool search_it(const std::map<std::string, Rule*>& rules, const std::string& target, unsigned long long target_count) {
    if(produced[target] >= target_count) {
        produced[target] -= target_count;
        return true;
    }

    target_count -= produced[target];
    produced[target] = 0;

    if(target == "ORE") {
        total += target_count;
        return false;
    }

    unsigned long long num_needed = 1 + (target_count - 1) / rules.at(target)->output.count;

    bool can_produce = true;
    for(auto input = rules.at(target)->inputs.begin(); input != rules.at(target)->inputs.end(); input++) {
        if(!search_it(rules, input->name, num_needed * input->count)) {
            can_produce = false;
        }
    }

    produced[target] += num_needed * rules.at(target)->output.count - target_count;

    return can_produce;
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
    unsigned temp = total;

    produced.clear();

    produced["ORE"] = 1000000000000;
    unsigned long long count = 0;
    auto back_produced = produced;
    unsigned long long target = 1;
    bool done = false;
    while(!done) {
        if(search_it(rules, "FUEL", target)) {
            printf("Made %lli FUEL\n", target);
            count += target;
            target <<= 1;
            back_produced = produced;
        }
        else {
            target >>= 1;
            produced = back_produced;
        }
        if(target == 0) {
            done = true;
        }
    }

    printf("To produce 1 fuel, need %i. Num FUEL produced is %lli. ORE remaining is: %lli\n", temp, count, produced["ORE"]);
}