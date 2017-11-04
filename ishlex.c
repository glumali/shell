/*--------------------------------------------------------------------*/
/* ishlex.c                                                           */
/* Author: Greg Umali                                                 */
/*--------------------------------------------------------------------*/

#include "dynarray.h"
#include "ish.h"
#include "lexer.h"
#include "token.h"
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

/* Read a line from stdin.  Write the line to stdout. If the line 
   contains unpaired quotation marks, then write an error message to 
   stderr and reject the line. Otherwise write to stdout each token 
   that the line contains followed by whether it is a special or
   normal token.  Repeat until EOF. Return 0 iff successful. As always, 
   argc is the command-line argument count and argv is an array of 
   command-line arguments. */

int main(int argc, char *argv[])
{
   /* Holds the line read in from standard input */
   char *pcLine;
   /* Holds the tokens parsed from the line */
   DynArray_T oTokens;
   /* Used to determine the success of functions */
   int iRet;

   pcPgmName = argv[0];

   printf("%% ");

   while ((pcLine = readLine(stdin)) != NULL)
   {
      /* Print shell prompt */
      printf("%s\n", pcLine);
      iRet = fflush(stdout);
      if (iRet == -1)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Parse line, return as DynArray of Tokens */
      oTokens = lexLine(pcLine);
      /* Print the Tokens, then free oTokens and Tokens inside it */
      if (oTokens != NULL)
      {
         writeTokens(oTokens);
         freeTokens(oTokens);
         DynArray_free(oTokens);
      }
      /* Free the parsed line */
      free(pcLine);
      printf("%% ");
   }
   printf("\n");
   return 0;
}
