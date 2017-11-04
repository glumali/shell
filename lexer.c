/*--------------------------------------------------------------------*/
/* lexer.c                                                            */
/* Author: Greg Umali                                                 */
/*--------------------------------------------------------------------*/

#include "lexer.h"
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

DynArray_T lexLine(const char *pcLine)
{
   /* lexLine() uses a DFA approach.  It "reads" its characters from
      pcLine. The DFA has these four states: */
   enum LexState {STATE_START, STATE_SPECIAL, 
                  STATE_ORDINARY, STATE_QUOTE};

   /* The current state of the DFA. */
   enum LexState eState = STATE_START;

   /* An index into pcLine. */
   size_t uLineIndex = 0;

   /* Pointer to a buffer in which the characters comprising each
      token are accumulated. */
   char *pcBuffer;

   /* An index into the buffer. */
   int uBufferIndex = 0;

   /* Holds each character and the finished token, which are
      all stored within the DynArray oTokens */
   char c;
   struct Token *psToken;
   DynArray_T oTokens;

   /* Checks the success of functions */
   int iSuccessful;

   assert(pcLine != NULL);

   /* Create an empty token DynArray object. */
   oTokens = DynArray_new(0);
   if (oTokens == NULL)
   {perror(getPgmName()); exit(EXIT_FAILURE);}

   /* Allocate memory for a buffer that is large enough to store the
      largest token that might appear within pcLine. */
   pcBuffer = (char*)malloc(strlen(pcLine) + 1);
   if (pcBuffer == NULL)
   {perror(getPgmName()); exit(EXIT_FAILURE);}

   for (;;)
   {
      /* "Read" the next character from pcLine. */
      c = pcLine[uLineIndex++];

      switch (eState)
      {
         /* Handle the START state. */
         case STATE_START:
            /* NULL, or EOF */
            if (c == '\0')
            {
               /* Exit */
               free(pcBuffer);
               return oTokens;
            }
            /* Special characters */
            else if (c == '<' || c == '>')
            {
               pcBuffer[uBufferIndex++] = c;

               /* Create a SPECIAL token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = newToken(SPECIAL_TOKEN, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
               {perror(getPgmName()); exit(EXIT_FAILURE);}
               uBufferIndex = 0;

               eState = STATE_SPECIAL;
            }
            /* Start of a quote */
            else if (c == '"')
            {
               eState = STATE_QUOTE;
            }
            else if (c == ' ')
               eState = STATE_START;
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_ORDINARY;
            }
            break;

            /* Handle the SPECIAL state. */
         case STATE_SPECIAL:
            if (c == '\0')
            {
               /* Exit */
               free(pcBuffer);
               return oTokens;
            }
            else if (c == '<' || c == '>')
            {
               pcBuffer[uBufferIndex++] = c;

               /* Create a SPECIAL token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = newToken(SPECIAL_TOKEN, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
               {perror(getPgmName()); exit(EXIT_FAILURE);}
               uBufferIndex = 0;

               eState = STATE_SPECIAL;
            }
            else if (c == ' ')
            {
               eState = STATE_SPECIAL;
            }
            /* Start of a quote */
            else if (c == '"')
            {
               eState = STATE_QUOTE;
            }
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_ORDINARY;
            }
            break;

            /* Handle the ORDINARY state. */
         case STATE_ORDINARY:
            if (c == '\0')
            {
               /* Create an ORDINARY token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = newToken(ORDINARY_TOKEN, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
               {perror(getPgmName()); exit(EXIT_FAILURE);}
               uBufferIndex = 0;
               /* Exit */
               free(pcBuffer);
               return oTokens;
            }
            /* Special character */
            else if (c == '<' || c == '>')
            {
               /* Create an ORDINARY token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = newToken(ORDINARY_TOKEN, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
               {perror(getPgmName()); exit(EXIT_FAILURE);}
               uBufferIndex = 0;

               pcBuffer[uBufferIndex++] = c;

               /* Create a SPECIAL token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = newToken(SPECIAL_TOKEN, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
               {perror(getPgmName()); exit(EXIT_FAILURE);}
               uBufferIndex = 0;

               eState = STATE_SPECIAL;
            }
            /* Start of a quote */
            else if (c == '"')
            {
               eState = STATE_QUOTE;
            }
            else if (c == ' ')
            {
               /* Create an ORDINARY token. */
               pcBuffer[uBufferIndex] = '\0';
               psToken = newToken(ORDINARY_TOKEN, pcBuffer);
               iSuccessful = DynArray_add(oTokens, psToken);
               if (! iSuccessful)
               {perror(getPgmName()); exit(EXIT_FAILURE);}
               uBufferIndex = 0;

               eState = STATE_START;
            }
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_ORDINARY;
            }
            break;

            /* Handle the QUOTE state. */
         case STATE_QUOTE:
            /* Cannot exit with an open quote */
            if (c == '\0')
            {
               fprintf(stderr, "%s: unmatched quote\n", getPgmName());
               free(pcBuffer);
               freeTokens(oTokens);
               DynArray_free(oTokens);
               return NULL;
            }
            /* End of a quote */
            else if (c == '"')
            {
               eState = STATE_ORDINARY;
            }
            else
            {
               pcBuffer[uBufferIndex++] = c;
               eState = STATE_QUOTE;
            }
            break;

         default:
            assert(0);
      }
   }
}
