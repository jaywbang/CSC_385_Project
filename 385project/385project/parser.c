#include <unistd.h>
#include "shell.h"
#include "parser.h"
#include "scanner.h"
#include "node.h"
#include "source.h"

/*
	this function will parse a simple command 
	in order to parse the simple command it will call 
	upon the tokenize(); function to gather the input 
	token and until the end of input the function will return 
	the end of file token. 
*/
struct node_s *parsesimplecommand(struct token1 *tok)
{
	if(!tok)
	{
		return NULL;
	}
	
	struct node_s *cmd = new_node(NODE_COMMAND);
	if(!cmd)
	{
		freetoken(tok);
		return NULL;
	}
	
	struct source1 *src = tok-> src;
	
	do 
	{
		if(tok->text[0] == '\n')
		{
			freetoken(tok);
			break;
		}
		
		struct node_s *word = new_node(NODE_VAR);
		if(!word)
		{
			freenodetree(cmd);
			freetoken(tok);
			return NULL;
		}
		setnodevalstr(word, tok->text);
		addchildnode(cmd, word);
		freetoken(tok);
	}
	while((tok = tokenize(src)) != &endoffile_token);
	
	return cmd;
}
