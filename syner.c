/*--------------------------------------------------------------------*/
/* syner.c                                                            */
/* Author: Greg Umali                                                 */
/*--------------------------------------------------------------------*/

#include "syner.h"
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

/* Lexically analyze string pcLine.  If pcLine contains a lexical
   error, then return NULL.  Otherwise return a DynArray object
   containing the tokens in pcLine.  The caller owns the DynArray
   object and the tokens that it contains. */

Command_T synArr(DynArray_T tokens)
{
   /* synArr() uses a DFA approach.  It "reads" its characters from
      pcLine. The DFA has these four states: */
   enum LexState {STATE_START, STATE_COMMAND, 
                  STATE_INREDIR, STATE_OUTREDIR};

   /* The current state of the DFA. */
   enum LexState eState = STATE_START;


   /* Will store the final Command object that is returned */
   Command_T oCommand;

   /* Will store the new Command's name */
   char *pcName;

   /* Stores the arguments parsed from the token DynArray as strings */
   DynArray_T oArgs;

   /* Stores input/output redirect locations */
   char *pcInFile = NULL;
   char *pcOutFile = NULL;


   /* Will store the "working" token */
   Token_T psToken;

   /* Flags to track whether stdin or stdout has been redirected */
   int iInFlag = 0;
   int iOutFlag = 0;

   /* Checks the success of functions */
   int iSuccessful;

   /* Index variables */
   size_t u;
   size_t uLen = DynArray_getLength(tokens);
   size_t uArgIdx;


   assert(tokens != NULL);

   /* Create an empty token DynArray object. */
   oArgs = DynArray_new(0);
   if (oArgs == NULL)
   {perror(getPgmName()); exit(EXIT_FAILURE);}

   for (u = 0; u < uLen; u++)
   {
      psToken = DynArray_get(tokens, u);
      
      if (psToken == NULL)
      {
         perror(getPgmName()); 
         exit(EXIT_FAILURE);
      }

      switch (eState)
      {
         /* Handle the START state. */
         case STATE_START:
            /* NULL, or EOF */
            if (Token_getVal(psToken) == NULL)
            {
               /* Exit, just prints another prompt */
               return NULL;
            }
            /* Can't start with a special character */
            else if (Token_getType(psToken) == SPECIAL_TOKEN)
            {
               fprintf(stderr, "%s: missing command name\n", 
                       getPgmName());
               /* Iterate through array and free all strings */
               for (uArgIdx = 0; uArgIdx < DynArray_getLength(oArgs); 
                    uArgIdx++) {
                  free(DynArray_get(oArgs, uArgIdx));
               }
               DynArray_free(oArgs);
               return NULL;
            }
            /* Ordinary token */
            else
            {
               /* Store as the command name */
               pcName = Token_getVal(psToken);
               eState = STATE_COMMAND;
            }
            break;

            /* Handle the COMMAND state. */
         case STATE_COMMAND:
            /* NULL, or EOF */
            if (Token_getVal(psToken) == NULL)
            {
               /* Exit, makes Command object */
               oCommand = newCommand(pcName, oArgs, 
                                     pcInFile, pcOutFile);
               if (!oCommand)
               {perror(getPgmName()); exit(EXIT_FAILURE);}
               return oCommand;
            }
            else if (Token_getType(psToken) == SPECIAL_TOKEN && 
                     strcmp(Token_getVal(psToken), "<") == 0)
            {
               /* If flag already set, can't have two stdin redirects */
               if(iInFlag) {
                  fprintf(stderr, 
                         "%s: multiple redirection of standard input\n",
                         getPgmName());
                  /* Iterate through array and free all strings */
                  for (uArgIdx = 0; uArgIdx < DynArray_getLength(oArgs);
                       uArgIdx++) {
                     free(DynArray_get(oArgs, uArgIdx));
                  }
                  DynArray_free(oArgs);
                  return NULL;
               }
               else {
                  /* Set iInFlag */
                  iInFlag = 1;
                  eState = STATE_INREDIR;
               }
            }
            else if (Token_getType(psToken) == SPECIAL_TOKEN && 
                     strcmp(Token_getVal(psToken), ">") == 0)
            {
               /* If flag set, can't have two stdout redirects */
               if(iOutFlag) {
                  fprintf(stderr, 
                        "%s: multiple redirection of standard output\n",
                        getPgmName());
                  /* Iterate through array and free all strings */
                  for (uArgIdx = 0; uArgIdx < DynArray_getLength(oArgs);
                       uArgIdx++) {
                     free(DynArray_get(oArgs, uArgIdx));
                  }
                  DynArray_free(oArgs);
                  return NULL;
               }
               else {
                  /* Set iOutFlag */
                  iOutFlag = 1;
                  eState = STATE_OUTREDIR;
               }
            }
            /* Ordinary token is just added to arguments */
            else
            {
               /* Add the token to the oArgs array */
               iSuccessful = DynArray_add(oArgs, Token_getVal(psToken));
               if (!iSuccessful)
               {perror(getPgmName()); exit(EXIT_FAILURE);}

               eState = STATE_COMMAND;
            }
            break;

            /* Handle the INREDIR state. */
         case STATE_INREDIR:
            if (Token_getVal(psToken) == NULL || 
                Token_getType(psToken) == SPECIAL_TOKEN)
            {
               /* Token immediately after must be an ordinary token. */
               fprintf(stderr, 
                   "%s: standard input redirection without file name\n",
                   getPgmName());
               /* Iterate through array and free all strings */
               for (uArgIdx = 0; uArgIdx < DynArray_getLength(oArgs); 
                    uArgIdx++) {
                  free(DynArray_get(oArgs, uArgIdx));
               }
               DynArray_free(oArgs);
               return NULL;
            }
            /* Ordinary token */
            else
            {
               /* Store the ordinary token as the stdin location. */
               pcInFile = Token_getVal(psToken);
               eState = STATE_COMMAND;
            }
            break;

            /* Handle the OUTREDIR state. */
         case STATE_OUTREDIR:
            if (Token_getVal(psToken) == NULL || 
                Token_getType(psToken) == SPECIAL_TOKEN)
            {
               /* Token immediately after must be an ordinary token. */
               fprintf(stderr, 
                  "%s: standard output redirection without file name\n",
                  getPgmName());
               /* Iterate through array and free all strings */
               for (uArgIdx = 0; uArgIdx < DynArray_getLength(oArgs); 
                    uArgIdx++) {
                  free(DynArray_get(oArgs, uArgIdx));
               }
               DynArray_free(oArgs);
               return NULL;
            }
            /* Ordinary token */
            else
            {
               /* Store the ordinary token as the stdin location. */
               pcOutFile = Token_getVal(psToken);
               eState = STATE_COMMAND;
            }
            break;

         default:
            assert(0);
      }
   }
   if (eState == STATE_COMMAND)
   {
      /* Reaches the end of all tokens; exit, makes Command object */
      oCommand = newCommand(pcName, oArgs, pcInFile, pcOutFile);
      if (!oCommand)
      {perror(getPgmName()); exit(EXIT_FAILURE);}
      return oCommand;
   }
   else if (eState == STATE_INREDIR)
   {
      /* Token immediately after must be an ordinary token. */
      fprintf(stderr, 
              "%s: standard input redirection without file name\n", 
              getPgmName());
      /* Iterate through array and free all strings */
      for (uArgIdx = 0; uArgIdx < DynArray_getLength(oArgs); 
           uArgIdx++) {
         free(DynArray_get(oArgs, uArgIdx));
      }
      DynArray_free(oArgs);
      return NULL;
   }
   else if (eState == STATE_OUTREDIR)
   {
      /* Token immediately after must be an ordinary token. */
      fprintf(stderr, 
              "%s: standard output redirection without file name\n", 
              getPgmName());
      /* Iterate through array and free all strings */
      for (uArgIdx = 0; uArgIdx < DynArray_getLength(oArgs); 
           uArgIdx++) {
         free(DynArray_get(oArgs, uArgIdx));
      }
      DynArray_free(oArgs);
      return NULL;
   }
   else 
   {
      return NULL;
   }
}
