/*--------------------------------------------------------------------*/
/* node.h                                                             */
/* Author: Shruti Roy, Misrach Ewunetie                               */
/*--------------------------------------------------------------------*/

#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include <stddef.h>
#include "a4def.h"

/*
   a Node_T is an object that contains a path payload and references to
   the node's parent (if it exists) and children (if they exist).
   Children may be either files, or other node directories.
*/
typedef struct node* Node_T;

/*
   a File_T is an object that contains a path payload and references to
   the File's parent (if it exists) and children (if they exist).
*/
typedef struct file* File_T;

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
int Node_hasChild(Node_T n, const char* path, size_t* childID,
                  boolean file);

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
  Unlinks node parent from its child node child, if it can be found in 
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

/*
   Given a parent File and a directory string dir, returns a new
   File_T or NULL if any allocation error occurs in creating
   the File or its fields.
   The new structure is initialized to have its path as the parent's
   path (if it exists) prefixed to the directory string parameter,
   separated by a slash. It is also initialized with its parent link
   as the parent parameter value, but the parent itself is not changed
   to link to the new File.
*/

File_T File_create(const char* dir, Node_T parent, void* contents,
                   size_t length);

/*
  Destroys the file n.
*/
void File_destroy(File_T n);


/*
  Compares File1 and File2 based on their paths.
  Returns <0, 0, or >0 if File1 is less than
  equal to, or greater than File2, respectively.
*/
int File_compare(File_T File1, File_T File2);

/*
   Returns n's path.
*/
char* File_getPath(File_T n);

/*
   Returns the parent Node of n, if it exists, otherwise returns NULL
*/
Node_T File_getParent(File_T n);

/* Returns a copy of the content contained within the File of n, 
   if it exists, otherwise returns NULL. The caller will then own the 
   memory allocated for the copy of the content of the file 
*/
void* File_getContents(File_T n);

/* Replaces the content of the file n with the contents passed in using
   the length of the contents in order to replace the contents.
   Returns a copy of the contents of file n, for which the client is
   now responsible.
*/
void* File_replaceContents(File_T n, void *contents, size_t length);

/*
  Makes child a child of parent, if possible, and returns SUCCESS.
  This is not possible in the following cases:
  * parent already has a child with child's path,
    in which case: returns ALREADY_IN_TREE
    * child's path is not parent's path + / + directory,
    or the parent cannot link to the child,
    in which cases: returns PARENT_CHILD_ERROR
*/
int File_linkChild(Node_T parent, File_T child);

/*
  Unlinks File parent from its child File child, if it can be found in 
  the parent's children. child is unchanged.
*/
void File_unlinkChild(Node_T parent, File_T child);

/*
  Returns a string representation for n, 
  or NULL if there is an allocation error.
  Allocates memory for the returned string,
  which is then owned by client!
*/
char* File_ToString(File_T n);

#endif
