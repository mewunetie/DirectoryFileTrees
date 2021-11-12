/*--------------------------------------------------------------------*/
/* file.c                                                             */
/* Authors: Misrach Ewunetie, Shruti Roy                              */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "dynarray.h"
#include "node.h"
#include "file.h"

/*
   A node structure represents a directory in the directory tree
*/
struct node {
   /* the full path of this directory */
   char* path;

   /* the parent directory of this directory
      NULL for the root of the directory tree */
   Node_T parent;

   /* the files of this directory
      stored in sorted order by pathname */
   DynArray_T fchildren;

   /* the subdirectories of this directory
      stored in sorted order by pathname */
   DynArray_T dchildren;
};

/*
   A File structure represents a directory in the directory tree
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
File_T File_create(const char* dir, Node_T parent, void* contents,
                   size_t length)
{
   File_T new;
   char* path;

   assert(dir != NULL);

   new = malloc(sizeof(struct file));
   if(new == NULL)
      return NULL;

   if(parent == NULL)
      path = malloc(strlen(dir)+1);
   else
      path = malloc(strlen(parent->path) + 1 + strlen(dir) + 1);

   if(path == NULL) {
      free(new);
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
   new->contents = contents;
   new->length = length;

   return new;
}

/* see FT_file.h for specification */
void File_destroy(File_T n) {
   assert(n != NULL);

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
   assert(n != NULL);

   return n->contents;
}

/* see FT_file.h for specification */
void* File_replaceContents(File_T n, void *contents, size_t length) {
   void* original;
   assert(n != NULL);

   original = n->contents;

   n->contents = contents;
   n->length = length;

   return original;
}

/* see node.h for specification */
size_t File_getContentLength(File_T n) {
   assert (n != NULL);

   return n->length;
}

/* see node.h for specification */
int File_linkChild(Node_T parent, File_T child) {
   size_t i;
   char* rest;

   assert(parent != NULL);
   assert(child != NULL);

   if(Node_hasFileChild(parent, child->path, NULL))
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
                       (int (*)(const void*, const void*))
                       File_compare) == 1)
      return ALREADY_IN_TREE;

   if(DynArray_bsearch(parent->fchildren, child, &i,
                       (int (*)(const void*, const void*))
                       File_compare) == 1)
      return ALREADY_IN_TREE;

   if(DynArray_addAt(parent->fchildren, i, child) == TRUE)
      return SUCCESS;
   else
      return PARENT_CHILD_ERROR;
}

/* see node.h for specification */
void File_unlinkChild(Node_T parent, File_T child) {
   size_t i = -1;

   assert(parent != NULL);
   assert(child != NULL);

   if(DynArray_bsearch(parent->fchildren, child, &i,
                       (int (*)(const void*, const void*))
                       File_compare) != 0)
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
