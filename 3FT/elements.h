/*--------------------------------------------------------------------*/
/* elements.h                                                             */
/* Author: Shruti Roy, Misrach Ewunetie                               */
/*--------------------------------------------------------------------*/

#ifndef ELEMENTS_INCLUDED
#define ELEMENTS_INCLUDED

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

#endif
