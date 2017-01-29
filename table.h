#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "macro_def.h"
/*
	Definition of device
	data:
	Name: the name of the device
	next: point to the next device which is
		  also connected to the specific node
	dev_ptr: point to the real element which
			 stores the parameters of the device
*/
typedef struct device {
	char* Name;
	int type;
	int* ptr;
	struct device *next;
	struct device *dev_ptr;
} device;

/*
	Definition of the table node
	Name: the name of the node
	value: not used in current use
	first: the first device connected to this node
	last : the last one connected to this node
	next : point to the next node
*/
typedef struct tableNode {
	char* Name;
	int index;
	struct device *first;
	struct device *last;
	struct tableNode *next;
	bool preserved;
} tableNode;

typedef struct tableList
{
	int length;
	struct tableNode* first;
	struct tableNode* last;
}tableList;


// insert table node
tableNode* insertDevice(tableList*, char*, char*, int, device*); // insert the device to the node table
void inserttableNode(tableList*, char*); // create a new node 

// print the table
void printNodeTable(tableNode*); // print the node table



typedef struct hashNode {
	tableNode* Node;
	struct hashNode* next;
} hashNode;

typedef struct hashList {
	int length;
	struct hashNode *first, *last;
} hashList;

// function
hashList** initHash(int); // used to initialize hash table
int getHashIndex(char*, int); // used to get the hash number
void insertHashNode(hashList*, tableNode*); // used to insert a new hash
void printHash(hashList**, int); // used to print the hash table
tableNode* getNode(hashList*, char*); // used to search in the hash table

extern tableList* tabList;
extern tableNode** tableNodeCache;
extern hashList** hList;
extern int tableNode_HashIndex;
#endif
