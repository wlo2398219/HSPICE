#ifndef CKT_H
#define CKT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "table.h"

/*
	cktNode is used to store the name of node
	this structure is used to build the
	cktNode list for each device
*/
typedef struct cktNode
{
	char* Name;
	struct cktNode *next;
} cktNode;

/*
	Definition of the subckt
	Name: the name of the subckt, such as X2
	moduleName: corresponding module
	num_node: number of nodes in subckt
	first: head of the list
	last: tail of the list
	next: next subckt
*/
typedef struct subcktNode
{
	char* Name; // name in the circuit
	char* moduleName; // name of the module
	int num_node; 
	struct cktNode* first;
	struct cktNode* last; 
	struct subcktNode* next;
} subcktNode;

/*
	length: number of subckts
	first: head of the list
	last: tail of the lists
*/
typedef struct subcktList
{
	int length;
	struct subcktNode* first;
	struct subcktNode* last;
} subcktList;

// global variable
extern subcktList *scktList;

void readSubckt(tableList* list, subcktNode* node, char* delim, char* token); // read the subcircuit
void printSubckt(subcktNode*); // print the subcircuit

#endif
