/*--------------------------------------------------------------------*/
/* syner.h                                                            */
/* Author: Greg Umali                                                 */
/*--------------------------------------------------------------------*/

#ifndef SYNER_INCLUDED
#define SYNER_INCLUDED

#include <stddef.h>
#include "dynarray.h"
#include "command.h"

/*--------------------------------------------------------------------*/

/* synArr lexically analyzes the token array token.  If pcLine 
   contains a lexical error, then return NULL.  Otherwise return a 
   Command object using the tokens from tokens.  The caller owns the 
   Command object. */

Command_T synArr(DynArray_T tokens);

/*--------------------------------------------------------------------*/
#endif
