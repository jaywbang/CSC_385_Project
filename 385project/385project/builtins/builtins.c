#include "../shell.h"
/*
	this will dump all the table in to the screen
*/
struct builtin_s builtins[] =
{   
    {"dump", dump},
};

int builtins_count = sizeof(builtins)/sizeof(struct builtin_s);
