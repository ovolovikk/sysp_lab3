#pragma once
#include <iostream>
#include <string>
#include <vector>

struct Node {
    std::string type;
    std::string value;
    std::vector<Node*> kids;

    Node(std::string t, std::string v = "", std::vector<Node*> k = {})
        : type(std::move(t)), value(std::move(v)), kids(std::move(k)) {}

    Node* add(Node* n) {
        kids.push_back(n);
        return this;
    }
};

inline void printTree(Node* n, const std::string& prefix = "", bool last = true, bool isRoot = true) {
    std::cout << prefix;
    if (!isRoot) std::cout << (last ? "└── " : "├── ");
    std::cout << n->type;
    if (!n->value.empty()) std::cout << " " << n->value;
    std::cout << "\n";
    std::string next = isRoot ? "" : prefix + (last ? "    " : "│   ");
    for (size_t i = 0; i < n->kids.size(); i++)
        printTree(n->kids[i], next, i + 1 == n->kids.size(), false);
}

inline std::string jsonEscape(const std::string& s) {
    std::string r;
    for (char c : s) {
        if (c == '"') r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else if (c == '\n') r += "\\n";
        else if (c == '\t') r += "\\t";
        else r += c;
    }
    return "\"" + r + "\"";
}

inline std::string toJson(Node* n) {
    std::string s = "{\"type\":" + jsonEscape(n->type) + ",\"value\":" + jsonEscape(n->value) + ",\"children\":[";
    for (size_t i = 0; i < n->kids.size(); i++) {
        if (i) s += ",";
        s += toJson(n->kids[i]);
    }
    return s + "]}";
}
