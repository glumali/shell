/*--------------------------------------------------------------------*/
/* lexer.h                                                            */
/* Author: Greg Umali                                                 */
/*--------------------------------------------------------------------*/

#ifndef LEXER_INCLUDED
#define LEXER_INCLUDED

#include <stddef.h>
#include "dynarray.h"

/*--------------------------------------------------------------------*/

/* Analyzes the line pcLine and classifies each token as ordinary or
   special (IO redirect). Return a new DynArray_T object whose length 
   is uLength, or NULL if insufficient memory is available. */

DynArray_T lexLine(const char *pcLine);

/*--------------------------------------------------------------------*/

#endif
