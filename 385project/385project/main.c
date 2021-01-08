#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "shell.h"
#include "source.h"
#include "parser.h"
#include "executor.h"
/*
	main(); will be the function that takes in charge of running the REPL loop 
*/
int main(int argc, char **argv)
{
    char *cmd;
    
    initsh();
    do
    {
        printprompt1(); //This will print the prompt string 
        cmd = readcmd();
        if(!cmd)
        {
            exit(EXIT_SUCCESS);
        }
        if(cmd[0] == '\0' || strcmp(cmd, "\n") == 0)
        {
            free(cmd);
            continue;
        }
        if(strcmp(cmd, "exit\n") == 0)
        {
            free(cmd);
            break;
        }
	struct source1 src;
	src.buffer = cmd;
	src.bufsize = strlen(cmd);
	src.curpos = initsrcpos;
	parseandexecute(&src);
        free(cmd);
    } 
    while(1);
    exit(EXIT_SUCCESS);
}
/*
	this function will read the users input and then will allocate the buffer size
	if more buffer is needed the realloc() will extend the buffer size to make sure 
	that the memory issue does not occur
*/
char *readcmd(void)
{
    char buf[1024];
    char *ptr = NULL;
    char ptrlen = 0;
    while(fgets(buf, 1024, stdin))
    {
        int buflen = strlen(buf);
        if(!ptr)
        {
            ptr = malloc(buflen+1);
        }
        else
        {
            char *ptr2 = realloc(ptr, ptrlen+buflen+1);
            if(ptr2)
            {
                ptr = ptr2;
            }
            else
            {
                free(ptr);
                ptr = NULL;
            }
        }
        if(!ptr)
        {
            fprintf(stderr, "Error: Failed To Allocate Buffer: %s\n", strerror(errno));
            return NULL;
        }
        strcpy(ptr+ptrlen,buf);
        if(buf[buflen-1] == '\n')
        {
            if(buflen == 1 || buf[buflen-2] != '\\')
            {
                return ptr;
            }
            ptr[ptrlen+buflen-2] = '\0';
            buflen -= 2;
            printprompt2();
        }
        ptrlen += buflen;
    }
    return ptr;
}

int parseandexecute(struct source1 *src)
{
	whitespaces(src);
	
	struct token1 *tok = tokenize(src);
	
	if(tok == &endoffile_token)
	{
		return 0;
	}
	while(tok && tok != &endoffile_token)
	{
		struct node_s *cmd = parsesimplecommand(tok);
		
		if(!cmd)
		{
			break;
		}
		do_simple_command(cmd);
		freenodetree(cmd);
		tok = tokenize(src);
	}
	return 1;
}
