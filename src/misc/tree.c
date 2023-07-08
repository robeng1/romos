#include "tree.h"
#include "dll.h"

/*
 * Create a tree with root = NULL
 * */
tree_t *tree_create()
{
  return (tree_t *)kernel_zalloc(sizeof(tree_t));
}

/*
 * Create a tree node with specified value, and a list of 0 children
 * */
tree_node_t *treenode_create(void *value)
{
  tree_node_t *n = kernel_zalloc(sizeof(tree_node_t));
  n->value = value;
  n->children = list_create();
  return n;
}

/*
 * Insert a node under subroot
 * */
tree_node_t *tree_insert(tree_t *tree, tree_node_t *subroot, void *value)
{
  // Create a treenode
  tree_node_t *treenode = kernel_zalloc(sizeof(tree_node_t));
  treenode->children = list_create();
  treenode->value = value;

  // Insert it
  if (!tree->root)
  {
    tree->root = treenode;
    return treenode;
  }
  list_insert_front(subroot->children, treenode);
  return treenode;
}

tree_node_t *tree_find_parent(tree_t *tree, tree_node_t *remove_node, int *child_index)
{
  // If subroot is the parent
  if (remove_node == tree->root)
    return NULL;
  return tree_find_parent_recur(tree, remove_node, tree->root, child_index);
}

tree_node_t *tree_find_parent_recur(tree_t *tree, tree_node_t *remove_node, tree_node_t *subroot, int *child_index)
{
  int idx;
  if ((idx = list_contain(subroot->children, remove_node)) != -1)
  {
    *child_index = idx;
    return subroot;
  }
  foreach (child, subroot->children)
  {
    tree_node_t *ret = tree_find_parent_recur(tree, remove_node, child->val, child_index);
    if (ret != NULL)
    {
      return ret;
    }
    // ret is NULL, keep searching.
  }
  return NULL;
}

void tree_remove(tree_t *tree, tree_node_t *remove_node)
{
  // Search for tree's parent and remove the node from parent's children list
  // If parent is NULL, then just set tree->root to NULL(yeah, I dont care about mem leaks)
  int child_index = -1;
  tree_node_t *parent = tree_find_parent(tree, remove_node, &child_index);
  // Do treenode remove in here:
  if (parent != NULL)
  {
    tree_node_t *freethis = list_remove_by_index(parent->children, child_index);
    // Free tree node here
    kernel_free(freethis);
  }
}

void tree_to_list_recur(tree_node_t *subroot, list_t *list)
{
  if (subroot == NULL)
    return;
  foreach (child, subroot->children)
  {
    tree_node_t *curr_treenode = (tree_node_t *)child->val;
    void *curr_val = curr_treenode->value;
    list_insert_back(list, curr_val);
    tree_to_list_recur(child->val, list);
  }
}

void tree_to_list(tree_t *tree, list_t *list)
{
  tree_to_list_recur(tree->root, list);
}

void tree_to_array(tree_t *tree, void **array, int *size)
{
  tree_to_array_recur(tree->root, array, size);
}

void tree_to_array_recur(tree_node_t *subroot, void **array, int *size)
{
  if (subroot == NULL)
    return;
  void *curr_val = (void *)subroot->value;
  array[*size] = curr_val;
  *size = *size + 1;
  foreach (child, subroot->children)
  {
    tree_to_array_recur(child->val, array, size);
  }
}
