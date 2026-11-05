CXX      = clang++
CXXFLAGS = -std=c++17 -O2 -Wno-deprecated-register -Wno-unused-function
FILE    ?= examples/test.c

all: cparser

parser.cpp parser.hpp: parser.y
	bison -d -v -o parser.cpp parser.y

lexer.cpp: lexer.l parser.hpp
	flex -o lexer.cpp lexer.l

cparser: parser.cpp lexer.cpp main.cpp ast.h interpreter.h pygen.h
	$(CXX) $(CXXFLAGS) -o cparser parser.cpp lexer.cpp main.cpp

run: cparser
	./cparser $(FILE)

python: run
	python3 output/program.py

compare: run
	@$(CXX) -w -o output/native $(FILE)
	@./output/native > output/native.txt; echo "[exit $$?]" >> output/native.txt
	@python3 output/program.py > output/python.txt; echo "[exit $$?]" >> output/python.txt
	@echo "---- clang ----";  cat output/native.txt
	@echo "---- python ----"; cat output/python.txt
	@diff output/native.txt output/python.txt && echo "OK: outputs match"

viz: run
	open viz/index.html

conflicts:
	@mkdir -p output
	@sed 's/ %prec IFX//' parser.y > output/noprec.y
	@bison -v -o output/noprec.cpp output/noprec.y
	@n=$$(sed -n 's/^State \([0-9]*\) conflicts.*/\1/p' output/noprec.output); \
	 awk -v s="state $$n" -v e="state $$((n + 1))" '$$0 == s {f = 1} $$0 == e {f = 0} f' output/noprec.output

clean:
	rm -rf cparser parser.cpp parser.hpp parser.output lexer.cpp output viz/ast.js

.PHONY: all run python compare viz conflicts clean
