/*--------------------------------------------------------------------*/
/* ishsyn.c                                                           */
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

/* Accepts a string from stdin, and parses it to return a command.
   A command must begin with an ordinary token, must have at most one
   stdin redirect and stdout redirect each, and cannot follow a 
   redirect token with another special character or terminating the 
   program. Writes the details of the parsed command to stdout, and 
   repeats until EOF. Returns 0 iff successful. As always, argc is the 
   command-line argument count and argv is an array of arguments. */

int main(int argc, char *argv[])
{
   /* Holds the line read in from standard input */
   char *pcLine;
   /* Holds the tokens parsed from the line */
   DynArray_T oTokens;
   /* Used to determine the success of functions */
   int iRet;
   /* Holds the finished command */
   Command_T oCommand;

   pcPgmName = argv[0];

   printf("%% ");

   while ((pcLine = readLine(stdin)) != NULL)
   {
      /* Print shell prompt */
      printf("%s\n", pcLine);
      iRet = fflush(stdout);
      if (iRet == -1)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Parse the line for tokens, returned in DynArray */
      oTokens = lexLine(pcLine);

      if (oTokens != NULL)
      {
         /* Use the token array to create a Command object */
         oCommand = synArr(oTokens);
         /* Print the Command, then clear the Command */
         if (oCommand != NULL) {
            writeCommand(oCommand);
            freeCommand(oCommand);
            DynArray_free(oTokens);
         }
      }
      /* Free up the line read in from stdin */
      free(pcLine);
      printf("%% ");
   }
   printf("\n");
   return 0;
}
