%{
#include "parsetree.h"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *tokenFile;
%}

DIGIT      [0-9]+
ID         [a-zA-Z_][a-zA-Z0-9_]* 
WS         [ \t]+

%%

"//".*          /* skip C++‑style comments */


"int"        { fprintf(tokenFile, "INT\n"); return INT; }
"float"      { fprintf(tokenFile, "FLOAT\n"); return FLOAT; }
"boolean"    { fprintf(tokenFile, "BOOLEAN\n"); return BOOLEAN;}
"string"     { fprintf(tokenFile, "STRING\n"); return STRING;}
"if"         { fprintf(tokenFile, "IF\n"); return IF; }
"else"       { fprintf(tokenFile, "ELSE\n"); return ELSE; }
"while"      { fprintf(tokenFile, "WHILE\n"); return WHILE; }
"for"        { fprintf(tokenFile, "FOR\n"); return FOR; }
"return"     { fprintf(tokenFile, "RETURN\n"); return RETURN; }
"break"      { fprintf(tokenFile, "BREAK\n"); return BREAK; }
"continue"   { fprintf(tokenFile, "CONTINUE\n"); return CONTINUE; }
"void"       { fprintf(tokenFile, "VOID\n"); return VOID; }
"main"       { fprintf(tokenFile, "MAIN\n"); return MAIN; }

"=="         { fprintf(tokenFile, "EQ\n"); return EQ; }
"="          { fprintf(tokenFile, "ASSIGN\n"); return ASSIGN; }
"+"          { fprintf(tokenFile, "PLUS\n"); return PLUS; }
"-"          { fprintf(tokenFile, "MINUS\n"); return MINUS; }
"*"          { fprintf(tokenFile, "MUL\n"); return MUL; }
"/"          { fprintf(tokenFile, "DIV\n"); return DIV; }

"("          { fprintf(tokenFile, "LPAREN\n"); return LPAREN; }
")"          { fprintf(tokenFile, "RPAREN\n"); return RPAREN; }
"{"          { fprintf(tokenFile, "LBRACE\n"); return LBRACE; }
"}"          { fprintf(tokenFile, "RBRACE\n"); return RBRACE; }
";"          { fprintf(tokenFile, "SEMI\n"); return SEMI; }
","          { fprintf(tokenFile, "COMMA\n"); return COMMA; }  // Added for function parameters

{DIGIT}      { fprintf(tokenFile, "NUMBER(%s)\n", yytext); yylval.num = atoi(yytext); return NUMBER; }
{ID}         { fprintf(tokenFile, "ID(%s)\n", yytext); yylval.str = strdup(yytext); return ID; }

{WS}         ;  // Ignore whitespace
\n           { yylineno++; }
.            { fprintf(stderr, "Unknown character: %s at line %d\n", yytext, yylineno); }

%%

int yywrap(void) {
    return 1;  // End of input stream
}

#ifdef STANDALONE_LEXER
int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Could not open %s\n", argv[1]);
            return 1;
        }
    } else {
        yyin = stdin;
    }
    
    tokenFile = fopen("tokens.txt", "w");
    if (!tokenFile) {
        fprintf(stderr, "Could not open tokens.txt for writing\n");
        return 1;
    }
    
    int token;
    while ((token = yylex()) != 0) {
        printf("Token: %d", token);
        if (token == NUMBER) printf(" (value: %d)\n", yylval.num);
        else if (token == ID) printf(" (id: %s)\n", yylval.str);
        else printf("\n");
    }
    
    fclose(tokenFile);
    if (yyin != stdin) fclose(yyin);
    return 0;
}
#endif