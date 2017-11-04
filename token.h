/*--------------------------------------------------------------------*/
/* token.h                                                            */
/* Author: Greg Umali                                                 */
/*--------------------------------------------------------------------*/

#ifndef TOKEN_INCLUDED
#define TOKEN_INCLUDED

#include <stdio.h>
#include <stddef.h>
#include "dynarray.h"

/*--------------------------------------------------------------------*/

/* A Token object can be either special or ordinary. */
enum TokenType {SPECIAL_TOKEN, ORDINARY_TOKEN};

/*--------------------------------------------------------------------*/

/* A Token_T object is a buffer of characters read from user input. */

typedef struct Token *Token_T;

/*--------------------------------------------------------------------*/

/* Create and return a token whose type is eTokenType and whose
   value consists of string pcValue.  The caller owns the token. */

Token_T newToken(enum TokenType eTokenType, char *pcValue);

/*--------------------------------------------------------------------*/

/* Returns the type (SPECIAL_TOKEN or ORDINARY_TOKEN) of psToken. */

enum TokenType Token_getType(Token_T psToken);

/*--------------------------------------------------------------------*/

/* Returns the string that is the value of psToken. */

char *Token_getVal(Token_T psToken);

/*--------------------------------------------------------------------*/

/* Write all tokens in oTokens to stdout.  Writes tokens in order of
   appearance, followed by whether it is ordinary or special. */

void writeTokens(DynArray_T oTokens);

/*--------------------------------------------------------------------*/

/* Free all of the tokens in oTokens. */

void freeTokens(DynArray_T oTokens);

/*--------------------------------------------------------------------*/

/* If no lines remain in psFile, then return NULL. Otherwise read a line
   of psFile and return it as a string. The string does not contain a
   terminating newline character. The caller owns the string. */

char *readLine(FILE *psFile);

/*--------------------------------------------------------------------*/

#endif
