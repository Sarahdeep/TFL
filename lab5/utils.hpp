#ifndef LAB5_UTILS_HPP
#define LAB5_UTILS_HPP


#include <iostream>
#include <set>
#include <map>
#include <vector>

namespace Names {
    using grammar_t = std::map<std::string, std::set<std::vector<std::string>>>;
    using state_t = std::tuple<std::string, std::vector<std::string>,size_t, int>;
    using parsing_table_t = std::map<std::string, std::map<std::string,
    std::set<std::vector<std::string>>>>;
    using first_t = std::map<std::string, std::set<std::string>>;
    using follow_t = first_t;
}

using namespace Names;
class Grammar{
public:
    std::string ZeroState;
    grammar_t grammar;
    first_t FIRST;
    follow_t FOLLOW;
    parsing_table_t parsingTable;
    std::set<char> getAlphabet();
    void constructFIRST();
    void constructFOLLOW();
    void makeParsingTable();

};
Grammar getGrammar(const std::string &grammar);
void printGrammar(Grammar &grammar);
Grammar deleteEpsRules(Grammar &grammar);
#endif //LAB5_UTILS_HPP
