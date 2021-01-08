#include <string.h>
#include "shell.h"
#include "symtab/symtab.h"

extern char **environ;
/*
	this function will initialize 
	the table and will scan the enviroment
	list. it will also add each of the variable
	and values 
*/
void initsh(void)
{
	initsymtab();
	
	struct symtab_entry_s *entry;
	
	char **p2 = environ;
	
	while(*p2)
	{
		char *eq = strchr(*p2, '=');
		if(eq)
		{
			int len = eq-(*p2);
			char name[len+1];
			
			strncpy(name, *p2, len);
			name[len] = '\0';
			entry = add_to_symtab(name);
			
			if(entry)
			{
				symtabentrysetval(entry, eq+1);
				entry->flags |= FLAG_EXPORT;
			}
		}
		else
		{
			entry = add_to_symtab(*p2);
		}
		p2++;
	}
	
	entry = add_to_symtab("PS1");
	symtabentrysetval(entry, "$");
	entry = add_to_symtab("PS2");
	symtabentrysetval(entry, ">");
}
