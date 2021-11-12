/*--------------------------------------------------------------------*/
/* node.h                                                             */
/* Author: Shruti Roy, Misrach Ewunetie                               */
/*--------------------------------------------------------------------*/

#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include <stddef.h>
#include "a4def.h"
#include "elements.h"

/*
   Given a parent node and a directory string dir, returns a new
   Node_T or NULL if any allocation error occurs in creating
   the node or its fields.

   The new structure is initialized to have its path as the parent's
   path (if it exists) prefixed to the directory string parameter,
   separated by a slash. It is also initialized with its parent link
   as the parent parameter value, but the parent itself is not changed
   to link to the new node.  The children links are initialized but
   do not point to any children.
*/
Node_T Node_create(const char* dir, Node_T parent);

/*
  Destroys the entire hierarchy of nodes rooted at n,
  including n itself.
*/
size_t Node_destroy(Node_T n);

/*
  Compares node1 and node2 based on their paths.
  Returns <0, 0, or >0 if node1 is less than,
  equal to, or greater than node2, respectively.
*/
int Node_compare(Node_T node1, Node_T node2);

/*
   Returns n's path.
*/
char* Node_getPath(Node_T n);

/*
  Returns the number of child directories n has, if file is false.
  Returnes the number of child files n has, if file is true.
*/
size_t Node_getNumChildren(Node_T n, boolean file);

/*
  Returns 1 if n has a child directory with path,
  0 if it does not have such a directory child, and -1 if
  there is an allocation error during search.

  If n does have such a child, and childID is not NULL, store the
  child's identifier in *childID. If n does not have such a child,
  store the identifier that such a child would have in *childID.
*/
int Node_hasDirChild(Node_T n, const char* path, size_t *childID);

/*
   Returns 1 if n has a child file with path,
   0 if it does not have such a file child, and -1 if
   there is an allocation error during search.

   If n does have such a child, and childID is not NULL, store the
   child's identifier in *childID. If n does not have such a child,
   store the identifier that such a child would have in *childID.
*/
int Node_hasFileChild(Node_T n, const char* path, size_t* childID);

/*
   Returns the child node of n with identifier childID, if one exists,
   otherwise returns NULL.
*/
Node_T Node_getDirChild(Node_T n, size_t childID);

/* Returns the child file of n with identifier childID, if one exists,
   otherwise returns NULL.
*/
File_T Node_getFileChild(Node_T n, size_t childID);

/*
   Returns the parent node of n, if it exists, otherwise returns NULL
*/
Node_T Node_getParent(Node_T n);

/*
  Makes child a child of parent, if possible, and returns SUCCESS.
  This is not possible in the following cases:
  * parent already has a child with child's path,
    in which case: returns ALREADY_IN_TREE
    * child's path is not parent's path + / + directory,
    or the parent cannot link to the child,
    in which cases: returns PARENT_CHILD_ERROR
*/
int Node_linkChild(Node_T parent, Node_T child);

/*
  Unlinks node parent from its node child, if it can be found in 
  the parent's children. child is unchanged.
*/
void Node_unlinkChild(Node_T parent, Node_T child);

/*
  Returns a string representation for n, 
  or NULL if there is an allocation error.

  Allocates memory for the returned string,
  which is then owned by client!
*/
char* Node_toString(Node_T n);

#endif
