/*--------------------------------------------------------------------*/
/* FT_file.h.c                                                          */
/* Authors: Misrach Ewunetie, Shruti Roy                              */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "dynarray.h"
#include "FT_file.h"
#include "checkerDT.h"

/*
   A File structure represents a directory in the directory tree
*/
struct file {
   /* the full path of this directory */
   char* path;

   /* the parent directory of this directory
      NULL for the root of the directory tree */
   File_T parent;
   
   /* conent contained in of the file File */
   void *contents;

   /* size of conents */
   size_t length;
};

/* see FT_file.h for specification */
File_T File_create(const char* dir, File_T parent){
   File_T new;
   int result;
   char* path;

   assert(parent == NULL || CheckerDT_File_isValid(parent));
   assert(dir != NULL);

   new = malloc(sizeof(struct file));
   if(new == NULL) {
      assert(parent == NULL || CheckerDT_File_isValid(parent));
      return NULL;
   }


   if(parent == NULL)
      path = malloc(strlen(dir)+1);
   else
      path = malloc(strlen(parent->path) + 1 + strlen(dir) + 1);

   if(path == NULL) {
      free(new);
      assert(parent == NULL || CheckerDT_File_isValid(parent));
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
   if (!file) {
       new->fchildren = DynArray_new(0);
       new->dchildren = DynArray_new(0);
   }

   if(new->fchildren == NULL || new->dchildren == NULL) {
      if (new->dchildren != NULL) {
          DynArray_free(new->dchildren);
      }
      else if (new->fchildren != NULL) {
          DynArray_free(new->fchildren);
      }
      free(new->path);
      free(new);
      assert(parent == NULL || CheckerDT_File_isValid(parent));
      return NULL;
   }

    if (parent != NULL) {
        result = File_linkChild(parent, new, file);
        if(result != SUCCESS)
            (void) File_destroy(new, file);
        else
            assert(CheckerDT_File_isValid(new));

        assert(CheckerDT_File_isValid(parent));
    }


   assert(parent == NULL || CheckerDT_File_isValid(parent));
   assert(CheckerDT_File_isValid(new));
   return new;
}

/* see FT_file.h for specification */
size_t File_destroy(File_T n) {
   size_t i;
   size_t count = 0;
   File_T c;

   assert(n != NULL);
   
   if (n->parent != NULL) {
       File_unlinkChild(n->parent, n, file);
   }

    if(!file) {
        for(i = 0; i < DynArray_getLength(n->fchildren); i++)
        {
            c = DynArray_get(n->fchildren, i);
            count += File_destroy(c);
        }
        DynArray_free(n->fchildren);

        for(i = 0; i < DynArray_getLength(n->dchildren); i++)
        {
            c = DynArray_get(n->dchildren, i);
            count += File_destroy(c);
        }
        DynArray_free(n->dchildren);
    }

   free(n->path);
   free(n);
   count++;

   return count;
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
size_t File_getNumChildren(File_T n) {
    DynArray_T children;
   assert(n != NULL);

    if(file)
        children = n->fchildren;
    else
        children = n->dchildren;
   return DynArray_getLength(children);
}

/* see FT_file.h for specification */
int File_hasChild(File_T n, const char* path, size_t* childID) {
   size_t index;
   int result;
   File_T checker;
   DynArray_T children;

   assert(n != NULL);
   assert(path != NULL);

   checker = File_create(path, NULL, file);
   if(checker == NULL) {
      return -1;
   }

   if(file)
        children = n->fchildren;
    else
        children = n->dchildren;

   result = DynArray_bsearch(children, checker, &index,
                    (int (*)(const void*, const void*)) File_compare);
   (void) File_destroy(checker, file);

   if(childID != NULL)
      *childID = index;

   return result;
}

/* see FT_file.h for specification */
File_T File_getChild(File_T n, size_t childID) {
    DynArray_T children;
   assert(n != NULL);

   if(file)
        children = n->fchildren;
    else
        children = n->dchildren;

   if(DynArray_getLength(children) > childID) {
      return DynArray_get(children, childID);
   }
   else {
      return NULL;
   }
}

/* see FT_file.h for specification */
File_T File_getParent(File_T n) {
   assert(n != NULL);

   return n->parent;
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
static int File_linkChild(File_T parent, File_T child) {
   size_t i;
   char* rest;
    DynArray_T children;

   assert(parent != NULL);
   assert(child != NULL);
   assert(CheckerDT_File_isValid(parent));
   assert(CheckerDT_File_isValid(child));

   if(File_hasChild(parent, child->path, NULL, file)) {
      assert(CheckerDT_File_isValid(parent));
      assert(CheckerDT_File_isValid(child));
      return ALREADY_IN_TREE;
   }
   i = strlen(parent->path);
   if(strncmp(child->path, parent->path, i)) {
      assert(CheckerDT_File_isValid(parent));
      assert(CheckerDT_File_isValid(child));
      return PARENT_CHILD_ERROR;
   }
   rest = child->path + i;
   if(strlen(child->path) >= i && rest[0] != '/') {
      assert(CheckerDT_File_isValid(parent));
      assert(CheckerDT_File_isValid(child));
      return PARENT_CHILD_ERROR;
   }
   rest++;
   if(strstr(rest, "/") != NULL) {
      assert(CheckerDT_File_isValid(parent));
      assert(CheckerDT_File_isValid(child));
      return PARENT_CHILD_ERROR;
   }
   child->parent = parent;

   if(file)
        children = parent->fchildren;
    else
        children = parent->dchildren;

   if(DynArray_bsearch(children, child, &i,
         (int (*)(const void*, const void*)) File_compare) == 1) {
      assert(CheckerDT_File_isValid(parent));
      assert(CheckerDT_File_isValid(child));
      return ALREADY_IN_TREE;
   }

    if(DynArray_bsearch(children, child, &i,
         (int (*)(const void*, const void*)) File_compare) == 1) {
      assert(CheckerDT_File_isValid(parent));
      assert(CheckerDT_File_isValid(child));
      return ALREADY_IN_TREE;
   }

   if(DynArray_addAt(children, i, child) == TRUE) {
      assert(CheckerDT_File_isValid(parent));
      assert(CheckerDT_File_isValid(child));
      return SUCCESS;
   }
   else {
      assert(CheckerDT_File_isValid(parent));
      assert(CheckerDT_File_isValid(child));
      return PARENT_CHILD_ERROR;
   }
}

/*
  Unlinks File parent from its child File child, if it can be found in the parent's children. child is unchanged.
 */
static void File_unlinkChild(File_T parent, File_T child) {
   size_t i;
   DynArray_T children;

   assert(parent != NULL);
   assert(child != NULL);
   assert(CheckerDT_File_isValid(parent));
   assert(CheckerDT_File_isValid(child));

   if(file)
        children = parent->fchildren;
    else
        children = parent->dchildren;

   if(DynArray_bsearch(children, child, &i,
         (int (*)(const void*, const void*)) File_compare) != 0)
        (void) DynArray_removeAt(children, i);

   assert(CheckerDT_File_isValid(parent));
   assert(CheckerDT_File_isValid(child));
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
