#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHILDREN 10
#define MAX_LABEL_LENGTH 100

typedef struct TreeNode {
    char label[MAX_LABEL_LENGTH];
    struct TreeNode* children[MAX_CHILDREN];
    int child_count;
} TreeNode;

TreeNode* create_node(const char* label) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    strncpy(node->label, label, MAX_LABEL_LENGTH);
    node->child_count = 0;
    return node;
}

void add_child(TreeNode* parent, TreeNode* child) {
    if (parent->child_count < MAX_CHILDREN) {
        parent->children[parent->child_count++] = child;
    } else {
        printf("Error: Maximum children limit reached!\n");
    }
}

void print_dot(TreeNode* node, FILE* dot_file) {
    if (!node) return;

    // Print node
    fprintf(dot_file, "\"%s\" [label=\"%s\"];\n", node->label, node->label);

    // Print edges (parent -> child)
    for (int i = 0; i < node->child_count; i++) {
        fprintf(dot_file, "\"%s\" -> \"%s\";\n", node->label, node->children[i]->label);
        print_dot(node->children[i], dot_file); // Recursively print children
    }
}

void free_tree(TreeNode* node) {
    for (int i = 0; i < node->child_count; i++) {
        free_tree(node->children[i]);
    }
    free(node);
}

int main() {
    FILE* input_file = fopen("parsetree.txt", "r");
    if (!input_file) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    TreeNode* root = NULL;
    TreeNode* current_parent = NULL;
    char line[256];
    TreeNode* node_stack[256];
    int stack_top = -1;

    while (fgets(line, sizeof(line), input_file)) {
        // Remove leading and trailing whitespaces
        char* start = line;
        while (*start == ' ') start++; // Skip leading spaces
        char* end = start + strlen(start) - 1;
        while (end > start && *end == '\n') end--; // Remove newline
        *(end + 1) = '\0'; // Null terminate

        // Get level of indentation
        int level = 0;
        while (line[level] == ' ') level++; // Count leading spaces (indentation)

        // Create a new node with the current line
        TreeNode* node = create_node(start);

        if (level == 0) {
            root = node; // If root level, it's the root
        } else {
            // Get the correct parent node from the stack based on indentation level
            while (stack_top >= level) {
                stack_top--;
            }
            current_parent = node_stack[stack_top];
            add_child(current_parent, node);
        }

        // Push the current node onto the stack
        stack_top++;
        node_stack[stack_top] = node;
    }

    fclose(input_file);

    // Open output file for .dot format
    FILE* dot_file = fopen("parsetree.dot", "w");
    if (!dot_file) {
        perror("Error opening output .dot file");
        return EXIT_FAILURE;
    }

    // Print the .dot file header
    fprintf(dot_file, "digraph ParseTree {\n");

    // Print the tree in .dot format
    if (root) {
        print_dot(root, dot_file);
    }

    // Print the .dot file footer
    fprintf(dot_file, "}\n");

    fclose(dot_file);

    // Free the tree memory
    free_tree(root);

    printf("Parse tree in .dot format has been generated in parsetree.dot\n");

    return 0;
}
