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
   a prefix of the path
*/
static Node_T FT_traversePath(char* path, Node_T curr, boolean *isFile, boolean *foundFullPath) {
   Node_T found;
   File_T fileFound;
   size_t i;

   assert(path != NULL);

   if(curr == NULL)
      return NULL;

   if(!strcmp(path,Node_getPath(curr))) {
      *foundFullPath = TRUE;
      return curr;
   }

   if(!strncmp(path, Node_getPath(curr), strlen(Node_getPath(curr)))) {
      for(i = 0; i < Node_getNumChildren(curr, 0); i++) {
         found = FT_traversePath(path,
                                Node_getDirChild(curr, i), isFile, foundFullPath);
         if(found != NULL)
            return found;
      }
      for(i = 0; i < Node_getNumChildren(curr, 1); i++) {
         fileFound = Node_getFileChild(curr, i);

         if (fileFound != NULL) {
             if(!strcmp(path,File_getPath(fileFound))) {
              *foundFullPath = TRUE;
             }
            *isFile = TRUE;
            return NULL;
         }
      }
      return curr;
   }
   return NULL;
}


/*
   Given a prospective parent and child node,
   adds child to parent's children list, if possible

   If not possible, destroys the hierarchy rooted at child
   and returns PARENT_CHILD_ERROR, otherwise, returns SUCCESS.
*/

static int FT_linkParentToChild(Node_T parent, Node_T child) {

   assert(parent != NULL);

   if(Node_linkChild(parent, child) != SUCCESS) {
      (void) Node_destroy(child);
      return PARENT_CHILD_ERROR;
   }

   return SUCCESS;
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

   if(curr == NULL) {
      if(root != NULL) {
         return CONFLICTING_PATH;
      }
   }
   else {
      if(!strcmp(path, Node_getPath(curr)))
         return ALREADY_IN_TREE;

      restPath += (strlen(Node_getPath(curr)) + 1);
   }

   copyPath = malloc(strlen(restPath)+1);
   if(copyPath == NULL)
      return MEMORY_ERROR;
   strcpy(copyPath, restPath);
   dirToken = strtok(copyPath, "/");

   while(dirToken != NULL) {
      new = Node_create(dirToken, curr);

      if(new == NULL) {
         if(firstNew != NULL)
            (void) Node_destroy(firstNew);
         free(copyPath);
         return MEMORY_ERROR;
      }

      newCount++;

    if(firstNew == NULL)
         firstNew = new;
    else {
         result = FT_linkParentToChild(curr, new);
         if(result != SUCCESS) {
            (void) Node_destroy(new);
            (void) Node_destroy(firstNew);
            free(copyPath);
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

   result = FT_linkParentToChild(parent, firstNew);

   return result;
}



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

boolean FT_containsDir(char *path)  
{
   Node_T curr;
   boolean result;
   boolean foundFullPath = FALSE;

   assert(path != NULL);

   if(!isInitialized)
      return FALSE;

   curr = FT_traversePath(path, root, &result, &foundFullPath);


   if(curr == NULL) {
      return FALSE;
   }
   if(!strcmp(path, Node_getPath(curr)))
      return TRUE;
   
   return FALSE;
}




int FT_rmDir(char *path)
{
    Node_T curr, parent;
    boolean isFile = FALSE;
    boolean foundFullPath = FALSE;

    assert(path != NULL);
   

    if(!isInitialized)
      return INITIALIZATION_ERROR;

    curr = FT_traversePath(path, root, &isFile, &foundFullPath);

    if(isFile) {
          return NOT_A_DIRECTORY;
       }

    if(curr == NULL) {
       return  NO_SUCH_PATH;
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

    lastOccurance = strrchr(path, '/');

    if (lastOccurance == NULL) {
       return CONFLICTING_PATH;
    }

    parentPath = malloc((lastOccurance - path + 1));

    if (parentPath == NULL) {
       return MEMORY_ERROR;
    }

    strncpy(parentPath, path, lastOccurance - path);
    strcat(parentPath, "");
   
    current = FT_traversePath(parentPath, root, &isFile, &foundFullPath);

    if(isFile) 
       return NOT_A_DIRECTORY;
    

    if (current == NULL) {
        if (root != NULL) {
          return CONFLICTING_PATH;
       }
         result = FT_insertRestOfPath(parentPath, current);
        if (result != SUCCESS) {
          return result;
       }
        current = FT_traversePath(parentPath, current, &isFile, &foundFullPath);
    }

    else {
       if (strcmp(parentPath, Node_getPath(current))) {
          result = FT_insertRestOfPath(parentPath, current);

          if (result != SUCCESS) {
             return result;
          }
          current = FT_traversePath(parentPath, current, &isFile, &foundFullPath);
      }
    }

    exists = (Node_hasFileChild(current, path, NULL) || Node_hasDirChild(current, path, NULL));

    if (exists) {
       return ALREADY_IN_TREE;
    }

    file = File_create(++lastOccurance, current, contents, length);

    if(file == NULL) {
       return MEMORY_ERROR;
    }

    result = File_linkChild(current, file);

    return result;
}

boolean FT_containsFile(char *path)
{
    Node_T parent;
    boolean result;
    boolean isFile = FALSE;
    char *lastOccurance;
    char *parentPath;
    boolean foundFullPath = FALSE;

    assert(path != NULL);

    if(!isInitialized)
      return FALSE;

    lastOccurance = strrchr(path, '/');

    if (lastOccurance == NULL) {
       return FALSE;
    }

    parentPath = malloc((lastOccurance - path + 1));

    if (parentPath == NULL) {
       return FALSE;
    }

    strncpy(parentPath, path, lastOccurance - path);
    strcat(parentPath, "");
   
    parent = FT_traversePath(parentPath, root, &isFile, &foundFullPath);

    if (parent == NULL) {
        return FALSE;
    }

    else {
       if (!strcmp(parentPath, Node_getPath(parent))) {
          result = Node_hasFileChild(parent, path, NULL);
          if (result) {
             return TRUE;
          }
      }
    }

      return FALSE;
}

int FT_rmFile(char *path)
{
    File_T curr;
    Node_T parent;
    int result;
    boolean isFile = FALSE;
    char *lastOccurance;
    char *parentPath;
    size_t childID = 0;
    boolean foundFullPath = FALSE;

    assert(path != NULL);
   

    if(!isInitialized)
      return INITIALIZATION_ERROR;

     lastOccurance = strrchr(path, '/');

    if (lastOccurance == NULL) {
       return NO_SUCH_PATH;
    }

    parentPath = malloc((lastOccurance - path + 1));

    if (parentPath == NULL) {
       return MEMORY_ERROR;
    }

    strncpy(parentPath, path, lastOccurance - path);
    strcat(parentPath, "");
   
    parent = FT_traversePath(parentPath, root, &isFile, &foundFullPath);

    if (parent == NULL) {
        return NO_SUCH_PATH;
    }

    else {
       if (!strcmp(parentPath, Node_getPath(parent))) {
          result = Node_hasFileChild(parent, path, &childID);
          if (result) {

             curr = Node_getFileChild(parent, childID);
             File_unlinkChild(parent, curr);
             File_destroy(curr);
             return SUCCESS;
          }
          if (result == -1) {
             return MEMORY_ERROR;
          }
          if(Node_hasDirChild(parent, path, NULL)) {
             return NOT_A_FILE;
          }
      }
    }

    return NO_SUCH_PATH;
}


void *FT_getFileContents(char *path)
{
    File_T curr;
    Node_T parent;
    int result;
    boolean isFile = FALSE;
    char *lastOccurance;
    char *parentPath;
    size_t childID = 0;
    boolean foundFullPath = FALSE;

    assert(path != NULL);
   

    if(!isInitialized)
      return NULL;

     lastOccurance = strrchr(path, '/');

    if (lastOccurance == NULL) {
       return NULL;
    }

    parentPath = malloc((lastOccurance - path + 1));

    if (parentPath == NULL) {
       return NULL;
    }

    strncpy(parentPath, path, lastOccurance - path);
    strcat(parentPath, "");
   
    parent = FT_traversePath(parentPath, root, &isFile, &foundFullPath);

    if (parent == NULL) {
        return NULL;
    }

    else {
       if (!strcmp(parentPath, Node_getPath(parent))) {
          result = Node_hasFileChild(parent, path, &childID);
          if (result) {
             curr = Node_getFileChild(parent, childID);
             return File_getContents(curr);
          }
      }
    }

    
    return NULL;
}

void *FT_replaceFileContents(char *path, void *newContents,
                             size_t newLength)
{
  File_T curr;
    Node_T parent;
    int result;
    boolean isFile = FALSE;
    char *lastOccurance;
    char *parentPath;
    size_t childID = 0;
    boolean foundFullPath = FALSE;

    assert(path != NULL);
   

    if(!isInitialized)
      return NULL;

     lastOccurance = strrchr(path, '/');

    if (lastOccurance == NULL) {
       return NULL;
    }

    parentPath = malloc((lastOccurance - path + 1));

    if (parentPath == NULL) {
       return NULL;
    }

    strncpy(parentPath, path, lastOccurance - path);
    strcat(parentPath, "");
   
    parent = FT_traversePath(parentPath, root, &isFile, &foundFullPath);

    if (parent == NULL) {
        return NULL;
    }

    else {
       if (!strcmp(parentPath, Node_getPath(parent))) {
          result = Node_hasFileChild(parent, path, &childID);
          if (result) {
             curr = Node_getFileChild(parent, childID);
             return File_replaceContents(curr, newContents, newLength);
          }
      }
    }

    
    return NULL;
}

int FT_stat(char *path, boolean *type, size_t *length)
{
    Node_T parent;
    File_T curr;
    int result;
    boolean isFile = FALSE;
    char *lastOccurance;
    char *parentPath;
    size_t childID = 0;
    boolean foundFullPath = FALSE;

    assert(path != NULL);

    if(!isInitialized)
      return INITIALIZATION_ERROR;

    lastOccurance = strrchr(path, '/');

    if (lastOccurance == NULL) {
       if(!strcmp(path, Node_getPath(root))) {
          *type = FALSE;
          return SUCCESS;
       }
       return NO_SUCH_PATH;
    }

    parentPath = malloc((lastOccurance - path + 1));

    if (parentPath == NULL) {
       return MEMORY_ERROR;
    }

    strncpy(parentPath, path, lastOccurance - path);
    strcat(parentPath, "");
   
    parent = FT_traversePath(parentPath, root, &isFile, &foundFullPath);

    if (parent == NULL) {
        return NO_SUCH_PATH;
    }

       if (!strcmp(parentPath, Node_getPath(parent))) {
          result = Node_hasFileChild(parent, path, &childID);
          if (result) {
             curr = Node_getFileChild(parent, childID);
             *type = TRUE;
             *length = File_getContentLength(curr);
             return SUCCESS;
          }
          if (result == -1)
            return MEMORY_ERROR;
          result = Node_hasDirChild(parent, path, NULL);
          if (result) {
             *type = FALSE;
             return SUCCESS;
          }
          if (result == -1)
            return MEMORY_ERROR;
      }

      return NO_SUCH_PATH;
}

int FT_init(void)
{
   if(isInitialized)
      return INITIALIZATION_ERROR;
   isInitialized = 1;
   root = NULL;
   count = 0;
   return SUCCESS;
}

int FT_destroy(void)
{
   if(!isInitialized)
      return INITIALIZATION_ERROR;

   if (root != NULL) {
       Node_destroy(root);
       count = 0;
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
static void FT_preOrderTraversal(Node_T n, DynArray_T d) {
   size_t c;

   assert(d != NULL);

   if(n != NULL) {
      (void) DynArray_add(d, Node_getPath(n));
      for(c = 0; c < Node_getNumChildren(n, TRUE); c++) {
         DynArray_add(d, File_getPath(Node_getFileChild(n, c)));
      }
      for(c = 0; c < Node_getNumChildren(n, FALSE); c++) {
         FT_preOrderTraversal(Node_getDirChild(n, c), d);
      }
   }
}

/*
   Alternate version of strlen that uses pAcc as an in-out parameter
   to accumulate a string length, rather than returning the length of
   str, and also always adds one more in addition to str's length.
*/
static void FT_strlenAccumulate(char* str, size_t* pAcc) {
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
   assert(acc != NULL);

   if(str != NULL)
      strcat(acc, str); strcat(acc, "\n");
}

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
   (void) FT_preOrderTraversal(root, nodes);

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