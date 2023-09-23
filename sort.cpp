#include <iostream>
#include <fstream>
#include <ostream>
#include <unordered_set>
#include <regex>
#include "filter.hpp"
#define URI 1


int count_slash(const std::string &expr, int index) {
    int ans = 0;
    while (index >= 0 && expr[index] == '\\') {
        ans++;
        index--;
    }
    return ans;
}

std::unordered_map<int, std::pair<int, int>> find_brackets_unnamed(std::string &str) {
    std::unordered_map<int, std::pair<int, int>> ans;
    std::stack<std::pair<int,int>> start;
    int k=0;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '(' && (i == 0 || count_slash(str, i - 1) % 2 == 0)) {
            start.emplace(k++,i);
        }
        if (str[i] == ')' && (i == 0 || count_slash(str, i - 1) % 2 == 0)) {
            ans[start.top().first] = std::pair<int, int>(start.top().second, i);
            start.pop();
        }
    }
    return ans;
}

std::string find_key(std::string &expr) {
    std::string ans;
    if (expr.length() < 2 || (expr[0] != '?' && expr[1] != '<'))
        return "";
    int i = 2;
    while ((expr[i] != '>' || count_slash(expr, i - 1) % 2 == 1) && i < expr.length()) {
        ans += expr[i];
        i++;
    }
    return ans;
}

std::vector<std::pair<int, std::pair<int, int>>> find_all_backrefs_unnamed(std::string &expr) {
    std::vector<std::pair<int, std::pair<int, int>>> ans;
    for (int i = 0; i < expr.length(); i++) {
        if (i + 1 < expr.length() && expr[i] == '\\' && expr[i + 1] >= '0' && expr[i + 1] <= '9')
            ans.emplace_back(expr[i + 1]-'0', std::pair<int, int>(i, i + 1));
        else if (i + 2 < expr.length() && expr[i] == '\\' && expr[i + 1] == '\\' && expr[i + 2] >= '0' &&
                 expr[i + 2] <= '9')
            ans.emplace_back(expr[i + 2]-'0', std::pair<int, int>(i + 1, i + 2));
    }
    return ans;
}

std::vector<std::pair<std::string, std::pair<int, int>>> find_all_backrefs_named(std::string &expr) {
    std::vector<std::pair<std::string, std::pair<int, int>>> ans;
    int start;
    for (int i = 0; i + 5 < expr.length(); i++) {
        while (i + 5 < expr.length() && expr.substr(i, 4) != R"(\\g{)")
            i++;
        start = i;
        if(i+5>=expr.length()){
            return ans;
        }
        i += 4;
        std::string key;
        while (expr[i] != '}') {
            key += expr[i];
            i++;
        }
        ans.emplace_back(key, std::pair<int, int>(start, i));
    }
    return ans;
}

std::unordered_map<std::string, std::pair<int, int>> find_brackets_named(const std::string &str) {
    std::unordered_map<std::string, std::pair<int, int>> map;
    std::stack<int> start;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '(' && (i == 0 || count_slash(str, i - 1) % 2 == 0)) {
            start.push(i);
        }
        if (str[i] == ')' && (i == 0 || count_slash(str, i - 1) % 2 == 0)) {
            std::string word = (str.substr(start.top() + 1, i - start.top()));
            word = find_key(word);
            if (word.length() > 0) {
                map[word] = std::pair<int, int>(start.top(), i);
            }
            start.pop();
        }
    }
    return map;
}

bool check_iter(const std::string &expr) {
    int i = 0;
    while (i < expr.length() && expr[i] != '*' && expr[i] != '+') {
        i++;
    }
    if (i == expr.length()){
        return false;
    }
    if (i == 0 || count_slash(expr, i - 1) % 2 == 0)
        return true;
    return false;
}

bool is_4_class(std::string &expr) {
    auto unnamed_coords = find_brackets_unnamed(expr);
    auto named_coords = find_brackets_named(expr);
    auto unnamed_refs = find_all_backrefs_unnamed(expr);
    auto named_refs = find_all_backrefs_named(expr);
    std::regex reg(R"(\(.*(((\\|\\\\)[1-9])|(\\g\{.*\})).*\))");
    for (const auto &ref: named_refs) {
        if (named_coords.count(ref.first)) {
            auto brackets = named_coords[ref.first];
            std::string check = expr.substr(brackets.first, brackets.second - brackets.first + 1);
            if (std::regex_match(check, reg)) {
                return true;
            }
        }
    }
    for (const auto &ref: unnamed_refs) {
        if (unnamed_coords.size() > ref.first-1) {
            auto brackets = unnamed_coords[ref.first-1];
            std::string check = expr.substr(brackets.first, brackets.second - brackets.first + 1);
            if (std::regex_match(check, reg)) {
                return true;
            }
        }
    }
    return false;
}

bool is_3_class(std::string &expr) {
    auto unnamed_coords = find_brackets_unnamed(expr);
    auto named_coords = find_brackets_named(expr);
    auto unnamed_refs = find_all_backrefs_unnamed(expr);
    auto named_refs = find_all_backrefs_named(expr);
    std::vector<int> candidates;
    for (auto coord1: unnamed_coords) {
        auto coord = coord1.second;
        for (auto ref: unnamed_refs) {
            if (ref.second.first > coord.first && ref.second.second < coord.second)
                candidates.push_back(coord.second);
        }
        for (const auto &ref: named_refs) {
            if (ref.second.first > coord.first && ref.second.second < coord.second)
                candidates.push_back(coord.second);
        }
    }
    for (int end: candidates) {
        if (end + 1 < expr.length() && (expr[end + 1] == '*' || expr[end + 1] == '+'))
            return true;
    }
    return false;
}

bool is_2_class(std::string &expr) {
    auto unnamed_coords = find_brackets_unnamed(expr);
    auto named_coords = find_brackets_named(expr);
    auto unnamed_refs = find_all_backrefs_unnamed(expr);
    auto named_refs = find_all_backrefs_named(expr);
    std::string substr;
    for (auto coord1: unnamed_coords) {
        auto coord = coord1.second;
        substr = expr.substr(coord.first, coord.second - coord.first + 1);
        if (check_iter(substr))
            return true;
    }
    return false;
}

int main() {
    filter();
    std::ifstream f("back_ref.txt");
    std::ofstream class1("first_class.csv");
    std::ofstream class2("second_class.csv");
    std::ofstream class3("third_class.csv");
    std::ofstream class4("fourth_class.csv");
#if URI == 1
    std::ifstream pat("uris2.txt");
    std::string str;
    std::ofstream image("im.txt");
#endif
    std::string s;
    while (getline(f, s)) {
#if URI == 1
        getline(pat, str);
#endif
        if (is_4_class(s)) {
            class4 << s << std::endl;
#if URI == 1
            image<< str <<'\t' << 4 <<'\n';
#endif
        } else if (is_3_class(s)) {
            class3 << s << std::endl;
#if URI == 1
            image<< str <<'\t' << 3 <<'\n';
#endif
        } else if (is_2_class(s)) {
            class2 << s << std::endl;
#if URI == 1
            image<< str <<'\t' << 2 <<'\n';
#endif
        } else {
            class1 << s << std::endl;
#if URI == 1
            image<< str <<'\t' << 1 <<'\n';
#endif
        }
    }
    f.close();
    class1.close();
    class2.close();
    class3.close();
    class4.close();
#if URI == 1
    pat.close();
    image.close();
#endif
    return 0;
}