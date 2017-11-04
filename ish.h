/*--------------------------------------------------------------------*/
/* ish.h                                                              */
/* Author: Greg Umali                                                 */
/*--------------------------------------------------------------------*/

#ifndef ISH_INCLUDED
#define ISH_INCLUDED

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*--------------------------------------------------------------------*/

/* Returns the name of the executable binary file. */

const char *getPgmName(void);

/*--------------------------------------------------------------------*/

void myHandler(int iSignal);

/*--------------------------------------------------------------------*/

void myHandler2(int iSignal);

/*--------------------------------------------------------------------*/

void alarmHandler(int iSignal)

/*--------------------------------------------------------------------*/

#endif
