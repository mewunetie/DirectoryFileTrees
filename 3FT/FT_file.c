/*--------------------------------------------------------------------*/
/* FT_file.c                                                          */
/* Authors: Misrach Ewunetie, Shruti Roy                              */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "dynarray.h"
#include "FT_file.h"
#include "FT_node.h"

/*
   A File structure represents a file in the directory tree
*/
struct file {
   /* the full path of this directory */
   char* path;

   /* the parent directory of this directory
      NULL for the root of the directory tree */
   Node_T parent;
   
   /* content contained in of the file File */
   void *contents;

   /* size of contents */
   size_t length;
};

/* see FT_file.h for specification */
File_T File_create(const char* dir, Node_T parent, void* contents
                   size_t length, int* status)
{
   File_T new;
   int result;
   char* path;

   assert(dir != NULL);

   new = malloc(sizeof(struct file));
   if(new == NULL) {
      *status = MEMORY_ERROR;
      return NULL;
   }

   if(parent == NULL)
      path = malloc(strlen(dir)+1);
   else
      path = malloc(strlen(parent->path) + 1 + strlen(dir) + 1);

   if(path == NULL) {
      free(new);
      *status = MEMORY_ERROR;
      return NULL;
   }

   *path = '\0';

   if(parent != NULL) {
      strcpy(path, parent->path);
      strcat(path, "/");
   }
   strcat(path, dir);

   new->path = path;

   new->parent = parent;

   new->contents = malloc(size_t length);
   
   if(new->contents == NULL) {
      free(new->path);
      free(new);
      *status = MEMORY_ERROR;
      return NULL;
   }

   memcpy(new->contents, contents, length);

   new->length = length;
   
   if (parent != NULL) {
       result = Node_linkChild(parent, new, file);
       if(result != SUCCESS)
           (void) File_destroy(new, file);
       *status = result;
   }

   return new;
}

/* see FT_file.h for specification */
void File_destroy(File_T n) {
   assert(n != NULL);
   
   if (n->parent != NULL) {
       File_unlinkChild(n->parent, n);
   }

   free(n->path);
   free(n);
}

/* see FT_file.h for specification */
char* File_getPath(File_T n) {
   char* pathCopy;

   assert(n != NULL);

   pathCopy = malloc(strlen(n->path)+1);
   if(pathCopy == NULL)
      return NULL;

   return strcpy(pathCopy, n->path);
}


/* see FT_file.h for specification */
int File_compare(File_T File1, File_T File2) {
   assert(File1 != NULL);
   assert(File2 != NULL);

   return strcmp(File1->path, File2->path);
}

/* see FT_file.h for specification */
Node_T File_getParent(File_T n) {
   assert(n != NULL);

   return n->parent;
}

/* see FT_file.h for specification */
void* File_getContents(File_T n) {
   void* contents;

   assert(n != NULL);

   contents = malloc(n->length);
   if (contents == NULL)
      return NULL;

   memcpy(contents, n->contents, n->length);
   return contents;
}

/* see FT_file.h for specification */
void* File_replaceContents(File_T n, void *contents, size_t length) {
   void* original;
   assert(n != NULL);

   original = n->contents;

   n->contents = malloc(length);
   if (n->contents == NULL)
      return NULL;

   memcpy(n->contents, contents, length);
   return original;
}

/*
  Makes child a child of parent, if possible, and returns SUCCESS.
  This is not possible in the following cases:
  * parent already has a child with child's path,
    in which case: returns ALREADY_IN_TREE
  * child's path is not parent's path + / + directory,
    or the parent cannot link to the child,
    in which cases: returns PARENT_CHILD_ERROR
 */
static int File_linkChild(Node_T parent, File_T child) {
   size_t i;
   char* rest;

   assert(parent != NULL);
   assert(child != NULL);

   if(Node_hasChild(parent, child->path, NULL, TRUE))
      return ALREADY_IN_TREE;
   
   i = strlen(parent->path);
   if(strncmp(child->path, parent->path, i))
      return PARENT_CHILD_ERROR;
      
   rest = child->path + i;
   if(strlen(child->path) >= i && rest[0] != '/')
      return PARENT_CHILD_ERROR;

   rest++;
   if(strstr(rest, "/") != NULL)
      return PARENT_CHILD_ERROR;

   child->parent = parent;

   if(DynArray_bsearch(parent->fchildren, child, &i,
         (int (*)(const void*, const void*)) File_compare) == 1)
      return ALREADY_IN_TREE;

   if(DynArray_bsearch(parent->fchildren, child, &i,
         (int (*)(const void*, const void*)) File_compare) == 1)
      return ALREADY_IN_TREE;

   if(DynArray_addAt(parent->fchildren, i, child) == TRUE)
      return SUCCESS;
   else
      return PARENT_CHILD_ERROR;
}

/*
  Unlinks File parent from its child File child, if it can be found in 
  the parent's children. child is unchanged.
 */
static void File_unlinkChild(Node_T parent, File_T child) {
   size_t i;

   assert(parent != NULL);
   assert(child != NULL);

   if(DynArray_bsearch(parent->fchildren, child, &i,
         (int (*)(const void*, const void*)) File_compare) != 0)
        (void) DynArray_removeAt(parent->fchildren, i);
}


/* see FT_file.h for specification */
char* File_ToString(File_T n) {
   char* copyPath;

   assert(n != NULL);

   copyPath = malloc(strlen(n->path)+1);
   if(copyPath == NULL) {
      return NULL;
   }
   else {
      return strcpy(copyPath, n->path);
   }
}
