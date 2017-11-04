/*--------------------------------------------------------------------*/
/* command.h                                                          */
/* Author: Greg Umali                                                 */
/*--------------------------------------------------------------------*/

#ifndef COMMAND_INCLUDED
#define COMMAND_INCLUDED

#include <stdio.h>
#include <stddef.h>
#include "dynarray.h"
#include "token.h"

/*--------------------------------------------------------------------*/

/* A Command_T object is used in Linux shells and contains information
   on the command name, arguments, stdin, and stdout locations. */

typedef struct Command *Command_T;

/*--------------------------------------------------------------------*/

/* Create and return a command whose name is pcName with arguments                                 
   list oArgs, stdin redirected to fd pcInFile, and stdout
   redirected to fd pcOutFile.  The caller owns the command. */

struct Command *newCommand(char *pcName, DynArray_T oArgs, 
                           char *pcInFile, char *pcOutFile);

/*--------------------------------------------------------------------*/

/* Returns the name of the Command object oCommand as a string. */

char* Command_getName(Command_T oCommand);

/*--------------------------------------------------------------------*/

/* Returns the argument DynArray of the Command object oCommand. */

DynArray_T Command_getArgs(Command_T oCommand);

/*--------------------------------------------------------------------*/

/* Returns the stdin location of oCommand as a string. */

char* Command_getStdin(Command_T oCommand);

/*--------------------------------------------------------------------*/

/* Returns the stdout location of oCommand as a string. */

char* Command_getStdout(Command_T oCommand);

/*--------------------------------------------------------------------*/

/* Outputs the details of Command object oCommand to stdout.  Writes 
   the name of the command, followed by its arguments, stdin redirect
   location, and stdout redirect location, if applicable. */

void writeCommand(Command_T oCommand);

/*--------------------------------------------------------------------*/

/* Free all of the data associated data with oCommand. */

void freeCommand(Command_T oCommand);

/*--------------------------------------------------------------------*/

#endif
