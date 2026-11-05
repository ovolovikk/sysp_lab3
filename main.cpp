#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include "ast.h"
#include "interpreter.h"
#include "pygen.h"

extern FILE* yyin;
extern int yyparse();
extern Node* root;

static std::string readFile(const char* path) {
    std::ifstream f(path);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <file.c>\n";
        return 1;
    }
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cerr << "cannot open " << argv[1] << "\n";
        return 1;
    }
    if (yyparse() != 0 || !root) return 1;

    std::cout << "========== AST ==========\n";
    printTree(root);

    std::string python = PyGen().generate(root);
    std::cout << "\n========== Python ==========\n" << python;

    std::ostringstream output;
    int code = 0;
    std::string error;
    try {
        code = Interpreter(output).run(root);
    } catch (std::exception& e) {
        error = e.what();
    }
    std::cout << "\n========== Run ==========\n" << output.str();
    if (!error.empty()) std::cout << "\nRuntime error: " << error << "\n";
    std::cout << "\n[exit code " << code << "]\n";

    std::filesystem::create_directories("output");
    std::ofstream("output/program.py") << python;
    std::ofstream("viz/ast.js") << "const DATA = {"
        << "\"file\":" << jsonEscape(argv[1])
        << ",\"source\":" << jsonEscape(readFile(argv[1]))
        << ",\"python\":" << jsonEscape(python)
        << ",\"output\":" << jsonEscape(output.str() + (error.empty() ? "" : "\nRuntime error: " + error))
        << ",\"ast\":" << toJson(root) << "};\n";

    std::cout << "\nwritten: output/program.py, viz/ast.js\n";
    return 0;
}
