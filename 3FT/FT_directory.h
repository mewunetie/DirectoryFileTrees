/*--------------------------------------------------------------------*/
/* node.h                                                             */
/* Author: Christopher Moretti                                        */
/*--------------------------------------------------------------------*/

#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include <stddef.h>
#include "a4def.h"

/*
   a Node_T is an object that contains a path payload and references to
   the node's parent (if it exists) and children (if they exist).
*/
typedef struct node* Node_T;


/*
   Given a parent node and a directory string dir, returns a new
   Node_T or NULL if any allocation error occurs in creating
   the node or its fields.

   The new structure is initialized to have its path as the parent's
   path (if it exists) prefixed to the directory string parameter,
   separated by a slash. It is also initialized with its parent link
   as the parent parameter value, and the parent itself is changed
   to link to the new node.  The children links are initialized but
   do not point to any children.
*/

Node_T Node_create(const char* dir, Node_T parent);

/*
  Destroys the entire hierarchy of nodes rooted at n,
  including n itself.

  Returns the number of nodes destroyed.
*/
size_t Node_destroy(Node_T n, boolean file);


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
  Returns the number of child directories n has.
*/
size_t Node_getNumChildren(Node_T n, boolean file);

/*
   Returns 1 if n has a child directory with path,
   0 if it does not have such a child, and -1 if
   there is an allocation error during search.

   If n does have such a child, and childID is not NULL, store the
   child's identifier in *childID. If n does not have such a child,
   store the identifier that such a child would have in *childID.
*/
int Node_hasChild(Node_T n, const char* path, size_t* childID, boolean file);

/*
   Returns the child node of n with identifier childID, if one exists,
   otherwise returns NULL.
*/
Node_T Node_getChild(Node_T n, size_t childID, boolean file);

/*
   Returns the parent node of n, if it exists, otherwise returns NULL
*/
Node_T Node_getParent(Node_T n);


/*
  Returns a string representation for n, 
  or NULL if there is an allocation error.

  Allocates memory for the returned string,
  which is then owned by client!
*/
char* Node_toString(Node_T n);

#endif
