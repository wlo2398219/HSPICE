#ifndef VOL_H
#define VOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "table.h"
#include "macro_def.h"

/*
	Definition of the voltageNode
	Name: the name of the voltage source
	posNode & negNode:
	voltage sources have two connecting points
	amplitude : the unit is volt
	type: DC, AC or DC + AC
	next : point to the next voltage source
*/
typedef struct  voltageNode
{
	bool is_float;
	int type, in_index;
	double amplitude;
	char* Name;
	tableNode* posNode;
	tableNode* negNode;
	struct voltageNode *next;
} voltageNode;


typedef struct voltageList {
	int length;
	voltageNode* first;
	voltageNode* last;
} voltageList;

// global variable
extern voltageList *volList;

//function
void insertVoltage(voltageList*, char*, char*, char*, double);
void printVoltageTable(voltageList*);
void printVoltageNode(voltageNode*);

#endif

