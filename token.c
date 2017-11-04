/*--------------------------------------------------------------------*/
/* token.c                                                            */
/* Author: Greg Umali                                                 */
/*--------------------------------------------------------------------*/

#include "token.h"
#include "ish.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*--------------------------------------------------------------------*/

/* A Token is either special or ordinary, expressed as a string. */

struct Token
{
   /* The type of the token. */
   enum TokenType eType;

   /* The string which is the token's value. */
   char *pcValue;
};

/*--------------------------------------------------------------------*/

/* Create and return a token whose type is eTokenType and whose                                   
   value consists of string pcValue.  The caller owns the token. */

struct Token *newToken(enum TokenType eTokenType,
                       char *pcValue)
{
   struct Token *psToken;

   assert(pcValue != NULL);

   /* Malloc necessary space for Token object */
   psToken = (struct Token*)malloc(sizeof(struct Token));
   if (psToken == NULL)
   {perror(getPgmName()); exit(EXIT_FAILURE);}

   psToken->eType = eTokenType;

   /* Malloc necessary space so the Token can own its value */
   psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
   if (psToken->pcValue == NULL)
   {perror(getPgmName()); exit(EXIT_FAILURE);}
   strcpy(psToken->pcValue, pcValue);

   return psToken;
}

/*--------------------------------------------------------------------*/

/* Returns the type (SPECIAL_TOKEN or ORDINARY_TOKEN) of psToken. */

enum TokenType Token_getType(Token_T psToken){
   assert(psToken != NULL);
   return psToken->eType;
}

/*--------------------------------------------------------------------*/

/* Returns the string that is the value of psToken. */

char *Token_getVal(Token_T psToken){
   assert(psToken != NULL);
   return psToken->pcValue;
}

/*--------------------------------------------------------------------*/

/* Write all tokens in oTokens to stdout.  Writes tokens in order of
   appearance, followed by whether it is ordinary or special. */

void writeTokens(DynArray_T oTokens)
{
   /* Index variables to iterate through Token DynArray */
   size_t u;
   size_t uLength;

   char* sTokenType;
   
   struct Token *psToken;

   assert(oTokens != NULL);

   uLength = DynArray_getLength(oTokens);

   for (u = 0; u < uLength; u++)
   {
      printf("Token: ");
      psToken = DynArray_get(oTokens, u);

      if(psToken->eType == SPECIAL_TOKEN)
         sTokenType = "special";
      else
         sTokenType = "ordinary";

      printf("%s (%s)\n", psToken->pcValue, sTokenType);
   }
}

/*--------------------------------------------------------------------*/

/* Free all of the tokens in oTokens. */

void freeTokens(DynArray_T oTokens)
{
   size_t u;
   size_t uLength;
   struct Token *psToken;

   assert(oTokens != NULL);

   uLength = DynArray_getLength(oTokens);

   for (u = 0; u < uLength; u++)
   {
      psToken = DynArray_get(oTokens, u);
      free(psToken->pcValue);
      free(psToken);
   }
}

/*--------------------------------------------------------------------*/

/* If no lines remain in psFile, then return NULL. Otherwise read a line                          
   of psFile and return it as a string. The string does not contain a                             
   terminating newline character. The caller owns the string. */

char *readLine(FILE *psFile)
{
   enum {INITIAL_LINE_LENGTH = 2};
   enum {GROWTH_FACTOR = 2};

   size_t uLineLength = 0;
   size_t uPhysLineLength = INITIAL_LINE_LENGTH;
   char *pcLine;
   int iChar;

   assert(psFile != NULL);

   /* If no lines remain, return NULL. */
   if (feof(psFile))
      return NULL;
   iChar = fgetc(psFile);
   if (iChar == EOF)
      return NULL;

   /* Allocate memory for the string. */
   pcLine = (char*)malloc(uPhysLineLength);
   if (pcLine == NULL)
   {perror(getPgmName()); exit(EXIT_FAILURE);}

   /* Read characters into the string. */
   while ((iChar != '\n') && (iChar != EOF))
   {
      if (uLineLength == uPhysLineLength)
      {
         uPhysLineLength *= GROWTH_FACTOR;
         pcLine = (char*)realloc(pcLine, uPhysLineLength);
         if (pcLine == NULL)
         {perror(getPgmName()); exit(EXIT_FAILURE);}
      }
      pcLine[uLineLength] = (char)iChar;
      uLineLength++;
      iChar = fgetc(psFile);
   }

   /* Append a null character to the string. */
   if (uLineLength == uPhysLineLength)
   {
      uPhysLineLength++;
      pcLine = (char*)realloc(pcLine, uPhysLineLength);
      if (pcLine == NULL)
      {perror(getPgmName()); exit(EXIT_FAILURE);}
   }
   pcLine[uLineLength] = '\0';

   return pcLine;
}
