#ifndef SOURCE_H
#define SOURCE_H
#define endoffile (-1)
#define errchar (0)
#define initsrcpos (-2)

struct source1
{
	char *buffer;
	long bufsize;
	long curpos;
};

char nextchar(struct source1 *src);
void deletechar(struct source1 *src);
char peekchar(struct source1 *src);
void whitespaces(struct source1 *src);

#endif
