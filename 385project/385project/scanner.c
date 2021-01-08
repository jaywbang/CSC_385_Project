#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "shell.h"
#include "scanner.h"
#include "source.h"

char *tok_buf = NULL;
int tok_bufsize = 0;
int tok_bufindex = -1;

struct token1 endoffile_token = 
{
	.text_len = 0,
};
/*
	this fucntion adds the single character to 
	the tokens buffer, if the buffer is full it 
	will extend the buffer by using the realloc();
*/
void addtobuff(char c)
{
	tok_buf[tok_bufindex++] = c;
	
	if(tok_bufindex >= tok_bufsize)
	{
		char *tmp = realloc(tok_buf, tok_bufsize*2);
		if(!tmp)
		{
			errno = ENOMEM;
			return;
		}
		tok_buf = tmp;
		tok_bufsize *= 2;
	}
}
/*
	this function will take the string and convert 
	the string into a token. this will also make 
	sure that the memory is set by allocating it. 
	this function will then fill in the structure 
	member fields. 
*/
struct token1 *create_token(char *str)
{
	struct token1 *tok = malloc(sizeof(struct token1));
	if(!tok)
	{
		return NULL;
	}
	memset(tok, 0, sizeof(struct token1));
	tok->text_len = strlen(str);
	
	char *nstr = malloc(tok->text_len+1);
	
	if(!nstr)
	{
		free(tok);
		return NULL;
	}
	strcpy(nstr, str);
	tok->text = nstr;
	return tok;
}
/*
	this function purpose is to free the memory
	that is taken up by the token structure, 
	also it will free the memory that is used 
	for the token's text 
*/
void freetoken(struct token1 *tok)
{
	if(tok->text)
	{
		free(tok->text);
	}
	free(tok);
}
/*
	this function will allocate the memory for the 
	tokens buffer. after it will then initialzie the 
	buffer and the sources pointer. 
*/
struct token1 *tokenize(struct source1 *src)
{
	int endloop = 0;
	
	if(!src || !src->buffer || !src->bufsize)
	{
		errno = ENODATA;
		return &endoffile_token;
	}
	
	if(!tok_buf)
	{
		tok_bufsize = 1024;
		tok_buf = malloc(tok_bufsize);
		if(!tok_buf)
		{
			errno = ENOMEM;
			return &endoffile_token;
		}
	}
	tok_bufindex = 0;
	tok_buf[0] = '\0';
	
	/*
		calls upon the nextchar(); function which 
		will then retrieve the input characters
		when the end of the input is reached then 
		it will return the End Of File Token. 
	*/
	char nc = nextchar(src);
	
	if(nc == errchar || nc == endoffile)
	{
		return &endoffile_token;
	}
	/*
		this will then will perform the loop which 
		will read the input character. if a whitespace 
		is encounters it will check the buffer to see 
		if it is fulled or empty. if the buffer is full
		it will break out of the loop, if the buffer is 
		empty it will skip the whitespace character 
		and go on to the next token. 
	*/
	do 
	{
		switch(nc)
		{
			case ' ':
			case '\t':
			if(tok_bufindex > 0)
			{
				endloop = 1;
			}
			break;
			case '\n':
			if(tok_bufindex > 0)
			{
				deletechar(src);
			}
			else
			{
				addtobuff(nc);
			}
			endloop = 1;
			break;
			default:
			addtobuff(nc);
			break;
		}
		if(endloop)
		{
			break;
		}
	}
	while ((nc = nextchar(src)) != endoffile);
	
	if(tok_bufindex == 0)
	{
		return &endoffile_token;
	}
	if(tok_bufindex >= tok_bufsize)
	{
		tok_bufindex--;
	}
	tok_buf[tok_bufindex] = '\0';
	/*
		the function will then call create_token();
		which will convert the token text to a token
		structure and will return it to the caller 
	*/
	struct token1 *tok = create_token(tok_buf);
	if(!tok)
	{
		fprintf(stderr, "Error: Failed To Allocate Buffer: %s\n", strerror(errno));
		return &endoffile_token;
	}
	tok->src = src;
	return tok;
}
