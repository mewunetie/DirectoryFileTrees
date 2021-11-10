/*--------------------------------------------------------------------*/
/* node.c                                                             */
/* Authors: Misrach Ewunetie, Shruti Roy                              */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "dynarray.h"
#include "node.h"

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



/* see node.h for specification */
Node_T Node_create(const char* dir, Node_T parent){
   Node_T new;
   char* path;

   assert(dir != NULL);

   new = malloc(sizeof(struct node));
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

   if(new->fchildren == NULL || new->dchildren == NULL) {
      if (new->dchildren != NULL) {
          DynArray_free(new->dchildren);
      }
      else if (new->fchildren != NULL) {
          DynArray_free(new->fchildren);
      }
      free(new->path);
      free(new);
      *status = MEMORY_ERROR;
      return NULL;
   }

   /* if (parent != NULL) {
      result = Node_linkChild(parent, new);
      if(result != SUCCESS)
         Node_destroy(new);
      *status = result;
      } */

   return new;
}

/* see node.h for specification */
size_t Node_destroy(Node_T n) {
   size_t i;
   size_t count = 0;
   Node_T d;
   File_T f;
   
   assert(n != NULL);
   
   /* if (n->parent != NULL) {
       Node_unlinkChild(n->parent, n);
       } */

   for(i = 0; i < DynArray_getLength(n->fchildren); i++)
   {
      f = DynArray_get(n->fchildren, i);
      File_destroy(f);
      count++;
   }
   DynArray_free(n->fchildren);

   for(i = 0; i < DynArray_getLength(n->dchildren); i++)
   {
      d = DynArray_get(n->dchildren, i);
      count += Node_destroy(d);
   }
   DynArray_free(n->dchildren);

   free(n->path);
   free(n);
   count++;

   return count;
}

/* see node.h for specification */
char* Node_getPath(Node_T n) {
   char* pathCopy;

   assert(n != NULL);

   pathCopy = malloc(strlen(n->path)+1);
   if(pathCopy == NULL)
      return NULL;

   return strcpy(pathCopy, n->path);
}


/* see node.h for specification */
int Node_compare(Node_T node1, Node_T node2) {
   assert(node1 != NULL);
   assert(node2 != NULL);

   return strcmp(node1->path, node2->path);
}

/* see node.h for specification */
size_t Node_getNumChildren(Node_T n, boolean file) {
   DynArray_T children;
   assert(n != NULL);

   if(file)
       children = n->fchildren;
   else
       children = n->dchildren;
   return DynArray_getLength(children);
}

/* see node.h for specification */
int Node_hasChild(Node_T n, const char* path, size_t* childID,
   boolean file) {
   size_t index;
   int result;
   Node_T checker;
   DynArray_T children;

   assert(n != NULL);
   assert(path != NULL);

   checker = Node_create(path, NULL, &result);
   if(checker == NULL) {
      return -1;
   }

   if(file)
        children = n->fchildren;
    else
        children = n->dchildren;

   result = DynArray_bsearch(children, checker, &index,
                    (int (*)(const void*, const void*)) Node_compare);
   (void) Node_destroy(checker);

   if(childID != NULL)
      *childID = index;

   return result;
}

/* see node.h for specification */
Node_T Node_getDirChild(Node_T n, size_t childID) {
   assert(n != NULL);

   if(DynArray_getLength(n->dchildren) > childID)
      return DynArray_get(n->dchildren, childID);
   else
      return NULL;
}

File_T Node_getFileChild(Node_T n, size_t childID) {
   assert(n != NULL);

   if(DynArray_getLength(n->fchildren) > childID)
      return DynArray_get(n->fchildren, childID);
   else
      return NULL;
}

/* see node.h for specification */
Node_T Node_getParent(Node_T n) {
   assert(n != NULL);

   return n->parent;
}

/* see node.h for specification */
int Node_linkChild(Node_T parent, Node_T child) {
   size_t i;
   char* rest;

   assert(parent != NULL);
   assert(child != NULL);

   if(Node_hasChild(parent, child->path, NULL, FALSE))
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

   if(DynArray_bsearch(parent->dchildren, child, &i,
         (int (*)(const void*, const void*)) Node_compare) == 1)
      return ALREADY_IN_TREE;

    if(DynArray_bsearch(parent->dchildren, child, &i,
         (int (*)(const void*, const void*)) Node_compare) == 1)
      return ALREADY_IN_TREE;

   if(DynArray_addAt(parent->dchildren, i, child) == TRUE)
      return SUCCESS;
   else
      return PARENT_CHILD_ERROR;
}

/* see node.h for specification */
void Node_unlinkChild(Node_T parent, Node_T child) {
   size_t i;

   assert(parent != NULL);
   assert(child != NULL);

   if(DynArray_bsearch(parent->dchildren, child, &i,
         (int (*)(const void*, const void*)) Node_compare) != 0)
        (void) DynArray_removeAt(parent->dchildren, i);
}


/* see node.h for specification */
char* Node_toString(Node_T n) {
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

/* see FT_file.h for specification */
File_T File_create(const char* dir, Node_T parent, void* contents,
                   size_t length)
{
   File_T new;
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

   new->contents = malloc(length);

   if(new->contents == NULL) {
      free(new->path);
      free(new);
      *status = MEMORY_ERROR;
      return NULL;
   }

   memcpy(new->contents, contents, length);

   /* if (parent != NULL) {
      result = Node_linkChild(parent, new, file);
      if(result != SUCCESS)
         (void) File_destroy(new, file);
      *status = result;
      } */

   return new;
}

/* see FT_file.h for specification */
void File_destroy(File_T n) {
   assert(n != NULL);

   /* if (n->parent != NULL) {
      File_unlinkChild(n->parent, n);
      } */

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

/* see node.h for specification */
int File_linkChild(Node_T parent, File_T child) {
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

/* see node.h for specification */
void File_unlinkChild(Node_T parent, File_T child) {
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
