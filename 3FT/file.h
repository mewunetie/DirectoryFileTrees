/*--------------------------------------------------------------------*/
/* file.h                                                             */
/* Author: Shruti Roy, Misrach Ewunetie                               */
/*--------------------------------------------------------------------*/

#ifndef FILE_INCLUDED
#define FILE_INCLUDED

#include <stddef.h>
#include "a4def.h"
#include "elements.h"

/*
   Given a parent node and a file string fname, returns a new
   File_T or NULL if any allocation error occurs in creating
   the File or its fields.
   The new structure is initialized to have its path as the parent's
   path, which must exist, prefixed to the directory string parameter,
   separated by a slash. It is also initialized with its parent link
   as the parent parameter value, but the parent itself is not changed
   to link to the new File.
*/

File_T File_create(const char* fname, Node_T parent, void* contents,
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

/* Returns a pointer to the content contained within the File of n, 
   if it exists, otherwise returns NULL. The caller owns the content 
   of the file. 
*/
void* File_getContents(File_T n);

/* Replaces the content of the file n with the contents passed in,
   and the length with the new length passed in. Returns a pointer
   to the original contents of file n, which the client owns.
*/
void* File_replaceContents(File_T n, void *contents, size_t length);

/* Returns the length of the contents of the file */
size_t File_getContentLength(File_T n);

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
  Unlinks File parent from its File child, if it can be found in 
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
