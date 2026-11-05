%{
#include <cstdio>
#include <string>
#include "ast.h"

int yylex();
void yyerror(const char* s);
extern int yylineno;

Node* root = nullptr;

static Node* bin(const char* op, Node* a, Node* b) {
    return new Node("binop", op, {a, b});
}
%}

%error-verbose

%union {
    int num;
    std::string* str;
    Node* node;
}

%token <num> NUMBER
%token <str> IDENT STRING
%token INT IF ELSE WHILE FOR RETURN PRINTF
%token EQ NE LE GE AND OR INC

%type <node> program block stmts stmt simple args expr

%nonassoc IFX
%nonassoc ELSE

%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right '!' UMINUS

%%

program
    : INT IDENT '(' ')' block                       { root = new Node("program", *$2, {$5}); }
    ;

block
    : '{' stmts '}'                                 { $$ = $2; }
    ;

stmts
    :                                               { $$ = new Node("block"); }
    | stmts stmt                                    { $$ = $1->add($2); }
    ;

stmt
    : simple ';'                                    { $$ = $1; }
    | IF '(' expr ')' stmt %prec IFX                { $$ = new Node("if", "", {$3, $5}); }
    | IF '(' expr ')' stmt ELSE stmt                { $$ = new Node("if", "", {$3, $5, $7}); }
    | WHILE '(' expr ')' stmt                       { $$ = new Node("while", "", {$3, $5}); }
    | FOR '(' simple ';' expr ';' simple ')' stmt   { $$ = new Node("for", "", {$3, $5, $7, $9}); }
    | PRINTF '(' STRING args ')' ';'                { $4->value = *$3; $$ = $4; }
    | RETURN expr ';'                               { $$ = new Node("return", "", {$2}); }
    | block                                         { $$ = $1; }
    ;

simple
    : INT IDENT                                     { $$ = new Node("decl", *$2); }
    | INT IDENT '=' expr                            { $$ = new Node("decl", *$2, {$4}); }
    | IDENT '=' expr                                { $$ = new Node("assign", *$1, {$3}); }
    | IDENT INC                                     { $$ = new Node("assign", *$1, {bin("+", new Node("var", *$1), new Node("num", "1"))}); }
    ;

args
    :                                               { $$ = new Node("printf"); }
    | args ',' expr                                 { $$ = $1->add($3); }
    ;

expr
    : NUMBER                                        { $$ = new Node("num", std::to_string($1)); }
    | IDENT                                         { $$ = new Node("var", *$1); }
    | '(' expr ')'                                  { $$ = $2; }
    | expr '+' expr                                 { $$ = bin("+", $1, $3); }
    | expr '-' expr                                 { $$ = bin("-", $1, $3); }
    | expr '*' expr                                 { $$ = bin("*", $1, $3); }
    | expr '/' expr                                 { $$ = bin("/", $1, $3); }
    | expr '%' expr                                 { $$ = bin("%", $1, $3); }
    | expr '<' expr                                 { $$ = bin("<", $1, $3); }
    | expr '>' expr                                 { $$ = bin(">", $1, $3); }
    | expr LE expr                                  { $$ = bin("<=", $1, $3); }
    | expr GE expr                                  { $$ = bin(">=", $1, $3); }
    | expr EQ expr                                  { $$ = bin("==", $1, $3); }
    | expr NE expr                                  { $$ = bin("!=", $1, $3); }
    | expr AND expr                                 { $$ = bin("&&", $1, $3); }
    | expr OR expr                                  { $$ = bin("||", $1, $3); }
    | '-' expr %prec UMINUS                         { $$ = new Node("unop", "-", {$2}); }
    | '!' expr                                      { $$ = new Node("unop", "!", {$2}); }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Syntax error (line %d): %s\n", yylineno, s);
}
