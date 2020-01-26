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

static std::map<std::string, std::pair<unsigned, unsigned>> best;
unsigned search_it(const std::map<std::string, Rule*>& rules, const std::string& target, unsigned target_count, unsigned& extra) {
    printf("Searching for %s qty %i\n", target.c_str(), target_count);

    if(target == "ORE") {
        printf("Reached base %i\n", target_count);
        return target_count;
    }

    auto found = best.find(target);
    unsigned ore_count, need_count, generated_count;
    if(found != best.end()) {
        ore_count = found->second.first;
        generated_count = found->second.second;
        need_count = ore_count * (1 + (target_count  / generated_count));
    }
    else {
        ore_count = 0;
        for(auto input = rules.at(target)->inputs.begin(); input != rules.at(target)->inputs.end(); input++) {
            ore_count += search_it(rules, input->name, input->count, extra);
        }
        generated_count = rules.at(target)->output.count;
        need_count = ore_count * (1 + (target_count  / generated_count));
        // Log this for later
        best[target] = std::pair<unsigned, unsigned>(ore_count, generated_count);
    }

    printf("To make %i %s, need %i ORE\n", generated_count, target.c_str(), ore_count);
    printf("To make %i %s, need %i ORE, have %i extra\n", target_count, target.c_str(), need_count, extra);

    if(extra <= need_count) {
        // Extra not quite enough to cover
        need_count -= extra;
        extra = 0;
    }
    else {
        extra -= need_count;
        need_count = 0;
    }

    if(need_count > ore_count) {
        extra += need_count % ore_count;
    }
    return need_count;
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

    unsigned extra = 0;
    unsigned count = search_it(rules, "FUEL", 1, extra);
    printf("Need %i ore\n", count);
}