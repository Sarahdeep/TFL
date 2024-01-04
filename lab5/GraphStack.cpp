#include "GraphStack.hpp"

GraphStack::GraphStack(std::string &rt) {
    root = new Node(rt);
}

Node::Node(std::string &val): value(val){
}
