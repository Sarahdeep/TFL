#include <iostream>
#include <fstream>
#include <ostream>
#include <unordered_set>
#include <regex>
#include "main.hpp"
#define URI 1

int count_slash(std::string &expr, int index) {
    int ans = 0;
    while (index >= 0 && expr[index] == '\\') {
        ans++;
        index--;
    }
    return ans;
}

int count_brackets(std::string &expr) {
    int res = 0;
    for (int i = 0; i < expr.length(); i++) {
        if (expr[i] == '(' && (i == 0 || count_slash(expr, i - 1) % 2 == 0)) {
            res++;
        }
    }
    return res;
}

void delete_brackets(std::string &expr) {
    std::stack<char> brackets;
    for (int i = 0; i < expr.length(); i++) {
        if (expr[i] != '(' && expr[i] != ')' && expr[i] != '[' && expr[i] != ']')
            continue;
        if (!brackets.empty() && (expr[i] == '(' || expr[i] == ')')) {
            expr.erase(expr.begin() + i);
            i -= 1;
            continue;
        }
        if (brackets.empty()) {
            if (expr[i] == '[' && (i == 0 || count_slash(expr, i - 1) % 2 == 0))
                brackets.push(expr[i]);
        } else if (brackets.top() == '[' && expr[i] == ']') {
            if (count_slash(expr, i - 1) % 2 == 0)
                brackets.pop();
        } else if (expr[i] == '[' && (i == 0 || count_slash(expr, i - 1) % 2 == 0)) {
            brackets.push(expr[i]);
        }
    }

}

void check_balance(std::string &expr) {
    std::stack<std::pair<char, int>> temp;
    std::stack<int> del;

    bool balanced = false;
    for (int i = 0; i < expr.length(); i++) {
        if (expr[i] != '(' && expr[i] != ')' && expr[i] != '[' && expr[i] != ']' && expr[i] != '{' && expr[i] != '}')
            continue;
        if (expr[i] == '(' && (i == 0 || count_slash(expr, i - 1) % 2 == 0)) {
            if (i + 2 < expr.length() && expr[i + 1] == '?' && expr[i + 2] == ':') {
                del.push(i);
            }
        }
        if (temp.empty()) {

            if (i == 0 || count_slash(expr, i - 1) % 2 == 0)
                temp.emplace(expr[i], i);
        } else if ((temp.top().first == '(' && expr[i] == ')')
                   || (temp.top().first == '{' && expr[i] == '}')
                   || (temp.top().first == '[' && expr[i] == ']')) {

            if (!del.empty() && temp.top().second == del.top() && count_slash(expr, i - 1) % 2 == 0) {
                expr.erase(del.top(), 3);
                del.pop();
                i -= 3;
                expr.erase(expr.begin() + i);
                i -= 1;
            }
            if (count_slash(expr, i - 1) % 2 == 0)
                temp.pop();
        } else {
            if (i == 0 || count_slash(expr, i - 1) % 2 == 0)
                temp.emplace(expr[i], i);
        }
    }
    if (temp.empty()) {

        // If stack is empty return true
        balanced = true;
    }

    if (!balanced)
        expr = "";

}

int filter() {
    getmid();
    std::string s;
    std::string repl;
    std::ifstream f("middle.txt");
    std::regex rows(R"((['"])(.*)\1)");
    std::regex neg_num(R"(.*\(.*\).*\\g\{(-[1-9])\}.*)");
    std::regex neg_num2(R"(\\g\{(-[1-9])\})");
    std::regex stopwords(
            R"((egrep|grep|sed|cat|replace|(Validemail = )|file|(perl -pe)|FIELD=|with|find|(perl -0777 -pe)|(perl -ne)|(perl -lpe)|\?=|\?!|\?<=|\?<!|\[\]))");
    std::regex neg(R"((\[\^))");
    std::regex lazy_q(R"(([\*\+])\?)");
    std::regex lazy_q2(R"((\{\d,\})\?)");
    std::regex jel_q(R"(([\*\+\?])\+)");
    std::regex jel_q2(R"((\{\d,\})\+)");
    std::regex JS(R"((.*)\(\?<(.*)>(.*)\)(.*)\\k<\2>(.*))");
    std::regex py(R"((.*)\(\?P<(.*)>(.*)\)(.*)\(\?P=\2\)(.*))");
    std::regex py_dirty_hack(R"((.*)\(\?<(.*)>(.*)\)(.*)\(\?P=\2\)(.*))");
    std::regex py_tail(R"(\?P<)");
    std::regex reg;
    std::regex upper(R"([:upper:])");
    std::regex lower(R"([:lower:])");
    std::regex alpha(R"([:lower:])");
    std::regex digit(R"([:digit:])");
    std::regex xdigit(R"([:xdigit:])");
    std::regex alnum(R"([:alnum:])");
    std::regex word(R"([:word:])");
    std::regex punct(R"([:punct:])");
    std::regex blank(R"([:blank:])");
    std::regex space(R"([:space:])");
    std::regex cntrl(R"([:cntrl:])");
    std::regex graph(R"([:graph:])");
    std::regex print(R"([:print:])");

    std::smatch match;
    int num = 0;
#if URI == 1
    std::string uri;
    std::ifstream uri_file("uris.txt");
    std::vector<std::pair<std::string,std::string>> regular_exps;
#elif
    std::unordered_set<std::string> regular_exps;
#endif
    while (getline(f, s)) {
#if  URI == 1
        getline(uri_file, uri);
#endif
        s = std::regex_replace(s, stopwords, "");
        s = std::regex_replace(s, neg, "[");
        s = std::regex_replace(s, lazy_q, "$1");
        s = std::regex_replace(s, jel_q, "$1");
        s = std::regex_replace(s, lazy_q2, "$1");
        s = std::regex_replace(s, jel_q2, "$1");
        s = std::regex_replace(s, JS, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, JS, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, JS, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, JS, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, JS, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, JS, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py_dirty_hack, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py_dirty_hack, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py_dirty_hack, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py_dirty_hack, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py_dirty_hack, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py_dirty_hack, "$1(?<$2>$3)$4\\\\g{$2}$5");
        s = std::regex_replace(s, py_tail, "?<");
        s = std::regex_replace(s, upper, R"([A-Z])");
        s = std::regex_replace(s, lower, R"([a-z])");
        s = std::regex_replace(s, alpha, R"([[A-Z][a-z]])");
        s = std::regex_replace(s, digit, R"([0-9])");
        s = std::regex_replace(s, xdigit, R"([[0-9]A-Fa-f])");
        s = std::regex_replace(s, alnum, R"([[[A-Z][a-z]][0-9]])");
        s = std::regex_replace(s, word, R"(\w)");
        s = std::regex_replace(s, punct, R"([-!"#$%&'()*+,./:;<=>?@[\\\]_`{|}~])");
        s = std::regex_replace(s, blank, R"([ \t])");
        s = std::regex_replace(s, space, R"(\s)");
        s = std::regex_replace(s, cntrl, R"([\x00-\x1F\x7F])");
        s = std::regex_replace(s, graph, R"([\x21-\x7E])");
        s = std::regex_replace(s, print, R"([\x20-\x7E])");
        check_balance(s);
        delete_brackets(s);
        if(std::regex_match(s,match, neg_num)){
            int p = std::stoi(match.str(1));
            num = count_brackets(s) + p + 1;
            std::string put = "\\" + std::to_string(num);
            s = std::regex_replace(s, neg_num2, put);
        }
        if (s.empty()) {
            continue;
        }
        if (std::regex_match(s, rows)) {
#if URI == 1
            regular_exps.emplace_back(uri, s.substr(1, s.size() - 2));
#elif
            regular_exps.insert(s.substr(1, s.size() - 2));
#endif
        } else {
#if URI == 1
            regular_exps.emplace_back(uri, s);
#elif
            regular_exps.insert(s);
#endif
        }

    }

    std::ofstream file("back_ref.txt");
#if URI == 1
    std::ofstream u("uris2.txt");
    for (const auto& r: regular_exps) {
        file << r.second << '\n';
        u << r.first << '\n';
    }
#else
    for (const std::string &r: regular_exps) {
        file << r << '\n';
    }
#endif
    return 0;
}