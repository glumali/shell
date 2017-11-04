/*--------------------------------------------------------------------*/
/* command.c                                                          */
/* Author: Greg Umali                                                 */
/*--------------------------------------------------------------------*/

#include "command.h"
#include "dynarray.h"
#include "token.h"
#include "ish.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*--------------------------------------------------------------------*/

/* A Command includes information on the command name, its arguments, 
   a stdin redirect location, and a stdout redirect location. */

struct Command
{
   /* The string which is the command's name. */
   char *pcName;

   /* DynArray of strings that contains the command's arguments. */
   DynArray_T oArgs;

   /* The string that is the location name of the command's input
      location. By default, this is the empty string, for stdin. */
   char *pcInFile;

   /* The string that is the location name of the command's output
      location. By default, this is the empty string, for stout. */
   char *pcOutFile;
};

/*--------------------------------------------------------------------*/

/* Create and return a command whose name is pcName with arguments
   list oTokens, stdin redirected to fd iInFile, and stdout
   redirected to fd iOutFile.  The caller owns the command. */

struct Command *newCommand(char *pcName, DynArray_T oArgs, 
                           char *pcInFile, char *pcOutFile)
{
   /* Holds the finished command object */
   struct Command *psCommand;

   /* Indexing variables to construct argument array */ 
   size_t u;
   size_t uLen;

   /* Copies args passed as an argument into oArgs of psCommand */
   char *pcOldArg;
   char *pcNewArg;

   assert(pcName != NULL);
   assert(oArgs != NULL);

   uLen = DynArray_getLength(oArgs);

   /* Malloc needed space for new Command */
   psCommand = (struct Command*)malloc(sizeof(struct Command));
   if (psCommand == NULL)
   {perror(getPgmName()); exit(EXIT_FAILURE);}

   /* Create new char pointer to the name of the command */
   psCommand->pcName = (char*)malloc(strlen(pcName) + 1);
   if (psCommand->pcName == NULL)
   {perror(getPgmName()); exit(EXIT_FAILURE);}
   strcpy(psCommand->pcName, pcName);

   /* Stores arguments of given arg array into oArgs of the Command */
   psCommand->oArgs = DynArray_new(0);
    
   /* Copies each element from oArgs into a string that is
      placed into the oArgs array of the new Command object. */
   for (u = 0; u < uLen; u++) {
      pcOldArg = DynArray_get(oArgs, u);

      /* Malloc enough memory for the new string */
      pcNewArg = (char*)malloc(strlen(pcOldArg) + 1);
      if (pcNewArg == NULL)
      {perror(getPgmName()); exit(EXIT_FAILURE);}

      /* Copy argument into the oArgs array of the Command */
      strcpy(pcNewArg, pcOldArg);
      DynArray_add(psCommand->oArgs, pcNewArg);
   }

   /* Create new char pointer to the redirected stdin location */
   if(pcInFile != NULL) {
      psCommand->pcInFile = (char*)malloc(strlen(pcInFile) + 1);
      if (psCommand->pcInFile == NULL)
      {perror(getPgmName()); exit(EXIT_FAILURE);}
      /* Store the stdin redirect location into the Command */
      strcpy(psCommand->pcInFile, pcInFile);
   }
   else psCommand->pcInFile = NULL;

   /* Create new char pointer to the redirected stdout location */
   if(pcOutFile != NULL) {
      psCommand->pcOutFile = (char*)malloc(strlen(pcOutFile) + 1);
      if (psCommand->pcOutFile == NULL)
      {perror(getPgmName()); exit(EXIT_FAILURE);}
      /* Store the stdout redirect location into the Command */
      strcpy(psCommand->pcOutFile, pcOutFile);
   }
   else psCommand->pcOutFile = NULL;

   return psCommand;
}

/*--------------------------------------------------------------------*/

/* Returns the name of the Command object oCommand as a string. */
char* Command_getName(Command_T oCommand)
{
   assert(oCommand != NULL);
   return oCommand->pcName;
}

/*--------------------------------------------------------------------*/

/* Returns the argument DynArray of the Command object oCommand. */
DynArray_T Command_getArgs(Command_T oCommand)
{
   assert(oCommand != NULL);
   return oCommand->oArgs;
}

/*--------------------------------------------------------------------*/

/* Returns the stdin location of oCommand as a string. */
char* Command_getStdin(Command_T oCommand)
{
   assert(oCommand != NULL);
   return oCommand->pcInFile;
}

/*--------------------------------------------------------------------*/

/* Returns the stdout location of oCommand as a string. */
char* Command_getStdout(Command_T oCommand)
{
   assert(oCommand != NULL);
   return oCommand->pcOutFile;
}

/*--------------------------------------------------------------------*/

/* Outputs the details of a Command object to stdout.  Writes the name
   of the command, followed by the command's arguments, stdin redirect
   location, and stdout redirect location, if applicable. */

void writeCommand(Command_T oCommand)
{
   size_t u;
   size_t uLength;
   char *pcArg;

   assert(oCommand != NULL);

   /* Print out command name */
   printf("Command name: %s\n", oCommand->pcName);

   /* Print out arguments */
   uLength = DynArray_getLength(oCommand->oArgs);

   for (u = 0; u < uLength; u++)
   {
      pcArg = DynArray_get(oCommand->oArgs, u);
      printf("Command arg: %s\n", pcArg);
   }

   /* Print out input location if applicable */
   if(oCommand->pcInFile != NULL)
      printf("Command stdin: %s\n", oCommand->pcInFile);

   /* Print out output location if applicable */
   if(oCommand->pcOutFile != NULL)
      printf("Command stdout: %s\n", oCommand->pcOutFile);
}

/*--------------------------------------------------------------------*/

/* Free all of the data associated data with a Command object. */

void freeCommand(Command_T oCommand)
{
   assert(oCommand != NULL);

   free(oCommand->pcName);

   freeTokens(oCommand->oArgs);
   free(oCommand->oArgs);

   if(oCommand->pcInFile != NULL) 
      free(oCommand->pcInFile);

   if(oCommand->pcOutFile != NULL) 
      free(oCommand->pcOutFile);

   free(oCommand);
}

/*--------------------------------------------------------------------*/

