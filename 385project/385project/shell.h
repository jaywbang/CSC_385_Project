#ifndef SHELL_H
#define SHELL_H

void printprompt1(void);
void printprompt2(void);

char *readcmd(void);

#include "source.h"

int parseandexecute(struct source1 *src);

void initsh(void);

int dump(int argc, char **argv);

struct builtin_s
{
	char *name;
	int (*func)(int argc, char **argv);
};
extern struct builtin_s builtins[];
extern int builtins_count;
#endif
