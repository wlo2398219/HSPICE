#ifndef CAP_H
#define CAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"

typedef struct capacitorNode {	
	char* Name;
	tableNode* posNode;
	tableNode* negNode;
	double cap;
	double Ic;
	struct capacitorNode* next;
}capacitorNode;


typedef struct capacitorList {
	int length;
	capacitorNode* first;
	capacitorNode* last;
}capacitorList;

// global variable
extern capacitorList *capList;

void insertCapacitor(capacitorList*, char*, double);
void printCapacitorTable(capacitorList*);
void printCapacitorNode(capacitorNode*);

#endif
