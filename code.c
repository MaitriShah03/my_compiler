#include <stdio.h>
#include "parsetree.h"   // for TreeNode, save_tree()

extern FILE *yyin;
FILE *tokenFile, *tacFile, *declFile, *treeFile;

extern TreeNode* root;
//extern TreeNode *root;  // set by parser
int yyparse();

int main() {
    yyin = fopen("input.txt", "r");
    if (!yyin) { perror("Failed to open input.txt"); return 1; }

    tokenFile = fopen("tokens.txt", "w");
    tacFile   = fopen("tac.txt",   "w");
    declFile  = fopen("decl.txt",  "w");
    treeFile  = fopen("parsetree.txt", "w");

    if (!tokenFile || !tacFile || !declFile || !treeFile) {
        perror("Failed to open one of the output files");
        return 1;
    }

    if (yyparse() != 0) {
        fprintf(stderr, "Parsing failed\n");
    } else {
        // Save the parse tree to parsetree.txt
        save_tree(root, treeFile, 0);
        printf("Parse tree written to parsetree.txt\n");
    }

    fclose(yyin);
    fclose(tokenFile);
    fclose(tacFile);
    fclose(declFile);
    fclose(treeFile);
    return 0;
}
