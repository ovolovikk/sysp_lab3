%{
#include <string>
#include "ast.h"
#include "parser.hpp"
%}

%option noyywrap
%option yylineno
%option nounput
%option noinput

%%

^[ \t]*#.*              ;
"//".*                  ;
"/*"([^*]|\*+[^*/])*\*+"/" ;
[ \t\r\n]+              ;

"int"                   return INT;
"if"                    return IF;
"else"                  return ELSE;
"while"                 return WHILE;
"for"                   return FOR;
"return"                return RETURN;
"printf"                return PRINTF;

"=="                    return EQ;
"!="                    return NE;
"<="                    return LE;
">="                    return GE;
"&&"                    return AND;
"||"                    return OR;
"++"                    return INC;

[0-9]+                  { yylval.num = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = new std::string(yytext); return IDENT; }
\"([^"\\\n]|\\.)*\"     { yylval.str = new std::string(yytext + 1, yyleng - 2); return STRING; }

.                       return yytext[0];

%%
