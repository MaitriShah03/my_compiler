%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsetree.h"

TreeNode* root = NULL;
extern int yylineno;
extern FILE *yyin;
extern FILE *tokenFile;
extern FILE *tacFile;
extern FILE *declFile;
//extern TreeNode *root;


extern int yylex();
void yyerror(const char *s);
char* new_temp();

int temp_count = 0;

char* new_temp() {
    char* t = malloc(8);
    if (!t) {
        fprintf(stderr, "Memory allocation failed for new temporary variable\n");
        exit(1);
    }
    sprintf(t, "t%d", temp_count++);
    return t;
}
%}

%union {
    int num;
    char* str;
    TreeNode* node;
}

// Operator precedence
%nonassoc IFX
%nonassoc ELSE
%left PLUS MINUS
%left MUL DIV
%left EQ

// Token definitions
%token <num> NUMBER
%token <str> ID
%token INT FLOAT BOOLEAN STRING IF ELSE WHILE FOR RETURN BREAK CONTINUE VOID MAIN
%token EQ ASSIGN PLUS MINUS MUL DIV
%token LPAREN RPAREN LBRACE RBRACE SEMI COMMA

// Non-terminal return types
%type <node> program functions function type_specifier
%type <node> stmts stmt decl_stmt assign_stmt expr_stmt return_stmt if_stmt while_stmt
%type <str> expr

%%

program:
    functions { 
        $$ = make_node("Program", 1, $1);
        root = $$;
    }
    ;

functions:
    /* empty */ { $$ = make_node("Functions", 0); }
    | functions function { $$ = make_node("Functions", 2, $1, $2); }
    ;

function:
    type_specifier ID LPAREN RPAREN LBRACE stmts RBRACE {
        fprintf(declFile, "Function: %s returns %s\n", $2, get_node_value($1));
        $$ = make_node("Function", 7,
            $1,
            make_leaf_str("ID", $2),
            make_leaf("LPAREN"),
            make_leaf("RPAREN"),
            make_leaf("LBRACE"),
            $6,
            make_leaf("RBRACE"));
        free($2);
    }
    | type_specifier MAIN LPAREN RPAREN LBRACE stmts RBRACE {
        fprintf(declFile, "Main function returns %s\n", get_node_value($1));
        $$ = make_node("MainFunction", 7,
            $1,
            make_leaf("MAIN"),
            make_leaf("LPAREN"),
            make_leaf("RPAREN"),
            make_leaf("LBRACE"),
            $6,
            make_leaf("RBRACE"));
    }
    ;

type_specifier:
    INT     { $$ = make_leaf_str("TYPE", "int"); }
    | FLOAT     { $$ = make_leaf_str("TYPE", "float"); }
    | BOOLEAN   { $$ = make_leaf_str("TYPE", "boolean"); }
    | STRING    { $$ = make_leaf_str("TYPE", "string"); }
    | VOID      { $$ = make_leaf_str("TYPE", "void"); }
    ;

stmts:
    stmts stmt      { $$ = make_node("Stmts", 2, $1, $2); }
    | /* empty */   { $$ = make_node("Stmts", 0); }
    | error SEMI {
        yyclearin;
        $$ = make_node("ErrorStmt", 0);
        fprintf(stderr, "Syntax error recovered at line %d.\n", yylineno);
    }
    ;

stmt:
    decl_stmt
    | assign_stmt
    | expr_stmt
    | return_stmt
    | if_stmt
    | while_stmt
    ;

decl_stmt:
    type_specifier ID SEMI {
        fprintf(declFile, "Declare %s %s\n", get_node_value($1), $2);
        $$ = make_node("DeclStmt", 3, $1, make_leaf_str("ID", $2), make_leaf("SEMI"));
        free($2);
    }
    ;

assign_stmt:
    ID ASSIGN expr SEMI {
        fprintf(tacFile, "%s = %s\n", $1, $3);
        $$ = make_node("AssignStmt", 4,
            make_leaf_str("ID", $1),
            make_leaf("ASSIGN"),
            make_leaf_str("EXPR", $3),
            make_leaf("SEMI"));
        free($1);
        free($3);
    }
    ;

expr_stmt:
    expr SEMI {
        fprintf(tacFile, "Expression: %s\n", $1);
        $$ = make_node("ExprStmt", 2,
            make_leaf_str("EXPR", $1),
            make_leaf("SEMI"));
        free($1);
    }
    ;

return_stmt:
    RETURN expr SEMI {
        fprintf(tacFile, "Return %s\n", $2);
        $$ = make_node("ReturnStmt", 3,
            make_leaf("RETURN"),
            make_leaf_str("EXPR", $2),
            make_leaf("SEMI"));
        free($2);
    }
    | RETURN SEMI {
        fprintf(tacFile, "Return void\n");
        $$ = make_node("ReturnStmt", 2, make_leaf("RETURN"), make_leaf("SEMI"));
    }
    ;

if_stmt:
    IF LPAREN expr RPAREN stmt %prec IFX {
        $$ = make_node("IfStmt", 5,
            make_leaf("IF"),
            make_leaf("LPAREN"),
            make_leaf_str("EXPR", $3),
            make_leaf("RPAREN"),
            $5);
        free($3);
    }
    | IF LPAREN expr RPAREN stmt ELSE stmt {
        $$ = make_node("IfElseStmt", 7,
            make_leaf("IF"),
            make_leaf("LPAREN"),
            make_leaf_str("EXPR", $3),
            make_leaf("RPAREN"),
            $5,
            make_leaf("ELSE"),
            $7);
        free($3);
    }
    ;

while_stmt:
    WHILE LPAREN expr RPAREN LBRACE stmts RBRACE {
        fprintf(tacFile, "While loop condition: %s\n", $3);
        $$ = make_node("WhileStmt", 7,
            make_leaf("WHILE"),
            make_leaf("LPAREN"),
            make_leaf_str("EXPR", $3),
            make_leaf("RPAREN"),
            make_leaf("LBRACE"),
            $6,
            make_leaf("RBRACE"));
        free($3);
    }
    ;

expr:
    expr PLUS expr {
        char* t = new_temp();
        fprintf(tacFile, "%s = %s + %s\n", t, $1, $3);
        $$ = t;
        free($1); free($3);
    }
    | expr MINUS expr {
        char* t = new_temp();
        fprintf(tacFile, "%s = %s - %s\n", t, $1, $3);
        $$ = t;
        free($1); free($3);
    }
    | expr MUL expr {
        char* t = new_temp();
        fprintf(tacFile, "%s = %s * %s\n", t, $1, $3);
        $$ = t;
        free($1); free($3);
    }
    | expr DIV expr {
        char* t = new_temp();
        fprintf(tacFile, "%s = %s / %s\n", t, $1, $3);
        $$ = t;
        free($1); free($3);
    }
    | expr EQ expr {
        char* t = new_temp();
        fprintf(tacFile, "%s = %s == %s\n", t, $1, $3);
        $$ = t;
        free($1); free($3);
    }
    | NUMBER {
        char* t = malloc(12);
        if (!t) {
            fprintf(stderr, "Memory allocation failed for number\n");
            exit(1);
        }
        sprintf(t, "%d", $1);
        $$ = t;
    }
    | ID {
        $$ = strdup($1);
    }
    | ID LPAREN RPAREN {
        char* t = new_temp();
        fprintf(tacFile, "%s = call %s()\n", t, $1);
        $$ = t;
        free($1);
    }
    | LPAREN expr RPAREN {
        $$ = $2;
    }
    ;

%%

// Error handler
void yyerror(const char *s) {
    extern char *yytext;
    fprintf(stderr, "Error at line %d near '%s': %s\n", yylineno, yytext, s);
}
