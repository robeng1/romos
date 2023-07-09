#ifndef GENERIC_TREE_H
#define GENERIC_TREE_H
#include <system.h>
#include <dll.h>

typedef struct tree_node
{
  list_t *children;
  void *value;
} tree_node_t;

typedef struct tree
{
  tree_node_t *root;
} tree_t;

tree_t *tree_create();

tree_node_t *treenode_create(void *value);

tree_node_t *tree_insert(tree_t *tree, tree_node_t *subroot, void *value);

tree_node_t *tree_find_parent(tree_t *tree, tree_node_t *remove_node, int *child_index);

tree_node_t *tree_find_parent_recur(tree_t *tree, tree_node_t *remove_node, tree_node_t *subroot, int *child_index);

void tree_remove(tree_t *tree, tree_node_t *remove_node);

void tree_to_list_recur(tree_node_t *subroot, list_t *list);

void tree_to_list(tree_t *tree, list_t *list);

void tree_to_array(tree_t *tree, void **array, int *size);

void tree_to_array_recur(tree_node_t *subroot, void **array, int *size);

#endif