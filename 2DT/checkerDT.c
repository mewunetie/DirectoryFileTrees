/*--------------------------------------------------------------------*/
/* checkerDT.c                                                        */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "dynarray.h"
#include "checkerDT.h"


/* see checkerDT.h for specification */
boolean CheckerDT_Node_isValid(Node_T n) {
   Node_T parent;
   const char* npath;
   const char* ppath;
   const char* rest;
   size_t i;

   size_t nChildren, c;
   const char *cpath, *cpath1, *cpath2;
   
   /* Sample check: a NULL pointer is not a valid node */
   if(n == NULL) {
      fprintf(stderr, "A node is a NULL pointer\n");
      return FALSE;
   }

   npath = Node_getPath(n);
   if (npath == NULL) {
      fprintf(stderr, "Node path is a NULL pointer\n");
      return FALSE;
   }
   
   parent = Node_getParent(n);
   if(parent != NULL) {
      /* Sample check that parent's path must be prefix of n's path */
      ppath = Node_getPath(parent);
      if (ppath == NULL) {
         fprintf(stderr, "Node path is a NULL pointer\n");
         return FALSE;
      }
      
      i = strlen(ppath);
      if(strncmp(npath, ppath, i)) {
         fprintf(stderr, "P's path is not a prefix of C's path\n");
         return FALSE;
      }
      
      /* Sample check that n's path after parent's path + '/'
         must have no further '/' characters */
      rest = npath + i;
      rest++;
      if(strstr(rest, "/") != NULL) {
         fprintf(stderr, "C's path has grandchild of P's path\n");
         return FALSE;
      }
   }

   nChildren = Node_getNumChildren(n);
   for (c = 0; c < nChildren; c++)
   {
      Node_T child = Node_getChild(n, c);

      if (child == NULL) {
         fprintf(stderr,
                 "No child at index in [0,Node_getNumChildren(n))\n");
         return FALSE;
      }

      cpath = Node_getPath(child);
      if (cpath == NULL) {
         fprintf(stderr, "Node path is a NULL pointer\n");
         return FALSE;
      }

      if (c == 0)
         cpath1 = cpath;
      else {
         int cmp;
         
         cpath2 = cpath;
         cmp = strcmp(cpath1, cpath2);
         if (cmp > 0) {
            fprintf(stderr, "Children are out of order\n");
            return FALSE;
         }         
         if (cmp == 0) {
            fprintf(stderr, "A node has duplicate children\n");
            return FALSE;
         }
         
         cpath1 = cpath2;
      }
   }
   
   return TRUE;
}

/*
   Performs a pre-order traversal of the tree rooted at n.
   Returns FALSE if a broken invariant is found and
   returns TRUE otherwise.

   You may want to change this function's return type or
   parameter list to facilitate constructing your checks.
   If you do, you should update this function comment.
*/
static size_t CheckerDT_treeCheck(Node_T n) {
   size_t c;
   size_t count = 0, subCount, nChildren;

   /* Sample check on each non-root node: node must be valid */
   /* If not, pass that failure back up immediately */
   if(!CheckerDT_Node_isValid(n))
      return 0;

   nChildren = Node_getNumChildren(n);
   for(c = 0; c < nChildren; c++)
   {
      Node_T child = Node_getChild(n, c);

      /* if recurring down one subtree results in a failed check
         farther down, passes the failure back up immediately */
      if((subCount = CheckerDT_treeCheck(child)) == 0)
         return 0;

      count += subCount;
   }
   
   return 1 + count;
}
   
/* see checkerDT.h for specification */
boolean CheckerDT_isValid(boolean isInit, Node_T root, size_t count) {
   size_t nNodes;
   const char* rpath;
   Node_T parent;
   /* Sample check on a top-level data structure invariant:
      if the DT is not initialized, its count should be 0. */
   if(!isInit)
   {
      if(count != 0) {
         fprintf(stderr, "Not initialized, but count is not 0\n");
         return FALSE;
      }

      if (root != NULL) {
         fprintf(stderr, "Not initialized, but root is not NULL\n");
         return FALSE;
      }

      return TRUE;
   }

   if (root == NULL) {
      if (count != 0) {
         fprintf(stderr,
                 "Count doesn't match number of nodes in tree\n");
         return FALSE;
      }
      return TRUE;
   }

   /* Checks if root fields are valid */
   rpath = Node_getPath(root);

   if (rpath == NULL) {
      fprintf(stderr, "Node path is a NULL pointer\n");
      return FALSE;
   }

   if (strstr(rpath, "/") != NULL) {
      fprintf(stderr, "Root path contains the forward slash\n");
      return FALSE;
   }
   
   parent = Node_getParent(root);

   if (parent != NULL) {
      fprintf(stderr, "Parent of root is not NULL\n");
      return FALSE;
   }
   
   /* Now checks invariants recursively at each node from the root. */
   if ((nNodes = CheckerDT_treeCheck(root)) == 0) {
      return FALSE;
   }
   
   if (nNodes != count) {
      fprintf(stderr, "Count doesn't match number of nodes in tree\n");
      return FALSE;
   }
   
   return TRUE;
}
