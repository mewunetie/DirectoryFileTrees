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
#include "FT_directory.h"
#include "FT_file.h"
#include "checkerDT.h"

/* A Directory Tree is an AO with 3 state variables: */
/* a flag for if it is in an initialized state (TRUE) or not (FALSE) */
static boolean isInitializedDir;
/* a pointer to the root node in the hierarchy */
static Node_T rootDir;
/* a counter of the number of nodes in the hierarchy */
static size_t countDir;

/* A Directory Tree is an AO with 3 state variables: */
/* a flag for if it is in an initialized state (TRUE) or not (FALSE) */
static boolean isInitializedFile;
/* a pointer to the root node in the hierarchy */
static File_T rootFile;
/* a counter of the number of nodes in the hierarchy */
static size_t countFile;




/*
   Starting at the parameter curr, traverses as far down
   the hierarchy as possible while still matching the path
   parameter.

   Returns a pointer to the farthest matching node down that path,
   or NULL if there is no node in curr's hierarchy that matches
   a prefix of the path
*/
static Node_T FT_traversePath(char* path, Node_T curr) {
   Node_T found;
   size_t i;

   assert(path != NULL);

   if(curr == NULL)
      return NULL;

   else if(!strcmp(path,Node_getPath(curr)))
      return curr;

   else if(!strncmp(path, Node_getPath(curr), strlen(Node_getPath(curr)))) {
      for(i = 0; i < Node_getNumChildren(curr); i++) {
         found = FT_traversePath(path,
                                Node_getChild(curr, i));
         if(found != NULL)
            return found;
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

      curr = new;
      dirToken = strtok(NULL, "/");
   }

   free(copyPath);

    count += newCount;

   if(parent == NULL) {
      root = firstNew;
      return SUCCESS;
   }

      return result;
}



int FT_insertDir(char *path)
{
   Node_T curr;
   int result;

   assert(CheckerDT_isValid(isInitialized,rootDir,count));
   assert(path != NULL);

   if(!isInitialized)
      return INITIALIZATION_ERROR;
   curr = FT_traversePath(path, root);
   result = FT_insertRestOfPath(path, curr);
   assert(CheckerDT_isValid(isInitialized,rootDir,count));
   return result;
}

boolean FT_containsDir(char *path)  
{
    Node_T curr;
   boolean result;

   assert(CheckerDT_isValid(isInitialized,rootDir,count));
   assert(path != NULL);

   if(!isInitialized)
      return FALSE;

   curr = FT_traversePath(path, root);


   if(curr == NULL)
      result = FALSE;
   else if(strcmp(path, Node_getPath(curr)))
      result = FALSE;
   else
      result = TRUE;

   assert(CheckerDT_isValid(isInitialized,rootDir,count));
   return result;
}




int FT_rmDir(char *path)
{
    Node_T curr, parent;
   int result;

   assert(CheckerDT_isValid(isInitialized,rootDir,count));
   assert(path != NULL);
   

   if(!isInitialized)
      return INITIALIZATION_ERROR;

   curr = FT_traversePath(path, root);
   if(curr == NULL)
      result =  NO_SUCH_PATH;
   else {
         parent = Node_getParent(curr);

   if(!strcmp(path,Node_getPath(curr))) 
   {
      if(parent == NULL)
         root = NULL;

      count -= Node_destroy(curr);

      result = SUCCESS;
   }

   else
      result = NO_SUCH_PATH;
   }

   assert(CheckerDT_isValid(isInitialized,rootDir,count));
   return result;
}

static Node_T FT_traversePath(char* path, File_T curr) {
   File_T found;
   size_t i;

   assert(path != NULL);

   if(curr == NULL)
      return NULL;

   else if(!strcmp(path, File_getPath(curr)))
      return curr;

   return NULL;
}


static int FT_insertPathHelper(char* path, File_T parent) {

   File_T curr = parent;
   File_T firstNew = NULL;
   File_T new;
   char* copyPath;
   char* dirToken;
   int result;
   size_t newCount = 0;

   assert(path != NULL);

   if(curr == NULL) {
      if(root != NULL) {
         return CONFLICTING_PATH;
      }
   }
   else if(!strcmp(path, File_getPath(curr)))
         return ALREADY_IN_TREE;


   copyPath = malloc(strlen(path)+1);
   
   if(copyPath == NULL)
      return MEMORY_ERROR;

   strcpy(copyPath, path);

   dirToken = strtok(copyPath, "/");

   while(dirToken != NULL) {
      new = File_create(dirToken, curr);

      if(new == NULL) {
         if(firstNew != NULL)
            (void) File_destroy(firstNew);
         free(copyPath);
         return MEMORY_ERROR;
      }

      newCount++;

      if(firstNew == NULL)
         firstNew = new;

      curr = new;
      dirToken = strtok(NULL, "/");
   }

   free(copyPath);

    count += newCount;

   if(parent == NULL) {
      root = firstNew;
      return SUCCESS;
   }

      return result;
}




int FT_insertFile(char *path, void *contents, size_t length)
{
    File_T curr;
   int result;

   assert(CheckerDT_isValid(isInitialized,rootDir,count));
   assert(path != NULL);

   if(!isInitialized)
      return INITIALIZATION_ERROR;
      
   curr = FT_traversePath(path, root);
   result = FT_insertPathHelper(path, curr);

   assert(CheckerDT_isValid(isInitialized,rootDir,count));
   return result;

}
boolean FT_containsFile(char *path)
{

}
int FT_rmFile(char *path)
{
    
}
void *FT_getFileContents(char *path)
{
    
}
void *FT_replaceFileContents(char *path, void *newContents,
                             size_t newLength)
{

}

int FT_stat(char *path, boolean *type, size_t *length)
{

}
int FT_init(void)
{

}
int FT_destroy(void)
{

}
char *FT_toString(void)
{

}