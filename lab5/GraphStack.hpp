#ifndef LAB5_GRAPHSTACK_HPP
#define LAB5_GRAPHSTACK_HPP
#include <iostream>
class Node{
public:
    std::string value;
    Node* parent= nullptr;
    Node(std::string& val);
};
class GraphStack {
public:
std::string value;
Node* root = nullptr;
void push(Node* parent, std::string value);
std::string pop();
GraphStack(std::string& root);
};


#endif //LAB5_GRAPHSTACK_HPP
