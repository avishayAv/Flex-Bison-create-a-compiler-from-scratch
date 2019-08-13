

%{
void error_char();
#include <stdio.h>
#include <string.h>
#include "parser.hpp"
#include "parser.tab.hpp"
#include "output.hpp"
void showToken(char*);
%}

%option yylineno
%option noyywrap

whitespace	([\t\n \r])

%%

"void" 			return VOID;
"int" 			return INT;
"byte" 			return BYTE;
"b" 			return B;
"bool" 			return BOOL;
"and" 			return AND;
"or" 			return OR;
"not"			return NOT;
"true" 			{yylval = new Var(yytext,"bool"); return TRUE;}
"false" 		{yylval = new Var(yytext,"bool"); return FALSE;}
"return" 		return RETURN;
"if" 			return IF;
"else" 			return ELSE;
"while" 		return WHILE;
"break" 		return BREAK;
"continue" 		return CONTINUE;
"@pre" 			return PRECOND;
";" 			return SC;
"," 			return COMMA;
"(" 			return LPAREN;
")" 			return RPAREN;
"{" 			return LBRACE;
"}" 			return RBRACE;
"=" 			return ASSIGN;
"=="|"!="|"<"|">"|"<="|">="			{yylval = new MyString(yytext); return RELOP;}
[+]|[-]|[*]|[/] 					{yylval = new MyString(yytext); return BINOP;}
[a-zA-Z][a-zA-Z0-9]*				{yylval = new Identifier(yytext);   return ID;}
[0]|[1-9][0-9]*						{yylval = new Var(yytext,"int"); return NUM;}
"//"[^\r\n]*[\r|\n|\r\n]? 			{}
\"([^\n\r\"\\]|\\[rnt"\\])+\"	 	{yytext[yyleng-1]='\0'; yytext++; yylval = new MyString(yytext); return STRING;}
{whitespace}						{}

.                                  error_char();
%%

void error_char() {
    output::errorLex(yylineno);
    exit(0);
}