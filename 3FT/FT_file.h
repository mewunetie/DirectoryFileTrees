/*--------------------------------------------------------------------*/
/* FT_file.h                                                             */
/* Authors: Shruti Roy, Misrach Ewunetie                              */
/*--------------------------------------------------------------------*/

#ifndef File_INCLUDED
#define File_INCLUDED

#include <stddef.h>
#include "a4def.h"

/*
   a File_T is an object that contains a path payload and references to
   the File's parent (if it exists) and children (if they exist).
*/
typedef struct file* File_T;

#include "FT_directory.c"

/*
   Given a parent File and a directory string dir, returns a new
   File_T or NULL if any allocation error occurs in creating
   the File or its fields.

   The new structure is initialized to have its path as the parent's
   path (if it exists) prefixed to the directory string parameter,
   separated by a slash. It is also initialized with its parent link
   as the parent parameter value, and the parent itself is changed
   to link to the new File. status is changed to the result of the
   function, i.e. SUCCESS if successful, ALREADY_IN_TREE if the file
   already exists at that path, MEMORY_ERROR if it cannot allocate some
   memory, and PARENT_CHILD_ERROR if a parent cannot link to the child.
*/

File_T File_create(const char* dir, Node_T parent, void* contents,
                   size_t length, int *status);

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
  Returns a string representation for n, 
  or NULL if there is an allocation error.

  Allocates memory for the returned string,
  which is then owned by client!
*/
char* File_ToString(File_T n);

#endif
