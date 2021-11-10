/*--------------------------------------------------------------------*/
/* FT_node.c                                                          */
/* Authors: Misrach Ewunetie, Shruti Roy                              */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "dynarray.h"
#include "FT_node.h"
#include "checkerDT.h"

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


/* see node.h for specification */
Node_T Node_create(const char* dir, Node_T parent){
   Node_T new;
   int result;
   char* path;

   assert(parent == NULL || CheckerDT_Node_isValid(parent));
   assert(dir != NULL);

   new = malloc(sizeof(struct node));
   if(new == NULL) {
      assert(parent == NULL || CheckerDT_Node_isValid(parent));
      return NULL;
   }


   if(parent == NULL)
      path = malloc(strlen(dir)+1);
   else
      path = malloc(strlen(parent->path) + 1 + strlen(dir) + 1);

   if(path == NULL) {
      free(new);
      assert(parent == NULL || CheckerDT_Node_isValid(parent));
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
      assert(parent == NULL || CheckerDT_Node_isValid(parent));
      return NULL;
   }

    if (parent != NULL) {
        result = Node_linkChild(parent, new);
        if(result != SUCCESS)
            (void) Node_destroy(new);
        else
            assert(CheckerDT_Node_isValid(new));

        assert(CheckerDT_Node_isValid(parent));
    }


   assert(parent == NULL || CheckerDT_Node_isValid(parent));
   assert(CheckerDT_Node_isValid(new));
   return new;
}

/* see node.h for specification */
size_t Node_destroy(Node_T n) {
   size_t i;
   size_t count = 0;
   Node_T c;

   assert(n != NULL);
   
   if (n->parent != NULL) {
       Node_unlinkChild(n->parent, n);
   }

    if(!file) {
        for(i = 0; i < DynArray_getLength(n->fchildren); i++)
        {
            c = DynArray_get(n->fchildren, i);
            count += Node_destroy(c);
        }
        DynArray_free(n->fchildren);

        for(i = 0; i < DynArray_getLength(n->dchildren); i++)
        {
            c = DynArray_get(n->dchildren, i);
            count += Node_destroy(c);
        }
        DynArray_free(n->dchildren);
    }

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
size_t Node_getNumChildren(Node_T n) {
    DynArray_T children;
   assert(n != NULL);

    if(file)
        children = n->fchildren;
    else
        children = n->dchildren;
   return DynArray_getLength(children);
}

/* see node.h for specification */
int Node_hasChild(Node_T n, const char* path, size_t* childID) {
   size_t index;
   int result;
   Node_T checker;
   DynArray_T children;

   assert(n != NULL);
   assert(path != NULL);

   checker = Node_create(path, NULL);
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
Node_T Node_getChild(Node_T n, size_t childID) {
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

/* see node.h for specification */
Node_T Node_getParent(Node_T n) {
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
static int Node_linkChild(Node_T parent, Node_T child) {
   size_t i;
   char* rest;
    DynArray_T children;

   assert(parent != NULL);
   assert(child != NULL);
   assert(CheckerDT_Node_isValid(parent));
   assert(CheckerDT_Node_isValid(child));

   if(Node_hasChild(parent, child->path, NULL)) {
      assert(CheckerDT_Node_isValid(parent));
      assert(CheckerDT_Node_isValid(child));
      return ALREADY_IN_TREE;
   }
   i = strlen(parent->path);
   if(strncmp(child->path, parent->path, i)) {
      assert(CheckerDT_Node_isValid(parent));
      assert(CheckerDT_Node_isValid(child));
      return PARENT_CHILD_ERROR;
   }
   rest = child->path + i;
   if(strlen(child->path) >= i && rest[0] != '/') {
      assert(CheckerDT_Node_isValid(parent));
      assert(CheckerDT_Node_isValid(child));
      return PARENT_CHILD_ERROR;
   }
   rest++;
   if(strstr(rest, "/") != NULL) {
      assert(CheckerDT_Node_isValid(parent));
      assert(CheckerDT_Node_isValid(child));
      return PARENT_CHILD_ERROR;
   }
   child->parent = parent;

   if(file)
        children = parent->fchildren;
    else
        children = parent->dchildren;

   if(DynArray_bsearch(children, child, &i,
         (int (*)(const void*, const void*)) Node_compare) == 1) {
      assert(CheckerDT_Node_isValid(parent));
      assert(CheckerDT_Node_isValid(child));
      return ALREADY_IN_TREE;
   }

    if(DynArray_bsearch(children, child, &i,
         (int (*)(const void*, const void*)) Node_compare) == 1) {
      assert(CheckerDT_Node_isValid(parent));
      assert(CheckerDT_Node_isValid(child));
      return ALREADY_IN_TREE;
   }

   if(DynArray_addAt(children, i, child) == TRUE) {
      assert(CheckerDT_Node_isValid(parent));
      assert(CheckerDT_Node_isValid(child));
      return SUCCESS;
   }
   else {
      assert(CheckerDT_Node_isValid(parent));
      assert(CheckerDT_Node_isValid(child));
      return PARENT_CHILD_ERROR;
   }
}

/*
  Unlinks node parent from its child node child, if it can be found in the parent's children. child is unchanged.
 */
static void Node_unlinkChild(Node_T parent, Node_T child) {
   size_t i;
   DynArray_T children;

   assert(parent != NULL);
   assert(child != NULL);
   assert(CheckerDT_Node_isValid(parent));
   assert(CheckerDT_Node_isValid(child));

   if(file)
        children = parent->fchildren;
    else
        children = parent->dchildren;

   if(DynArray_bsearch(children, child, &i,
         (int (*)(const void*, const void*)) Node_compare) != 0)
        (void) DynArray_removeAt(children, i);

   assert(CheckerDT_Node_isValid(parent));
   assert(CheckerDT_Node_isValid(child));
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
