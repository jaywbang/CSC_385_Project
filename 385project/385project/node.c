#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "shell.h"
#include "node.h"
#include "parser.h"

/*
	the new node function creates a new node
	which will then set its type field 
*/
struct node_s *new_node(enum node_type_e type)
{
	struct node_s *node = malloc(sizeof(struct node_s));
	
	if(!node)
	{
		return NULL;
	}
	
	memset(node, 0, sizeof(struct node_s));
	node->type = type;
	
	return node;
}
/*
	the addchildnode(); function will add a new child node
	which will incerement the root node children field
	if the root does not already have a children
	the new children will be assigned to the firstchild 
	if the root does have a children field already it will 
	be added to the end of the list 
*/
void addchildnode(struct node_s *parent, struct node_s *child)
{
	if(!parent || !child)
	{
		return;
	}
	
	if(!parent->first_child)
	{
		parent->first_child = child;
	}
	else
	{
		struct node_s *sibling = parent->first_child;
		
		while(sibling->next_sibling)
		{
			sibling - sibling->next_sibling;
		}
		sibling->next_sibling = child;
		child->prev_sibling = sibling;
	}
	parent->children++;
}
/*
	this function sets the node value to the string
	which will then copy the string to the allocated 
	memory which will then set the valtype and val.str
	field to the string. 
*/
void setnodevalstr(struct node_s *node, char *val)
{
	node->val_type = VAL_STR;
	if(!val)
	{
		node->val.str = NULL;
	}
	else
	{
		char *val2 = malloc(strlen(val)+1);
		
		if(!val2) 
		{
			node->val.str = NULL;
		}
		else
		{
			strcpy(val2, val);
			node->val.str = val2;
		}
	}
}
/*
	the freenodetree(); function will free the memory
	that is used by the node if the node has a children
	this function will be called to free all the children
*/
void freenodetree(struct node_s *node)
{
	if(!node)
	{
		return;
	}
	struct node_s *child = node->first_child;
	
	while(child)
	{
		struct node_s *next = child->next_sibling;
		freenodetree(child);
		child = next;
	}
	
	if(node->val_type == VAL_STR)
	{
		if(node->val.str)
		{
			free(node->val.str);
		}
	}
	free(node);
}
