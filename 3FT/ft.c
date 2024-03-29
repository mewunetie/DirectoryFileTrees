/*--------------------------------------------------------------------*/
/* ft.c                                                               */
/* Authors: Misrach Ewunetie, Shruti Roy                              */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "dynarray.h"
#include "ft.h"
#include "node.h"
#include "file.h"

/* A Directory Tree is an AO with 3 state variables: */
/* a flag for if it is in an initialized state (TRUE) or not (FALSE) */
static boolean isInitialized;
/* a pointer to the root node in the hierarchy */
static Node_T root;
/* a counter of the number of nodes in the hierarchy */
static size_t count;

/*
   Starting at the parameter curr, traverses as far down
   the hierarchy as possible while still matching the path
   parameter.

   Returns a pointer to the farthest matching node down that path,
   or NULL if there is no node in curr's hierarchy that matches
   a prefix of the path.

   If the full path is found, sets foundFullPath to true.
   Else, foundFullPath will be false.

   If a file is found with a prefix of the path or the path itself,
   isFile is set to true. Otherwise, it will be false.
*/
static Node_T FT_traversePath(char* path, Node_T curr,
                              boolean *isFile, boolean *foundFullPath) {
   Node_T found;
   File_T fileFound;
   size_t i;

   assert(path != NULL);
   assert(isFile != NULL);
   assert(foundFullPath != NULL);

   *foundFullPath = FALSE;
   *isFile = FALSE;

   /* if the current node is NULL, cannot traverse further */
   if(curr == NULL)
      return NULL;

   /* if the current path matches the target, we have traversed the
      full path */
   if(!strcmp(path,Node_getPath(curr))) {
      *foundFullPath = TRUE;
      return curr;
   }

   /* if the current path is a prefix of the target, continue traversing
      through its children */
   if(!strncmp(path, Node_getPath(curr), strlen(Node_getPath(curr)))) {
      /* iterate through directory children and traverse. return if
         the end of any matching path has been found. */
      for(i = 0; i < Node_getNumChildren(curr, 0); i++) {
         found = FT_traversePath(path, Node_getDirChild(curr, i),
                                 isFile, foundFullPath);
         if(found != NULL)
            return found;
      }

      /* iterate through file children and check if paths match target
         or a prefix of target. Indicate that a file has been found
         and whether it matches the full path. Return the current
         directory (the parent of the target file). */
      for(i = 0; i < Node_getNumChildren(curr, 1); i++) {
         fileFound = Node_getFileChild(curr, i);

         if (fileFound != NULL) {
             if(!strncmp(path, File_getPath(fileFound),
                              strlen(File_getPath(fileFound)))) {
                *isFile = TRUE;
                if(!strcmp(path,File_getPath(fileFound)))
                   *foundFullPath = TRUE;
                
                return curr;
             }
         }
      }
      return curr;
   }
   return NULL;
}

/*
   Inserts a new path into the tree rooted at parent, or, if
   parent is NULL, as the root of the data structure.

   If a node representing path already exists, returns ALREADY_IN_TREE

   If there is an allocation error in creating any of the new nodes or
   their fields, returns MEMORY_ERROR

   If there is an error linking any of the new nodes,
   returns PARENT_CHILD_ERROR

   Otherwise, returns SUCCESS
*/
static int FT_insertRestOfPath(char* path, Node_T parent) {

   Node_T curr = parent;
   Node_T firstNew = NULL;
   Node_T new;
   char* copyPath;
   char* restPath = path;
   char* dirToken;
   int result;
   size_t newCount = 0;

   assert(path != NULL);

   /* if root is not NULL, curr must be a descendent of root */
   if(curr == NULL) {
      if(root != NULL) {
         return CONFLICTING_PATH;
      }
   }
   /* if paths match, the target already exists */
   else {
      if(!strcmp(path, Node_getPath(curr)))
         return ALREADY_IN_TREE;

      restPath += (strlen(Node_getPath(curr)) + 1);
   }

   /* create a copy of the path to tokenize */
   copyPath = malloc(strlen(restPath)+1);
   if(copyPath == NULL)
      return MEMORY_ERROR;
   strcpy(copyPath, restPath);
   dirToken = strtok(copyPath, "/");

   /* iterate through, create, and link subsequent directories from 
      current until the full path is reached. If an error occurs, 
      revert back to the initial file tree */
   while(dirToken != NULL) {
      new = Node_create(dirToken, curr);

      if(new == NULL) {
         if(firstNew != NULL)
            (void) Node_destroy(firstNew);
         free(copyPath);
         return MEMORY_ERROR;
      }

      newCount++;

      /* link new nodes to each other, but not to parent in case
         of any error */
      if(firstNew == NULL)
         firstNew = new;
      else {
         if((result = Node_linkChild(curr, new)) != SUCCESS) {
            (void) Node_destroy(new);
            (void) Node_destroy(firstNew);
            return result;
         }
      }


      curr = new;
      dirToken = strtok(NULL, "/");
   }

   free(copyPath);

   count += newCount;

   if(parent == NULL) {
      root = firstNew;
      return SUCCESS;
   }

   /* connect the extended path to the parent node */
   if((result = Node_linkChild(parent, firstNew)) != SUCCESS)
      (void) Node_destroy(firstNew);


   return result;
}

/* see ft.h for specification */
int FT_insertDir(char *path)
{
   Node_T curr;
   int result;
   boolean isFile = FALSE;
   boolean foundFullPath = FALSE;

   assert(path != NULL);

   if(!isInitialized)
      return INITIALIZATION_ERROR;
   curr = FT_traversePath(path, root, &isFile, &foundFullPath);
   if(foundFullPath) {
            return ALREADY_IN_TREE;
   }

   if (isFile) {  
         return NOT_A_DIRECTORY;
   }

   if (curr == NULL) {
      if (root != NULL) {
         return CONFLICTING_PATH;
      }
   }

   result = FT_insertRestOfPath(path, curr);

   return result;
}

/* see ft.h for specification */
boolean FT_containsDir(char *path)  
{
   Node_T curr;
   boolean isFile = FALSE;
   boolean foundFullPath = FALSE;

   assert(path != NULL);

   if(!isInitialized)
      return FALSE;

   curr = FT_traversePath(path, root, &isFile, &foundFullPath);


   if(curr == NULL || isFile) {
      return FALSE;
   }

   if(!foundFullPath)
      return FALSE;
   
   return TRUE;
}

/* see ft.h for specification */
int FT_rmDir(char *path)
{
    Node_T curr, parent;
    boolean isFile = FALSE;
    boolean foundFullPath = FALSE;

    assert(path != NULL);
   

    if(!isInitialized)
      return INITIALIZATION_ERROR;

    curr = FT_traversePath(path, root, &isFile, &foundFullPath);

    if(isFile && foundFullPath) {
          return NOT_A_DIRECTORY;
    }

    if(curr == NULL || isFile) {
       return NO_SUCH_PATH;
    }
      

    if(!strcmp(path,Node_getPath(curr))) {
      parent = Node_getParent(curr);
      if(parent == NULL)
         root = NULL;
      else {
         Node_unlinkChild(parent, curr);
      }

      count -= Node_destroy(curr);
      return SUCCESS;
   }

   return NO_SUCH_PATH;
}

/* see ft.h for specification */
int FT_insertFile(char *path, void *contents, size_t length)
{
    File_T file;
    Node_T current;
    char *lastOccurance;
    char *parentPath;
    int result;
    int exists;
    boolean isFile = FALSE;
    boolean foundFullPath = FALSE;

    assert(path != NULL);
    
    if(!isInitialized)
      return INITIALIZATION_ERROR;

    /* create a truncated copy of path that represents the parent */
    lastOccurance = strrchr(path, '/');

    if (lastOccurance == NULL) {
       return CONFLICTING_PATH;
    }

    parentPath = calloc((size_t)(lastOccurance - path + 1), 1);

    if (parentPath == NULL) {
       return MEMORY_ERROR;
    }

    strncpy(parentPath, path, (size_t)(lastOccurance - path));

    /* search for the parent directory of the target file */
    current = FT_traversePath(parentPath, root,
                              &isFile, &foundFullPath);

    /* the path terminates at a prefix file */
    if(isFile) {
       free(parentPath);
       return NOT_A_DIRECTORY;
    }

    /* if the full parent path doesn't exist, insert it and continue
       traversing down to the parent directory once created */
    if(!foundFullPath) {
       result = FT_insertRestOfPath(parentPath, current);
       if (result != SUCCESS) {
          free(parentPath);
          return result;
       }
       current = FT_traversePath(parentPath, current, &isFile,
                                 &foundFullPath);
    }

    free(parentPath);

    /* check if the parent directory already has a child with path */
    exists = (Node_hasFileChild(current, path, NULL) ||
              Node_hasDirChild(current, path, NULL));

    if (exists) {
       return ALREADY_IN_TREE;
    }

    file = File_create(++lastOccurance, current, contents, length);

    if(file == NULL) {
       return MEMORY_ERROR;
    }

    result = File_linkChild(current, file);

    if (result == SUCCESS)
       count++;
    return result;
}

/* see ft.h for specification */
boolean FT_containsFile(char *path)
{
    Node_T parent;
    boolean result;
    boolean isFile = FALSE;
    boolean foundFullPath = FALSE;

    assert(path != NULL);

    if(!isInitialized)
      return FALSE;
  
    parent = FT_traversePath(path, root, &isFile, &foundFullPath);

    if (parent == NULL || !foundFullPath || !isFile) {
      return FALSE;
    }

    result = Node_hasFileChild(parent, path, NULL);
    if (result) {
       return TRUE;
    }

    return FALSE;
}

/* see ft.h for specification */
int FT_rmFile(char *path)
{
    File_T curr;
    Node_T parent;
    int result;
    boolean isFile = FALSE;
    size_t childID = 0;
    boolean foundFullPath = FALSE;

    assert(path != NULL);
   

    if(!isInitialized)
      return INITIALIZATION_ERROR;
  
    parent = FT_traversePath(path, root, &isFile, &foundFullPath);

    if (parent == NULL || !foundFullPath) {
        return NO_SUCH_PATH;
    }
    if (!isFile)
       return NOT_A_FILE;

    result = Node_hasFileChild(parent, path, &childID);
    if (result) {
       curr = Node_getFileChild(parent, childID);
       File_unlinkChild(parent, curr);
       File_destroy(curr);

        count--;
        return SUCCESS;
     }
     if (result == -1) {
        return MEMORY_ERROR;
     }

    return NO_SUCH_PATH;
}

/* see ft.h for specification */
void *FT_getFileContents(char *path)
{
    File_T curr;
    Node_T parent;
    int result;
    boolean isFile = FALSE;
    size_t childID = 0;
    boolean foundFullPath = FALSE;

    assert(path != NULL);
   

    if(!isInitialized)
      return NULL;
  
    parent = FT_traversePath(path, root, &isFile, &foundFullPath);

    if (parent == NULL || !foundFullPath || !isFile) {
        return NULL;
    }

    result = Node_hasFileChild(parent, path, &childID);
    if (result) {
       curr = Node_getFileChild(parent, childID);
       return File_getContents(curr);
    }

    return NULL;
}

/* see ft.h for specification */
void *FT_replaceFileContents(char *path, void *newContents,
                             size_t newLength)
{
   File_T curr;
    Node_T parent;
    int result;
    boolean isFile = FALSE;
    size_t childID = 0;
    boolean foundFullPath = FALSE;

    assert(path != NULL);
   

    if(!isInitialized)
      return NULL;
   
    parent = FT_traversePath(path, root, &isFile, &foundFullPath);

    if (parent == NULL || !foundFullPath || !isFile) {
       return NULL;
    }

    result = Node_hasFileChild(parent, path, &childID);
    if (result) {
       curr = Node_getFileChild(parent, childID);
       return File_replaceContents(curr, newContents, newLength);
    }

    return NULL;
}

/* see ft.h for specification */
int FT_stat(char *path, boolean *type, size_t *length)
{
    Node_T curr;
    File_T file;
    int result;
    boolean isFile = FALSE;
    size_t childID = 0;
    boolean foundFullPath = FALSE;

    assert(path != NULL);
    assert(type != NULL);
    assert(length != NULL);
    
    if(!isInitialized)
      return INITIALIZATION_ERROR;

     
    curr = FT_traversePath(path, root, &isFile, &foundFullPath);

    if (!foundFullPath) {
        return NO_SUCH_PATH;
    }

    if (!strcmp(path, Node_getPath(curr))) {
       *type = FALSE;
       return SUCCESS;
    }
    else {
       result = Node_hasFileChild(curr, path, &childID);
       if (result) {
          file = Node_getFileChild(curr, childID);
          *type = TRUE;
          *length = File_getContentLength(file);
          return SUCCESS;
       }
       if (result == -1)
          return MEMORY_ERROR;
    }

    return NO_SUCH_PATH;
}

/* see ft.h for specification */
int FT_init(void)
{
   if(isInitialized)
      return INITIALIZATION_ERROR;
   isInitialized = 1;
   root = NULL;
   count = 0;
   return SUCCESS;
}

/* see ft.h for specification */
int FT_destroy(void)
{
   if(!isInitialized)
      return INITIALIZATION_ERROR;

   if (root != NULL) {
       count -= Node_destroy(root);
   }

   root = NULL;
   isInitialized = 0;
   return SUCCESS;

}

/*
   Performs a pre-order traversal of the tree rooted at n,
   inserting each payload to DynArray_T d beginning at index i.
   Returns the next unused index in d after the insertion(s).
*/
static size_t FT_preOrderTraversal(Node_T n, DynArray_T d, size_t i) {
   size_t c;

   assert(d != NULL);

   if(n != NULL) {
      (void) DynArray_set(d, i++, Node_getPath(n));
      for(c = 0; c < Node_getNumChildren(n, TRUE); c++) {
         (void) DynArray_set(d, i++,
                             File_getPath(Node_getFileChild(n, c)));
      }
      for(c = 0; c < Node_getNumChildren(n, FALSE); c++) {
         i = FT_preOrderTraversal(Node_getDirChild(n, c), d, i);
      }
   }

   return i;
}

/*
   Alternate version of strlen that uses pAcc as an in-out parameter
   to accumulate a string length, rather than returning the length of
   str, and also always adds one more in addition to str's length.
*/
static void FT_strlenAccumulate(char* str, size_t* pAcc) {
   assert(str != NULL);
   assert(pAcc != NULL);

   if(str != NULL)
      *pAcc += (strlen(str) + 1);
}


/*
   Alternate version of strcat that inverts the typical argument
   order, appending str onto acc, and also always adds a newline at
   the end of the concatenated string.
*/
static void FT_strcatAccumulate(char* str, char* acc) {
   assert(str != NULL);
   assert(acc != NULL);

   if(str != NULL)
      strcat(acc, str); strcat(acc, "\n");
}

/* see ft.h for specification */
char *FT_toString(void)
{
   DynArray_T nodes;
   size_t totalStrlen = 1;
   char* result = NULL;


   if(!isInitialized)
      return NULL;

   nodes = DynArray_new(count);
   if (nodes == NULL) {
      return NULL;
   }

   (void) FT_preOrderTraversal(root, nodes, 0);

   DynArray_map(nodes, (void (*)(void *, void*)) FT_strlenAccumulate,
                (void*) &totalStrlen);

   result = malloc(totalStrlen);
   if(result == NULL) {
      DynArray_free(nodes);
      return NULL;
   }
   *result = '\0';

   DynArray_map(nodes, (void (*)(void *, void*)) FT_strcatAccumulate,
                (void *) result);

   DynArray_free(nodes);
   return result;
}
