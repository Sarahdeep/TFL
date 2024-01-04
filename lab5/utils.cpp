#include <fstream>
#include "utils.hpp"
#include "GraphStack.hpp"
void optimizeGrammar(Grammar &grammar){
    for (auto& item: grammar.grammar){
        for(auto it =item.second.begin(); it!=item.second.end();){
            if(std::vector<std::string>(1, item.first) ==*it){
                it = item.second.erase(it);
            } else{
                it++;
            }
        }
    }
}

void printGrammar(Grammar &grammar) {
    for (const auto &rule: grammar.grammar) {
        std::cout << rule.first << " -> ";
        size_t i = rule.second.size();
        for (const auto &r: rule.second) {
            i--;
            for (const auto &elem: r) {
                std::cout << elem;
            }
            if (i != 0)
                std::cout << '|';
        }
        std::cout << std::endl;
    }
}

std::set<std::string> getEpsRules(Grammar &grammar) {
    std::set<std::string> ans;
    for (const auto &rules: grammar.grammar) {
        if (rules.second.contains(std::vector<std::string>(1, "e"))) {
            ans.insert(rules.first);
            grammar.grammar[rules.first].erase(std::vector<std::string>(1, "e"));
        }
    }
    bool changed = true;
    while (changed) {
        size_t len = ans.size();
        for (const auto &rules: grammar.grammar) {
            for (const auto &v_rules: rules.second) {
                bool contains = true;
                for (const auto &letter: v_rules) {
                    if (!ans.contains(letter)) {
                        contains = false;
                    }
                }
                if (contains) {
                    ans.insert(rules.first);
                }
            }
        }
        if (len == ans.size()) {
            changed = false;
        }
    }
    return ans;
}
std::vector<int> getIndex(std::string &let, std::vector<std::string> &rules) {
    std::vector<int> ans;
    for (int i = 0; i < rules.size(); i++) {
        if (let == rules[i]) {
            ans.push_back(i);
        }
    }
    return ans;
}

void getSubsets(std::vector<int> &idxes, std::vector<std::vector<int>> &subsets, std::vector<int> &subset, int idx) {
    subsets.push_back(subset);
    for (int i = idx; i < idxes.size(); i++) {
        subset.push_back(idxes[i]);
        getSubsets(idxes, subsets, subset, i + 1);
        subset.pop_back();
    }
}

std::vector<std::string> getNewRule(std::vector<int> &idx, std::vector<std::string> &rule) {
    std::vector<std::string> newRule = rule;
    for (int i: idx) {
        newRule.erase(newRule.begin() + i);
    }
    return newRule;
}

Grammar deleteEpsRules(Grammar &grammar) {
    Grammar new_grammar;
    for (auto rule: grammar.grammar) {
        new_grammar.grammar.insert(rule);
    }
    std::set<std::string> eps_rules = getEpsRules(new_grammar);
    for (auto rule: eps_rules) {
        for (auto rules: new_grammar.grammar) {
            for (auto r: rules.second) {
                std::vector<int> idxes = getIndex(rule, r);
                std::vector<std::vector<int>> subsets;
                std::vector<int> subset;
                int idx = 0;
                getSubsets(idxes, subsets, subset, idx);
                for (auto sub: subsets) {
                    std::vector<std::string> newRule;
                    if (!sub.empty()) {
                        newRule = getNewRule(sub, r);
                        if (!newRule.empty())
                            new_grammar.grammar[rules.first].insert(newRule);
                    }
                }
            }
        }
    }
    if (eps_rules.contains(grammar.ZeroState)) {
        new_grammar.ZeroState = grammar.ZeroState + "'";
        std::vector<std::string> s = {grammar.ZeroState};
        std::vector<std::string> e = {"e"};
        new_grammar.grammar[new_grammar.ZeroState].insert(s);
        new_grammar.grammar[new_grammar.ZeroState].insert(e);
    }else{
        new_grammar.ZeroState = grammar.ZeroState;
    }
    return new_grammar;
}
void splitRight(std::set<std::vector<std::string>> &rule, std::string &right) {
    size_t pos;
    std::string token;
    while ((pos = right.find('|')) != std::string::npos) {
        token = right.substr(0, pos);
        std::vector<std::string> v;
        for (auto letter: token) {
            v.emplace_back(1, letter);
        }
        rule.insert(v);
        right.erase(0, pos + 1);
    }
    std::vector<std::string> v;
    for (auto letter: right) {
        v.emplace_back(1, letter);
    }
    rule.insert(v);
}

Grammar getGrammar(const std::string &grammarPath) {
    std::ifstream grammar(grammarPath);
    std::string rule;
    grammar_t ans;
    while (std::getline(grammar, rule)) {
        rule.erase(std::remove_if(rule.begin(), rule.end(), ::isspace),
                   rule.end());
        std::string left = rule.substr(0, rule.find("->"));
        std::string right = rule.substr(rule.find("->") + 2, rule.length() - 1);
        splitRight(ans[left], right);
    }

    return {"S", ans};
}



std::set<char> Grammar::getAlphabet() {
    std::set<char> alphabet;
    for(const auto& rules:grammar){
        for(const auto& rule: rules.second){
            for(auto elem: rule){
                if (islower(elem[0]) && elem[0] != 'e'){
                    alphabet.insert(elem[0]);
                }
            }
        }
    }
    return alphabet;
}
bool containEps(Grammar& grammar){
    std::vector<std::string> e = {"e"};
    return grammar.grammar[grammar.ZeroState].contains(e);
}


std::set<std::string> getFirst(first_t& FIRST, std::vector<std::string>& rule){
    std::set<std::string> ans;
    if (rule[0].length()>1 || islower(rule[0][0])){
        ans.insert(rule[0]);
    } else{
        ans.insert(FIRST[rule[0]].begin(), FIRST[rule[0]].end());
    }
    return ans;
}

std::set<std::string> getFirstEps(first_t& FIRST, Grammar& grammar, std::vector<std::string>& rule){
    std::set<std::string> ans;
    if (!(rule[0].length()==1 && isupper(rule[0][0]))){
        ans.insert(rule[0]);
    } else{
        if(rule[0] == grammar.ZeroState && containEps(grammar))
        {
            ans.insert(FIRST[rule[0]].begin(), FIRST[rule[0]].end());
            std::vector<std::string> tmp(rule.begin()+1, rule.end());
            auto res = getFirstEps(FIRST, grammar, tmp);
            ans.insert(res.begin(), res.end());
        }else {
            ans.insert(FIRST[rule[0]].begin(), FIRST[rule[0]].end());
        }
    }
    return ans;
}
void Grammar::constructFIRST() {
    bool changed = true;
    for (const auto& rules: grammar){
        FIRST[rules.first] = std::set<std::string>();
    }
    std::vector<std::string> e = {"e"};
    if(grammar[ZeroState].contains(e)){
        FIRST[ZeroState].insert("e");
    }
    while (changed) {
        changed = false;
        for (const auto& rules: grammar){
            for(auto rule: rules.second){
                auto tmp = getFirstEps(FIRST,*this, rule);
                size_t len = FIRST[rules.first].size();
                FIRST[rules.first].insert(tmp.begin(), tmp.end());
                if(FIRST[rules.first].size() > len){
                    changed = true;
                }
            }
        }
    }
}

void Grammar::constructFOLLOW() {
    bool changed = true;
    bool Eps = containEps(*this);
    for (const auto& rules: grammar){
        FOLLOW[rules.first] = std::set<std::string>();
    }
    FOLLOW[ZeroState] = {"$"};
    while (changed){
        changed = false;
        for (const auto& rules: grammar){
            for(auto rule: rules.second){
                for( int i = 0; i < rule.size(); i++){
                    if(!(rule[0].length()==1 && isupper(rule[0][0])))
                        continue;
                    size_t len = FOLLOW[rule[i]].size();
                    if(i + 1 < rule.size()){
                        std::vector<std::string> tmp(rule.begin()+i+1, rule.end());
                        auto first = getFirstEps(FIRST, *this, tmp);
                        first.erase("e");
                        FOLLOW[rule[i]].insert(first.begin(), first.end());

                    }
                    if(i + 1 == rule.size() || (Eps && rule[i] == ZeroState)){
                        FOLLOW[rule[i]].insert(FOLLOW[rules.first].begin(), FOLLOW[rules.first].end());
                    }
                    if(FOLLOW[rule[i]].size() > len){
                        changed = true;
                    }
                }
            }
        }
    }
}

void Grammar::makeParsingTable() {
    for(auto rules: grammar){
        for(auto rule: rules.second){
            for(auto let: getFirstEps(FIRST, *this, rule)){
                if (let == "e"){
                    for(auto follow: FOLLOW[rules.first]){
                        std::vector<std::string> e{"e"};
                        parsingTable[rules.first][follow].insert(e);
                        if (follow == "$"){
                            parsingTable[rules.first][follow].insert(e);
                        }
                    }
                }
                parsingTable[rules.first][let].insert(rule);
            }
        }
    }
}
bool isTerm(std::string& s){
    return !(s.length() == 1 && isupper(s[0]));
}

GraphStack initStep(Grammar& grammar){
    GraphStack graphStack(grammar.ZeroState);
    return graphStack;
}
void step(Grammar& grammar, Node* parent, std::string& input, int& pos){
    std::string cur(1, input[pos]);
    if (isTerm(parent->value) && parent->value == cur){
        pos++;
        parent = parent->parent; //pop
    }else if(isTerm(parent->value)){
        throw std::exception();
    }else{
        if(grammar.parsingTable[parent->value].contains(cur)){
            for(auto rule: grammar.parsingTable[parent->value][cur]){
                for(int i = rule.size()-1; i >= 0; i--){
                    //push elems
                }
            }
        }else if(grammar.parsingTable[parent->value].contains("e")){
            parent = parent->parent; //pop
        }else{
            throw std::exception();
        }
    }

}
