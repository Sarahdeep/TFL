#include <iostream>
#include <fstream>
#include <ostream>
#include <unordered_set>
#include <regex>
#include <jpcre2.hpp>
#include "nlohmann/json.hpp"
#include <thread>
/*
 1. Remove duplicates
 2. Delete without backrefs pattern
 3. Delete command options

*/
typedef jpcre2::select<char> jp;
using namespace nlohmann;
//void check(const std::regex& regex, std::string str, ){
//
//}
int getmid() {
    std::string s;
    std::string regex;
    std::ifstream f("regexes_stackoverflow_data_internetSources-stackoverflow.json");
    std::unordered_set<std::string> unique_regex;
    //std::regex reg(R"(((\\|\\\|$)[1-9]|\\k<.*>|\\g\{.*\}|\?P=))");
    std::regex backref(
            R"(((.*\(.*\).*(\\\\|\\)[1-9].*)|(.*\(\?<(.*)>.*\).*\\k<\5>.*)|(.*\(\?P<(.*)>.*\).*\(\?P=\7\).*)|(.*\(\?<(.*)>.*\).*\\g\{\9\}.*)|(.*\(.*\).*\\g\{-[1-9]\}.*)|.*\(.*\).*\\g\{-[1-9]\}.*))");
    //jp::Regex re;
    //re.setPattern(R"(((.*\(.*\).*(\\|\\\)[1-9].*)|(.*\(\?<(.*)>.*\).*\\k<\5>.*)|(.*\(\?P<(?<track2>.*)>.*\).*\(\?P=\k<track2>\).*)|(.*\(\?<(?<track3>.*)>.*\).*\\g\{\k<track3>\}.*)))");
    std::regex find_regex(R"((r|\s|R|@|=|E|\()(["'’])(.*?)\2)");
    int i =0;
    while (getline(f, s)) {
        json data = json::parse(s);
        auto patterns = data["patterns"];
        for (const auto &pattern: patterns) {
            regex = std::string(pattern);
            if(regex.length()>250){
                continue;
            }
            if (std::regex_match(regex, backref))
                unique_regex.insert(pattern);

        }
        // if(re.match(std::string(pattern)))
        //    std::cout << pattern;
        //}
    }
    f.close();
    std::cout << unique_regex.size();
    std::sregex_token_iterator end;
    std::string part;
    std::unordered_set<std::string> separated_regex;
    for (const std::string &r: unique_regex) {
        std::sregex_token_iterator it(r.begin(), r.end(), find_regex, 0);
        if (it == end) {
            separated_regex.insert(r);
        }
        while (it != end) {
            part = it->str();
            part = part.substr(1, part.length());
            if (std::regex_match(std::string(part), backref))
                separated_regex.insert(part);
            it++;
        }

    }
    std::ofstream file("middle.txt");
    for (const std::string &r: separated_regex) {
        file << r << '\n';
    }
    return 0;
}
