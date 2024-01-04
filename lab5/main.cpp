#include <iostream>
#include "utils.hpp"

int main() {
    std::string grammarPath("grammar");
    auto originalGrammar = getGrammar(grammarPath);
    auto withoutEpsGrammar = deleteEpsRules(originalGrammar);
    withoutEpsGrammar.constructFIRST();
    withoutEpsGrammar.constructFOLLOW();
    for(const auto& f:withoutEpsGrammar.FIRST){
        std::cout << f.first<<": {";
        for (const auto& let: f.second){
            std::cout << let <<" ";
        }
        std::cout <<"}\n";
    }

    for(const auto& f:withoutEpsGrammar.FOLLOW){
        std::cout << f.first<<": {";
        for (const auto& let: f.second){
            std::cout << let <<" ";
        }
        std::cout <<"}\n";
    }
    withoutEpsGrammar.makeParsingTable();
    printGrammar(withoutEpsGrammar);
    for(const auto& f:withoutEpsGrammar.parsingTable){
        std::cout << f.first<<": {\n";
        for (const auto& let: f.second){
            std::cout << let.first<<": {";
            for (const auto& l: let.second){
                for(const auto& r: l)
                    std::cout << r;
                std::cout << ", ";
            }
            std::cout <<"}\n";
        }
        std::cout <<"}\n";
    }

    return 0;
}
