#ifndef RES_H
#define RES_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"

/*
	Definition of the resistorNode
	Name: the name of the resistor
	firstNode & negNode:
	resistors have two connecting points
	resistance: the unit is ohm
	next : point to the next resistor
*/
typedef struct  resistorNode {
	char* Name;
	tableNode* posNode;
	tableNode* negNode;
	double resistance;
	struct resistorNode *next;
} resistorNode;

/*
	Definition of the resistorList
	length: total number of resistors
	*first: head of the linked list
	*last: tail of the linked list
*/
typedef struct resistorList {
	int length;
	resistorNode* first;
	resistorNode* last;
}resistorList;

// global variable
resistorList *resList;

void insertResistor(resistorList*,char*,double); // insert resistor to resistor table
void printResistorTable(resistorList*); // print the resistor table
void printResistorNode(resistorNode*); // print a single resistor
 
#endif
