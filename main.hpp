#include <iostream>
#include <fstream>
#include <ostream>
#include <unordered_set>
#include <unordered_map>
#include <regex>
#include <jpcre2.hpp>
#include "nlohmann/json.hpp"
#include <thread>

#define URI 1


using namespace nlohmann;

//void check(const std::regex& regex, std::string str, ){
//
//}
int getmid() {
    std::string s;
    std::string regex;
    std::ifstream f("regexes_stackoverflow_data_internetSources-stackoverflow.json");
#if URI == 1
    std::vector<std::pair<std::string, std::string>> unique_regex;
#else
    std::unordered_set<std::string> unique_regex;
#endif
    std::regex backref(
            R"(((.*\(.*\).*(\\\\|\\)[1-9].*)|(.*\(\?<(.*)>.*\).*\\k<\5>.*)|(.*\(\?P<(.*)>.*\).*\(\?P=\7\).*)|(.*\(\?<(.*)>.*\).*\\g\{\9\}.*)|(.*\(.*\).*\\g\{-[1-9]\}.*)|.*\(.*\).*\\g\{-[1-9]\}.*))");
    std::regex find_regex(R"((r|\s|R|@|=|E|\()(["'’])(.*?)\2)");
#if URI == 1
    basic_json uri;
#endif
    while (getline(f, s)) {
        json data = json::parse(s);
        auto patterns = data["patterns"];
#if URI == 1
        uri = data["uri"];
#endif
        for (const auto &pattern: patterns) {
            regex = std::string(pattern);
            if (regex.length() > 250) {
                continue;
            }
            if (std::regex_match(regex, backref))
#if URI == 1
                unique_regex.emplace_back(std::string(pattern), pattern);
#else
            unique_regex.insert(pattern);
#endif

        }
    }
    f.close();
    std::sregex_token_iterator end;
    std::string part;
#if URI == 1
    std::vector<std::pair<std::string, std::string>> separated_regex;
    for (auto r: unique_regex) {
        std::sregex_token_iterator it(r.second.begin(), r.second.end(), find_regex, 0);
        if (it == end) {
            separated_regex.emplace_back(r.first, r.second);
        }
        while (it != end) {
            part = it->str();
            part = part.substr(1, part.length());
            if (std::regex_match(std::string(part), backref))
                separated_regex.emplace_back(r.first, part);
            it++;
        }
    }
#else
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
#endif
    std::ofstream file("middle.txt");
#if URI == 1
    std::ofstream uris("uris.txt");
    for (const auto &r: separated_regex) {
        file << r.second << '\n';
        uris << r.first << '\n';
    }
#else
    for (const std::string &r: separated_regex) {
    file << r << '\n';
}
#endif
    return 0;
}
