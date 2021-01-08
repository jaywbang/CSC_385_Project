#include <stdio.h>
#include "shell.h"
#include "symtab/symtab.h"

void printprompt1(void)
{
	struct symtab_entry_s *entry = get_symtab_entry("PS1");

    	if(entry && entry->val)
    	{
        	fprintf(stderr, "%s", entry->val);
    	}
   	else
   	{
   		fprintf(stderr, "$ ");
   	}
}

void printprompt2(void)
{
	struct symtab_entry_s *entry = get_symtab_entry("PS2");

    	if(entry && entry->val)
    	{
        	fprintf(stderr, "%s", entry->val);
    	}
    	else
    	{
        	fprintf(stderr, "> ");
    	}
}
