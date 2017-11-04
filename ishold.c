/*--------------------------------------------------------------------*/
/* ish.c                                                              */
/* Author: Greg Umali                                                 */
/*--------------------------------------------------------------------*/

#include "dynarray.h"
#include "ish.h"
#include "lexer.h"
#include "token.h"
#include "syner.h"
#include "command.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*--------------------------------------------------------------------*/

/* The name of the executable binary file. */
static const char *pcPgmName;

/*--------------------------------------------------------------------*/

/* Returns the name of the executable binary file. */

const char *getPgmName()
{
   return pcPgmName;
}

/*--------------------------------------------------------------------*/

/* Accepts a DynArray of tokens, and parses it to return a command.
   A command must begin with an ordinary token, must have at most one
   stdin redirect and stdout redirect each, and cannot follow a 
   redirect token with another special character or terminating the 
   program. Executes the command, and repeats until EOF. Returns 0 
   iff successful. As always, argc is the command-line argument count 
   and argv is an array of arguments. */

int main(int argc, char *argv[])
{
   char *pcLine;
   pid_t iPid;
   int iRet;

   DynArray_T oTokens;
   Command_T oCommand;

   pcPgmName = argv[0];

   printf("%% ");

   while ((pcLine = readLine(stdin)) != NULL)
   {
      printf("%s\n", pcLine);
      iRet = fflush(stdout);
      if (iRet == -1)
      {perror(pcPgmName); exit(EXIT_FAILURE);}
      oTokens = lexLine(pcLine);
      if (oTokens != NULL)
      {
         oCommand = synArr(oTokens);
         if (oCommand != NULL) {
            
            iRet = fflush(NULL);
            if (iRet == EOF) {perror(pcPgmName); exit(EXIT_FAILURE); }

            iPid = fork();
            if (iPid == -1) {perror(pcPgmName); exit(EXIT_FAILURE); }

            if (iPid == 0)
            {
               /* This code is executed by the child process only. */
               DynArray_T oArgs = Command_getArgs(oCommand);
               size_t u;
               size_t uLength = DynArray_getLength(oArgs);
               char* pcCommandName = Command_getName(oCommand);

               /* The working string argument */
               char *pcArgument;

               /* New string array to hold the arguments of the Command */
               char **pcArgs = (char **)malloc(sizeof(char*) * (uLength + 1) + 1);
               if (pcArgs == NULL) {perror(pcPgmName); exit(EXIT_FAILURE); }

               /* First element is the name of the command */
               *pcArgs = pcCommandName;

               for (u = 0; u < uLength; u++)
               {
                  pcArgument = DynArray_get(oArgs, u);
                  /* Set each element of pcArgs to be the corresponding
                     element of oArgs from oCommand */
                  *(pcArgs + u + 1) = pcArgument;
               }
               /* Set last element to the null character */
               *(pcArgs + u + 1) = '\0';

               execvp(pcCommandName, pcArgs);
               perror(pcPgmName);
               exit(EXIT_FAILURE);
            }

            /* This code is executed by the parent process only. */

            /* Wait for the child process to exit. */
            iPid = wait(NULL);
            if (iPid == -1) {perror(pcPgmName); exit(EXIT_FAILURE); }
         }
      }
      free(pcLine);
      printf("%% ");
   }
   printf("\n");
   return 0;
}
