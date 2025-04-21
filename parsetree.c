#include "parsetree.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enable or disable debug logs
#define DEBUG 0

#if DEBUG
#define LOG(fmt, ...) fprintf(stderr, "[LOG] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG(fmt, ...)
#endif

TreeNode* make_node(const char* label, int child_count, ...) {
    if (label == NULL) {
        fprintf(stderr, "Error in make_node: Null label passed\n");
        exit(EXIT_FAILURE);
    }
    if (child_count < 0) {
        fprintf(stderr, "Error in make_node: Negative child count (%d)\n", child_count);
        exit(EXIT_FAILURE);
    }

    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) {
        fprintf(stderr, "Error in make_node: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    node->label = strdup(label);
    if (!node->label) {
        free(node);
        fprintf(stderr, "Error in make_node: Memory allocation for label failed\n");
        exit(EXIT_FAILURE);
    }

    node->child_count = child_count;
    node->children = NULL;

    if (child_count > 0) {
        node->children = (TreeNode**)malloc(child_count * sizeof(TreeNode*));
        if (!node->children) {
            free(node->label);
            free(node);
            fprintf(stderr, "Error in make_node: Memory allocation for children failed\n");
            exit(EXIT_FAILURE);
        }

        va_list args;
        va_start(args, child_count);
        for (int i = 0; i < child_count; i++) {
            node->children[i] = va_arg(args, TreeNode*);
            if (!node->children[i]) {
                fprintf(stderr, "Warning in make_node: Null child node at index %d\n", i);
            }
        }
        va_end(args);
    }

    LOG("Created node: %s with %d children", label, child_count);
    return node;
}

TreeNode* make_leaf(const char* label) {
    if (label == NULL) {
        fprintf(stderr, "Error in make_leaf: Null label passed\n");
        exit(EXIT_FAILURE);
    }

    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) {
        fprintf(stderr, "Error in make_leaf: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    node->label = strdup(label);
    if (!node->label) {
        free(node);
        fprintf(stderr, "Error in make_leaf: Memory allocation for label failed\n");
        exit(EXIT_FAILURE);
    }

    node->child_count = 0;
    node->children = NULL;

    LOG("Created leaf node: %s", label);
    return node;
}

TreeNode* make_leaf_str(const char* label, const char* value) {
    if (!label || !value) {
        fprintf(stderr, "Error in make_leaf_str: Null label or value passed\n");
        exit(EXIT_FAILURE);
    }

    size_t total_len = strlen(label) + strlen(value) + 2; // ':' + '\0'
    char* combined = (char*)malloc(total_len);
    if (!combined) {
        fprintf(stderr, "Error in make_leaf_str: Memory allocation for combined label failed\n");
        exit(EXIT_FAILURE);
    }

    snprintf(combined, total_len, "%s:%s", label, value);
    TreeNode* node = make_leaf(combined);
    free(combined);

    return node;
}

TreeNode* make_leaf_type(const char* type) {
    if (!type) {
        fprintf(stderr, "Error in make_leaf_type: Null type passed\n");
        exit(EXIT_FAILURE);
    }
    return make_leaf_str("TYPE", type);
}

const char* get_node_value(const TreeNode* node) {
    if (!node || !node->label) return NULL;

    const char* colon = strchr(node->label, ':');
    return colon ? colon + 1 : NULL;
}

// Static buffer to return type without dynamic memory
const char* get_node_type(const TreeNode* node) {
    if (!node || !node->label) return NULL;

    static char buffer[128];
    const char* colon = strchr(node->label, ':');

    if (!colon) return node->label;

    size_t len = colon - node->label;
    if (len >= sizeof(buffer)) len = sizeof(buffer) - 1;

    strncpy(buffer, node->label, len);
    buffer[len] = '\0';

    return buffer;
}

int is_node_type(const TreeNode* node, const char* type) {
    if (!node || !node->label || !type) return 0;

    const char* colon = strchr(node->label, ':');
    if (colon) {
        return strncmp(node->label, type, colon - node->label) == 0;
    }
    return strcmp(node->label, type) == 0;
}

void free_tree(TreeNode* node) {
    if (!node) return;

    for (int i = 0; i < node->child_count; i++) {
        free_tree(node->children[i]);
    }

    free(node->children);
    free(node->label);
    free(node);
}

void print_tree(const TreeNode* node, int level) {
    if (!node) return;

    for (int i = 0; i < level; i++) printf("  ");
    printf("%s\n", node->label);

    for (int i = 0; i < node->child_count; i++) {
        print_tree(node->children[i], level + 1);
    }
}

void save_tree(const TreeNode* node, FILE* file, int level) {
    if (!node || !file) return;

    for (int i = 0; i < level; i++) fprintf(file, "  ");
    fprintf(file, "%s\n", node->label);

    for (int i = 0; i < node->child_count; i++) {
        save_tree(node->children[i], file, level + 1);
    }
}
