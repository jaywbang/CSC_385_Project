#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "shell.h"
#include "node.h"
#include "executor.h"
/*
	this function will take the name of the command
	which will then search the directories which will 
	be listed as PATH. it will then make its attempt to 
	find the command executable file. 
*/
char *search_path(char *file)
{
	char *PATH = getenv("PATH");
    	char *p = PATH;
    	char *p2;
	
	while(p && *p)
	{
		p2 = p;
		
		while(*p2 && *p2 != ':')
		{
			p2++;
		}
		int plen = p2-p;
		if(!plen)
		{
			plen = 1;
		}
		
		int alen = strlen(file);
		char path[plen+1+alen+1];
		
		strncpy(path, p, p2-p);
		path[p2-p] = '\0';
		
		if(p2[-1] != '/')
		{
			strcat(path, "/");
		}
		strcat(path, file);
		struct stat st;
		/*
			the stat() will check to see if a file 
			is existed with the pathname. if it does
			indeed exit it will contain the command and 
			will return the full pathname of the command
			that was inputed. if the file is not found 
			the function will search the rest of the path
			in attempt to find the command. it will run until
			it finds the executable file. if it fails to find 
			the file it will return NULL;
		*/
		if(stat(path, &st) == 0)
		{
			if(!S_ISREG(st.st_mode))
			{
				errno = ENOENT;
				p = p2;
				if(*p2 == ':')
				{
					p++;
				}
				continue;
			}
			p = malloc(strlen(path)+1);
			if(!p)
			{
				return NULL;
			}
			strcpy(p,path);
			return p;
		}
		else
		{
			p = p2;
			if(*p2 == ':')
			{
				p++;
			}
		}
	}
	errno = ENOENT;
	return NULL;
}
/*
	this function will execute the command 
	that is called by the execv(); function
	this will then replace the current command
	if the command contains slash character it will
	directly call execv(); if not it will call 
	the searchpath(); which will return the path 
	and update it to the execv(); function
*/
int do_exec_cmd(int argc, char **argv)
{
	if(strchr(argv[0], '/'))
	{
		execv(argv[0],argv);
	}
	else
	{
		char *path = search_path(argv[0]);
		if(!path)
		{
			return 0;
		}
		execv(path, argv);
		free(path);
	}
	return 0;
}
/*
	this function will free the memory that was
	taken up by the arugment list of the commmand
	that was last executed 
*/
static inline void free_argv(int argc, char **argv)
{
	if(!argc)
	{
		return;
	}
	while(argc--)
	{
		free(argv[argc]);
	}
}
/*
	this function will serve as the main function 
	of the executor. this function takes the command
	and convert it into an argument list 
*/
int do_simple_command(struct node_s *node)
{
	if(!node)
	{
		return 0;
	}
	struct node_s *child = node->first_child;
	if(!child)
	{
		return 0;
	}
	int argc = 0;
	long max_args = 255;
	char *argv[max_args+1];
	char *str;
	
	while(child)
	{
		str = child->val.str;
		argv[argc] = malloc(strlen(str)+1);
		
		if(!argv[argc])
		{
			free_argv(argc, argv);
			return 0;
		}
		strcpy(argv[argc], str);
		if(++argc >= max_args)
		{
			break;
		}
		child = child->next_sibling;
	}
	argv[argc] = NULL;
	
	int i = 0;
	for( ; i < builtins_count; i++)
	{
		if(strcmp(argv[0], builtins[i].name) == 0)
		{
			builtins[i].func(argc, argv);
			free_argv(argc, argv);
			return 1;
		}
	}
	pid_t child_pid = 0;
	/*
		the fork of the child will be created in 
		which it will then call upon the 
		doexeccmd(); if the command is executed
		correctly it will not return ; if it returns
		there would be an error most likely the 
		command was not found or the memory space was 
		not allocated correctly. which will then exit using
		the exit();
	*/
	if((child_pid = fork()) == 0)
	{
		do_exec_cmd(argc, argv);
		fprintf(stderr, "Error: Failed To Execute Command: %s\n", strerror(errno));
		if(errno == ENOEXEC)
		{
			exit(126);
		}
		else if(errno == ENOENT)
		{
			exit(127);
		}
		else
		{
			exit(EXIT_FAILURE);
		}
	}
	else if(child_pid < 0)
	{
		fprintf(stderr, "Error: Failed To Fork Command: %s\n", strerror(errno));
		return 0;
	}
	int status = 0;
	/*
		the waitpid(); is called in the parent process
		which will wait for the children to finish their 
		execution, which will then free the memory which 
		was used to store the arguments and return 
	*/
	waitpid(child_pid, &status, 0);
	free_argv(argc,argv);
	return 1;
}
