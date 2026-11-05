#pragma once
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include "ast.h"

struct ReturnSignal {
    int value;
};

struct Interpreter {
    std::map<std::string, int> vars;
    std::ostream& out;

    explicit Interpreter(std::ostream& o) : out(o) {}

    int& lookup(const std::string& name) {
        auto it = vars.find(name);
        if (it == vars.end()) throw std::runtime_error("undeclared variable '" + name + "'");
        return it->second;
    }

    int eval(Node* n) {
        if (n->type == "num") return std::stoi(n->value);
        if (n->type == "var") return lookup(n->value);
        if (n->type == "unop") {
            int a = eval(n->kids[0]);
            return n->value == "-" ? -a : !a;
        }
        const std::string& op = n->value;
        int a = eval(n->kids[0]);
        if (op == "&&") return a && eval(n->kids[1]);
        if (op == "||") return a || eval(n->kids[1]);
        int b = eval(n->kids[1]);
        if (op == "+") return a + b;
        if (op == "-") return a - b;
        if (op == "*") return a * b;
        if (op == "/" || op == "%") {
            if (b == 0) throw std::runtime_error("division by zero");
            return op == "/" ? a / b : a % b;
        }
        if (op == "<") return a < b;
        if (op == ">") return a > b;
        if (op == "<=") return a <= b;
        if (op == ">=") return a >= b;
        if (op == "==") return a == b;
        if (op == "!=") return a != b;
        throw std::runtime_error("unknown operator " + op);
    }

    void print(Node* n) {
        const std::string& f = n->value;
        size_t arg = 0;
        for (size_t i = 0; i < f.size(); i++) {
            if (f[i] == '\\' && i + 1 < f.size()) {
                char c = f[++i];
                out << (c == 'n' ? '\n' : c == 't' ? '\t' : c);
            } else if (f[i] == '%' && i + 1 < f.size()) {
                char c = f[++i];
                if (c == 'd') {
                    if (arg >= n->kids.size()) throw std::runtime_error("printf: not enough arguments");
                    out << eval(n->kids[arg++]);
                } else {
                    out << c;
                }
            } else {
                out << f[i];
            }
        }
    }

    void exec(Node* n) {
        const std::string& t = n->type;
        if (t == "block") {
            for (Node* k : n->kids) exec(k);
        } else if (t == "decl") {
            vars[n->value] = n->kids.empty() ? 0 : eval(n->kids[0]);
        } else if (t == "assign") {
            lookup(n->value) = eval(n->kids[0]);
        } else if (t == "if") {
            if (eval(n->kids[0])) exec(n->kids[1]);
            else if (n->kids.size() > 2) exec(n->kids[2]);
        } else if (t == "while") {
            while (eval(n->kids[0])) exec(n->kids[1]);
        } else if (t == "for") {
            for (exec(n->kids[0]); eval(n->kids[1]); exec(n->kids[2])) exec(n->kids[3]);
        } else if (t == "printf") {
            print(n);
        } else if (t == "return") {
            throw ReturnSignal{eval(n->kids[0])};
        }
    }

    int run(Node* program) {
        try {
            exec(program->kids[0]);
        } catch (ReturnSignal& r) {
            return r.value;
        }
        return 0;
    }
};
