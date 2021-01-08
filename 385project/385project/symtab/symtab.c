#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../shell.h"
#include "../node.h"
#include "../parser.h"
#include "symtab.h"

struct symtab_stack_s symtab_stack; //this will serve the pointer for the symbol table stack
int symtab_level; //this variable is the current level in the stack
/*
	this function will serve the purpose to initialize the symbol 
	table stack which will then allocate memory fand initialize 
	the symbol table 
*/
void initsymtab(void)
{
	symtab_stack.symtab_count = 1;
	symtab_level = 0;
	
	struct symtab_s *global_symtab = malloc(sizeof(struct symtab_s));
	
	if(!global_symtab)
	{
		fprintf(stderr, "Fatal Error: No Memory For Global Symbol Table\n");
		exit(EXIT_FAILURE);
	}
	
	memset(global_symtab, 0, sizeof(struct symtab_s));
	symtab_stack.global_symtab = global_symtab;
	symtab_stack.local_symtab = global_symtab;
	symtab_stack.symtab_list[0] = global_symtab;
	global_symtab->level = 0;
}
/*
	this function will create a new symbol table
*/
struct symtab_s *new_symtab(int level)
{
	struct symtab_s *symtab = malloc(sizeof(struct symtab_s));
	
	if(!symtab)
	{
		fprintf(stderr, "Fatal Error: No Memory For New Symbol Tabl\n");
		exit(EXIT_FAILURE);
	}
	
	memset(symtab, 0, sizeof(struct symtab_s));
	symtab->level = level;
	return symtab;
}
/*
	the freesymtab(); function will free the 
	memory that was used by the symbol table
	along with its entries
*/
void freesymtab(struct symtab_s *symtab)
{
	if(symtab == NULL)
	{
		return;
	}
	struct symtab_entry_s *entry = symtab->first;
	
	while(entry)
	{
		if(entry->name)
		{
			free(entry->name);
		}
		if(entry->val)
		{
			free(entry->val);
		}
		if(entry->func_body)
		{
			freenodetree(entry->func_body);
		}
		struct symtab_entry_s *next = entry->next;
		free(entry);
		entry = next;
	}
	free(symtab);
}
/*
	this function prints the content of the symbol table 
*/
void dumplocalsymtab(void)
{
	struct symtab_s *symtab = symtab_stack.local_symtab;
	int i = 0;
	int indent = symtab->level *4;
	
	fprintf(stderr, "%*sSymbol table [Level %d]:\r\n",indent," ",symtab->level);
	fprintf(stderr, "%*s===========================\r\n",indent," ");
	fprintf(stderr, "%*s  No          Symbol               Val\r\n",indent," ");
	fprintf(stderr, "%*s--------------------------------------------------\r\n",indent," ");
	
	struct symtab_entry_s *entry = symtab->first;
	
	while(entry)
	{
		fprintf(stderr, "%*s[%04d] %-32s '%s'\r\n", indent, " ", i++, entry->name, entry->val);
		entry = entry->next;
		
	}
	fprintf(stderr, "%*s--------------------------------------------------\r\n", indent," ");
}
/*
	this function will add a new entry towards the
	symbol table since a key must be unique this 
	function will call upon the do_lookup(); to 
	ensure that there is one unique key for each 
	symbol
*/
struct symtab_entry_s *add_to_symtab(char *symbol)
{
	if(!symbol || symbol[0] == '\0')
	{
		return NULL;
	}
	
	struct symtab_s *st = symtab_stack.local_symtab;
	struct symtab_entry_s *entry = NULL;
	
	if((entry = do_lookup(symbol,st)))
	{
		return entry;
	}
	
	entry = malloc(sizeof(struct symtab_entry_s));
	
	if(!entry)
	{
		fprintf(stderr, "Fatal Error: No Memory For New Symbol Table Entry\n");
		exit(EXIT_FAILURE);
	}
	
	memset(entry, 0, sizeof(struct symtab_entry_s));
	entry->name = malloc(strlen(symbol)+1);
	
	if(!entry->name)
	{
		fprintf(stderr, "Fatal Error: No Memory For New Symbol Table Entry\n");
		exit(EXIT_FAILURE);
	}
	
	strcpy(entry->name, symbol);
	
	if(!st->first)
	{
		st->first = entry;
		st->last = entry;
	}
	else
	{
		st->last->next = entry;
		st->last = entry;
	}
	return entry;
}
/*
	this function frees the memory that was 
	used up by the enty and will adjust the 
	pointers to remove the entry of the symbol
	table 
*/
int rem_from_symtab(struct symtab_entry_s *entry, struct symtab_s *symtab)
{
	int res = 0;
	if(entry->val)
	{
		free(entry->val);
	}
	
	if(entry->func_body)
	{
		freenodetree(entry->func_body);
	}
	
	free(entry->name);
	
	if(symtab->first == entry)
	{
		symtab->first == symtab->first->next;
		if(symtab->last == entry)
		{
			symtab->last = NULL;
		}
		res = 1;
	}
	else
	{
		struct symtab_entry_s *e = symtab->first;
		struct symtab_entry_s *p = NULL;
		
		while(e && e != entry)
		{
			p = e;
			e = e->next;
		}
		
		if(e == entry)
		{
			p->next = entry->next;
			res = 1;
		}
	}
	free(entry);
	return res;
}
/*
	this function will search the symbol table
	with the first entry and will check to see 
	if a key matches. if it does it will return 
	the entry and if it does not it will go through
	the list till it finds the matching key if it 
	does not find it the function will return NULL
*/
struct symtab_entry_s *do_lookup(char *str, struct symtab_s *symtable)
{
	if(!str || !symtable)
	{
		return NULL;
	}
	
	struct symtab_entry_s *entry = symtable->first;
	
	while(entry)
	{
		if(strcmp(entry->name, str) == 0)
		{
			return entry;
		}
		entry = entry->next;
	}
	return NULL;
}
/*
	this function will search the full 
	stack to find the mach for the entry
	key with their given names
*/
struct symtab_entry_s *get_symtab_entry(char *str)
{
	int i = symtab_stack.symtab_count-1;
	
	do
	{
		struct symtab_s *symtab = symtab_stack.symtab_list[i];
		struct symtab_entry_s *entry = do_lookup(str, symtab);
		
		if(entry)
		{
			return entry;
		}
	}
	while(--i >= 0);
	
	return NULL;
}
/*
	this function will free the memory that 
	was used to store the old entry value
	which after will create a copy of the new 
	value which will then store it in the
	symbol table entry
*/
void symtabentrysetval(struct symtab_entry_s *entry, char *val)
{
	if(entry->val)
	{
		free(entry->val);
	}
	
	if(!val)
	{
		entry->val = NULL;
	}
	
	else
	{
		char *val2 = malloc(strlen(val)+1);
		
		if(val2)
		{
			strcpy(val2, val);
		}
		else
		{
			fprintf(stderr, "Error: No Memory For Symbol Table Entry Value\n");
		}
		entry->val = val2;
	}
}
/*
	this function add the symbol table to 
	the stack and will assign the added 
	table to the local symbol table
*/
void symtab_stack_add(struct symtab_s *symtab)
{
	symtab_stack.symtab_list[symtab_stack.symtab_count++] = symtab;
	symtab_stack.local_symtab = symtab;
}
/*
	this function will create a new symbol table 
	and push it onto the stack
*/
struct symtab_s *symtab_stack_push(void)
{
	struct symtab_s *st = new_symtab(++symtab_level);
	symtab_stack_add(st);
	return st;
}
/*
	this function will remove the symbol 
	table from the top of the stack 
*/
struct symtab_s *symtab_stack_pop(void)
{
	if(symtab_stack.symtab_count == 0)
	{
		return NULL;
	}
	
	struct symtab_s *st = symtab_stack.symtab_list[symtab_stack.symtab_count-1];
	
	symtab_stack.symtab_list[--symtab_stack.symtab_count] = NULL;
	symtab_level--;
	
	if(symtab_stack.symtab_count == 0)
	{
		symtab_stack.local_symtab = NULL;
		symtab_stack.global_symtab = NULL;
	}
	else
	{
		symtab_stack.local_symtab = symtab_stack.symtab_list[symtab_stack.symtab_count -1];
	}
	
	return st;
}
//this function returns the pointer to the local table
struct symtab_s *get_local_symtab(void)
{
	return symtab_stack.local_symtab;

}
//this function returns the pointer to the global table
struct symtab_s *get_global_symtab(void)
{
	return symtab_stack.global_symtab;
}
//this function returns the pointer to the symbol table
struct symtab_stack_s *get_symtab_stack(void)
{
	return &symtab_stack;
}