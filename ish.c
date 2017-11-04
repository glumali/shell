/*--------------------------------------------------------------------*/
/* ish.c                                                              */
/* Author: Greg Umali                                                 */
/*--------------------------------------------------------------------*/

#define _GNU_SOURCE

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
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

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

/* This function is intended to be a signal handler.  Write a message
   to stdout that contains iSignal, the number of the signal that
   caused the function to be called. */

void myHandler(int iSignal)
{
   int iRet;
   void (*pfRet)(int);
   
   /* Install myHandler2 as the handler for SIGINT signals. */
   pfRet = signal(SIGINT, myHandler2);
   if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE); }

   alarm(5);
}

/*--------------------------------------------------------------------*/

/* This function is intended to be a signal handler.  Write a message
   to stdout that contains iSignal, the number of the signal that
   caused the function to be called. */

void myHandler2(int iSignal)
{
   printf("To exit the shell, issue an 'exit' command");
}

/*--------------------------------------------------------------------*/

void alarmHandler(int iSignal)
{
   void (*pfRet)(int);
   pfRet = signal(SIGINT, myHandler);
}

/*--------------------------------------------------------------------*/

/* Accepts a DynArray of tokens, and parses it to return a command.
   A command must begin with an ordinary token, must have at most one
   stdin redirect and stdout redirect each, and cannot follow a 
   redirect token with another special character or terminating the 
   program. Executes the command, and repeats until EOF. Returns 0 
   iff successful. As always, argc is the command-line argument count 
   and argv is an array of arguments. */

int main(int argc, char *argv[])
{
   /* Line read in from user from stdin */
   char *pcLine;

   /* Process ID, used to determine if parent or child */
   pid_t iPid;

   /* Used to determine the success of functions */
   int iRet;

   /* Holds the Tokens created after a lexLine() call */
   DynArray_T oTokens;
   /* Holds the Command created after a synArr() call */
   Command_T oCommand;

   /* Stores the name of the command */
   char* pcCommandName;
   /* Stores the arguments of the command */
   DynArray_T oArgs;

   /* Used to iterate through the arguments of the Command */
   size_t u;
   size_t uLength;

   /* String for a path variable for the cd command */
   char *pcPath;

   /* For signal handling */
   void (*pfRet)(int);
   sigset_t sSet;

   /* The permissions of the newly-created file. */
   enum {PERMISSIONS = 0600};

   /* Denotes that the setenv command can overwrite a variable */
   enum {OVERWRITE_ON = 1};

   pcPgmName = argv[0];

   /* Print shell prompt */
   printf("%% ");

   while ((pcLine = readLine(stdin)) != NULL)
   {
      /* Read line from user stdin */
      printf("%s\n", pcLine);
      iRet = fflush(stdout);
      if (iRet == -1)
      {perror(pcPgmName); exit(EXIT_FAILURE);}

      /* Parse the line and return DynArray of tokens */
      oTokens = lexLine(pcLine);
      if(oTokens != NULL)
      {
         /* Parse the tokens array and return */
         oCommand = synArr(oTokens);
         if (oCommand != NULL)
         {
            iRet = fflush(NULL);
            if (iRet == EOF) {perror(pcPgmName); exit(EXIT_FAILURE); }

            oArgs = Command_getArgs(oCommand);
            uLength = DynArray_getLength(oArgs);

            pcCommandName = Command_getName(oCommand);

            /* Make sure SIGINT signals are not blocked. */
            iRet = sigemptyset(&sSet);
            if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE); }
            iRet = sigaddset(&sSet, SIGINT);
            if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE); }
            iRet = sigprocmask(SIG_UNBLOCK, &sSet, NULL);
            if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE); }
            
            /* Restore myHandler for SIGINT signals. */
            pfRet = signal(SIGINT, myHandler);
            if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE); }

            /* Make sure that SIGALRM signals are not blocked. */
            iRet = sigemptyset(&sSigSet);
            if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE); }   
            iRet = sigaddset(&sSigSet, SIGALRM);
            if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE); }   
            iRet = sigprocmask(SIG_UNBLOCK, &sSigSet, NULL);
            if (iRet == -1) {perror(pcPgmName); exit(EXIT_FAILURE); }

            /* Install myHandler as the handler for SIGALRM signals. */
            pfRet = signal(SIGALRM, myHandler);
            if (pfRet == SIG_ERR) {perror(pcPgmName); exit(EXIT_FAILURE); }


            /* Implementation of the "exit" command */
            if (strcmp(pcCommandName, "exit") == 0) 
            {
               if (uLength == 0) {exit(0); }
               /* Error if the exit function has arguments */
               else 
               {
                  fprintf(stderr, "%s: too many arguments\n", 
                          getPgmName());
               }
            }

            /* Implementation of "setenv" command */
            else if (strcmp(pcCommandName, "setenv") == 0) 
            {
               switch(uLength) {
                  case 0:
                     /* Error to have 0 command line arguments */
                     fprintf(stderr, "%s: missing variable\n", 
                             getPgmName());
                     break;
                  case 1: 
                     /* Value omitted, argument is the var */
                     iRet = setenv(DynArray_get(oArgs, 0), "", 
                                   OVERWRITE_ON);
                     if(iRet == -1)
                     {perror(pcPgmName); exit(EXIT_FAILURE); }
                     break;
                  case 2: 
                     /* Var and value must have been specified */
                     iRet = setenv(DynArray_get(oArgs, 0), 
                                   DynArray_get(oArgs, 1), OVERWRITE_ON);
                     if(iRet == -1)
                     {perror(pcPgmName); exit(EXIT_FAILURE); }
                     break;
                  default:
                     /* Over two command line arguments is an error */
                     fprintf(stderr, "%s: too many arguments\n", 
                             getPgmName());
               }
            }

            /* Implementation of "unsetenv" command */
            else if (strcmp(pcCommandName, "unsetenv") == 0) 
            {
               switch(uLength) {
                  case 0:
                     /* Error to have 0 command line arguments */
                     fprintf(stderr, "%s: missing variable\n", 
                             getPgmName());
                     break;
                  case 1: 
                     /* Var has been specified, call unsetenv */
                     iRet = unsetenv(DynArray_get(oArgs, 0));
                     if(iRet == -1)
                     {perror(pcPgmName); exit(EXIT_FAILURE); }
                     break;
                  default:
                     /* Over one command line arguments is an error */
                     fprintf(stderr, "%s: too many arguments\n", 
                             getPgmName());
               }
            }

            /* Implementation of "cd [dir]" command */
            else if (strcmp(pcCommandName, "cd") == 0) 
            {
               switch(uLength) {
                  case 0:
                     /* Stores value of HOME into pcPath */ 
                     pcPath = getenv("HOME");
                     /* Error to have 0 command line arguments if HOME 
                        is not set */
                     if(pcPath == NULL)
                     {
                        fprintf(stderr, "%s: HOME is not set\n", 
                                getPgmName());
                     }
                     /* Calls chdir to change directory */
                     iRet = chdir(pcPath);
                     if(iRet == -1)
                     {perror(pcPgmName); exit(EXIT_FAILURE); }
                     break;
                  case 1: 
                     /* Calls chdir to change directory */
                     iRet = chdir(DynArray_get(oArgs, 0));
                     if(iRet == -1)
                     {perror(pcPgmName); exit(EXIT_FAILURE); }
                     break;
                  default:
                     /* Over one command line arguments is an error */
                     fprintf(stderr, "%s: too many arguments\n", 
                             getPgmName());
               }
            }

            else 
            {
               iPid = fork();
               if (iPid == -1) {perror(pcPgmName); exit(EXIT_FAILURE); }

               if (iPid == 0)
               {
                  /* This code is executed by the child process only. */

                  /* The working string argument */
                  char *pcArgument;

                  /* Integer file descriptor for IO redirection */
                  int iFd;
                  /* File names of stdIn and stdOut redirect */
                  char *pcIn;
                  char *pcOut;

                  /* New string array to hold the args of the Command */
                  char **pcArgs = (char **)malloc(sizeof(char*) 
                                                  * (uLength + 1) + 1);
                  if (pcArgs == NULL) 
                  {
                     perror(pcPgmName); 
                     exit(EXIT_FAILURE); 
                  }

                  pcIn = Command_getStdin(oCommand);
                  pcOut = Command_getStdout(oCommand);

                  if (pcIn != NULL)
                  {
                     /* Opens the stdin redirect location */
                     iFd = open(pcIn, O_RDONLY);
                     if (iFd == -1) 
                     {perror(pcPgmName); exit(EXIT_FAILURE); }

                     /* Closes fd for stdin */
                     iRet = close(0);
                     if (iRet == -1) 
                     {perror(pcPgmName); exit(EXIT_FAILURE); }

                     /* The fd for stdin redirect now goes to fd 0 */
                     iRet = dup(iFd);
                     if (iRet == -1) 
                     {perror(pcPgmName); exit(EXIT_FAILURE); }

                     /* Closes the temporary stdin fd */
                     iRet = close(iFd);
                     if (iRet == -1) 
                     {perror(pcPgmName); exit(EXIT_FAILURE); }
                  }

                  if (pcOut != NULL)
                  {
                     /* Opens the stdout redirect location */
                     iFd = creat(pcOut, PERMISSIONS);
                     if (iFd == -1) 
                     {perror(pcPgmName); exit(EXIT_FAILURE); }

                     /* Closes fd for stdout */
                     iRet = close(1);
                     if (iRet == -1) 
                     {perror(pcPgmName); exit(EXIT_FAILURE); }

                     /* The fd for stdout redirect now goes to fd 0 */
                     iRet = dup(iFd);
                     if (iRet == -1) 
                     {perror(pcPgmName); exit(EXIT_FAILURE); }

                     /* Closes the temporary stdout fd */
                     iRet = close(iFd);
                     if (iRet == -1) 
                     {perror(pcPgmName); exit(EXIT_FAILURE); }
                  }

                  /* First element is the name of the command */
                  *pcArgs = pcCommandName;

                  for (u = 0; u < uLength; u++)
                  {
                     pcArgument = DynArray_get(oArgs, u);
                     /* Set each element of pcArgs to the corresponding
                        element of oArgs from oCommand */
                     *(pcArgs + u + 1) = pcArgument;
                  }
                  /* Set last element to the null character */
                  *(pcArgs + u + 1) = NULL;

                  execvp(pcCommandName, pcArgs);
                  perror(pcPgmName);
                  exit(EXIT_FAILURE);
               }

               /* This code is executed by the parent process only. */

               /* Wait for the child process to exit. */
               iPid = wait(NULL);
               if (iPid == -1) {perror(pcPgmName); exit(EXIT_FAILURE); }
            }
         }
      }
      free(pcLine);
      printf("%% ");
   }
   printf("\n");
   return 0;
}
