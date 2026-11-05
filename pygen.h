#pragma once
#include <sstream>
#include <string>
#include "ast.h"

struct PyGen {
    std::ostringstream out;

    void line(int indent, const std::string& s) {
        out << std::string(indent * 4, ' ') << s << "\n";
    }

    std::string expr(Node* n, bool top = true) {
        if (n->type == "num" || n->type == "var") return n->value;
        if (n->type == "unop") {
            if (n->value == "!") return "int(not " + expr(n->kids[0]) + ")";
            return "-" + expr(n->kids[0], false);
        }
        const std::string& op = n->value;
        bool call = op == "&&" || op == "||" || op == "/" || op == "%";
        std::string a = expr(n->kids[0], call);
        std::string b = expr(n->kids[1], call);
        if (op == "&&") return "int(bool(" + a + ") and bool(" + b + "))";
        if (op == "||") return "int(bool(" + a + ") or bool(" + b + "))";
        if (op == "/") return "cdiv(" + a + ", " + b + ")";
        if (op == "%") return "cmod(" + a + ", " + b + ")";
        std::string s = a + " " + op + " " + b;
        return top ? s : "(" + s + ")";
    }

    void stmt(Node* n, int ind) {
        const std::string& t = n->type;
        if (t == "block") {
            if (n->kids.empty()) line(ind, "pass");
            for (Node* k : n->kids) stmt(k, ind);
        } else if (t == "decl") {
            line(ind, n->value + " = " + (n->kids.empty() ? "0" : expr(n->kids[0])));
        } else if (t == "assign") {
            line(ind, n->value + " = " + expr(n->kids[0]));
        } else if (t == "if") {
            line(ind, "if " + expr(n->kids[0]) + ":");
            stmt(n->kids[1], ind + 1);
            if (n->kids.size() > 2) {
                line(ind, "else:");
                stmt(n->kids[2], ind + 1);
            }
        } else if (t == "while") {
            line(ind, "while " + expr(n->kids[0]) + ":");
            stmt(n->kids[1], ind + 1);
        } else if (t == "for") {
            stmt(n->kids[0], ind);
            line(ind, "while " + expr(n->kids[1]) + ":");
            stmt(n->kids[3], ind + 1);
            stmt(n->kids[2], ind + 1);
        } else if (t == "printf" && n->kids.empty()) {
            std::string s = n->value;
            for (size_t p = s.find("%%"); p != std::string::npos; p = s.find("%%", p + 1)) s.erase(p, 1);
            line(ind, "print(\"" + s + "\", end=\"\")");
        } else if (t == "printf") {
            std::string args;
            for (size_t i = 0; i < n->kids.size(); i++) {
                if (i) args += ", ";
                args += expr(n->kids[i]);
            }
            if (n->kids.size() == 1) args += ",";
            line(ind, "print(\"" + n->value + "\" % (" + args + "), end=\"\")");
        } else if (t == "return") {
            line(ind, "return " + expr(n->kids[0]));
        }
    }

    std::string generate(Node* program) {
        line(0, "def cdiv(a, b):");
        line(1, "q = abs(a) // abs(b)");
        line(1, "return q if (a < 0) == (b < 0) else -q");
        line(0, "");
        line(0, "");
        line(0, "def cmod(a, b):");
        line(1, "return a - b * cdiv(a, b)");
        line(0, "");
        line(0, "");
        line(0, "def " + program->value + "():");
        stmt(program->kids[0], 1);
        line(0, "");
        line(0, "");
        line(0, "raise SystemExit(" + program->value + "())");
        return out.str();
    }
};
