#include <errno.h>
#include "shell.h"
#include "source.h"
/*
	retrieves the last characters from user's input and puts it back into the 
	input source. 
*/
void deletechar(struct source1 *src)
{
	if(src->curpos < 0)
	{
		return;
	}
	src->curpos--;
}
/*
	this function will return the next characters of input and will update the 
	sources pointer. the next time this function is called upon it will return 
	the input character. when the function reaches to the last charcter in the 
	input it will return the End Of File. 
*/
char nextchar(struct source1 *src)
{
	if(!src || !src->buffer)
	{
		errno = ENODATA;
		return errchar;
	}
	char c1 = 0;
	if(src->curpos == initsrcpos)
	{
		src->curpos = -1;
	}
	else
	{
		c1 = src->buffer[src->curpos];
	}
	if(++src->curpos >= src->bufsize)
	{
		src->curpos = src->bufsize;
		return endoffile;
	}
	return src->buffer[src->curpos];
}
/*
	this function will perform similar to the nextchar(); in which it will return 
	the next characters of input. however, this function will not update its source
	pointers so the next time the nextchar(); is called upon it will return the 
	same input characters. 
*/
char peekchar(struct source1 *src)
{
	if(!src || !src->buffer)
	{
		errno = ENODATA;
		return errchar;
	}
	
	long pos = src->curpos;
	
	if(pos == initsrcpos)
	{
		pos++;
	}
	pos++;
	if(pos >= src->bufsize)
	{
		return endoffile;
	}
	return src->buffer[pos];
}
void whitespaces(struct source1 *src)
{
	char c;
	
	if(!src || !src->buffer)
	{
		return;
	}
	
	while(((c = peekchar(src)) != endoffile) && (c == ' ' || c == '\t'))
	{
		nextchar(src);
	}
}
