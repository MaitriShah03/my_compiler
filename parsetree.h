#ifndef PARSETREE_H
#define PARSETREE_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Tree node structure for abstract syntax trees
 * 
 * Represents a node in the parse tree with:
 * - A label (stored as "TYPE:VALUE" where :VALUE is optional)
 * - Child count
 * - Array of child nodes
 */
typedef struct TreeNode {
    char* label;           ///< Node type and optional value (format "TYPE:VALUE")
    int child_count;       ///< Number of child nodes
    struct TreeNode** children; ///< Array of child node pointers
} TreeNode;

/* ==================== Node Creation Functions ==================== */

/**
 * @brief Create a new tree node with the given label and children
 * @param label The node label/type
 * @param child_count Number of child nodes
 * @param ... Variable arguments list of child nodes (TreeNode*)
 * @return Newly allocated TreeNode
 */
TreeNode* make_node(const char* label, int child_count, ...);

/**
 * @brief Create a leaf node (node with no children)
 * @param label The node label/type
 * @return Newly allocated TreeNode
 */
TreeNode* make_leaf(const char* label);

/**
 * @brief Create a leaf node with a string value
 * @param label The node type
 * @param value The string value (stored as "TYPE:VALUE")
 * @return Newly allocated TreeNode
 */
TreeNode* make_leaf_str(const char* label, const char* value);

/**
 * @brief Create a leaf node specifically for type information
 * @param type The type name (e.g., "int", "float")
 * @return Newly allocated TreeNode with label "TYPE:<type>"
 */
TreeNode* make_leaf_type(const char* type);

/* ==================== Tree Operations ==================== */

/**
 * @brief Print the tree structure to stdout
 * @param node Root node of the tree to print
 * @param level Current indentation level (start with 0)
 */
void print_tree(const TreeNode* node, int level);

/**
 * @brief Save the tree structure to a file
 * @param node Root node of the tree to save
 * @param file FILE pointer to write to
 * @param level Current indentation level (start with 0)
 */
void save_tree(const TreeNode* node, FILE* file, int level);

/**
 * @brief Recursively free all memory used by the tree
 * @param node Root node of the tree to free
 */
void free_tree(TreeNode* node);

/* ==================== Node Value Access ==================== */

/**
 * @brief Get the value portion of a node's label (after colon)
 * @param node The node to examine
 * @return Pointer to the value portion, or NULL if no value exists
 */
const char* get_node_value(const TreeNode* node);

/**
 * @brief Get the type portion of a node's label (before colon)
 * @param node The node to examine
 * @return Pointer to the type portion, or full label if no colon exists
 */
const char* get_node_type(const TreeNode* node);

/**
 * @brief Check if a node has a specific type
 * @param node The node to check
 * @param type The type to compare against
 * @return 1 if node matches type, 0 otherwise
 */
int is_node_type(const TreeNode* node, const char* type);

#endif // PARSETREE_H