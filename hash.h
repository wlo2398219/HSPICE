#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"

typedef struct hashNode {
	char* Name;
	struct hashNode* next;
} hashNode;

typedef struct hashList {
	struct hashNode *first, *last;
} hashList;

int getHash(char*, int);
void insertHashNode(hashList*, char*);

#endif