#include "ckt.h"

// external variable
subcktList *scktList;

void readSubckt(tableList* list, subcktNode* node, char* delim, char* token) {
	bool first = true;
	cktNode *cktTmp, *tmp;
	char str_tmp[50][50];
	int i = 0, j;

	while ((token = strtok(NULL, delim)) != NULL) {
		if (first) { // read the name of the subckt
			node->first->Name = (char*) malloc(sizeof(token));
			strcpy(node->first->Name, token);
			strcpy(str_tmp[i++], token);
			node->last = node->first;
			node->first->next = NULL;
			node->num_node++;
			first = false;
		}
		else { // read the node
			cktTmp = (cktNode*) malloc(sizeof(cktNode));
			cktTmp->Name = (char*) malloc(sizeof(token));
			cktTmp->next = NULL;
			strcpy(cktTmp->Name, token);
			strcpy(str_tmp[i++], token);
			tmp = node->last; // used to store the last 2 nodes
			node->last->next = cktTmp;
			node->last = cktTmp;
			node->num_node++;
		}
	}

	if (node->num_node < 2) { // error checking
		printf("error, the minimun argument of subckt is 2\n");
	}
	else { 	// the last one is the name of the module
		node->num_node--;
		node->moduleName = (char*) malloc(sizeof(node->last->Name));
		strcpy(node->moduleName, node->last->Name);
		free(node->last);
		node->last = tmp;
		node->last->next = NULL;
	}

	i--;
	for(j = 0 ; j < i ; j++){
		inserttableNode(list,str_tmp[j]);
	}

}

void printSubckt(subcktNode * subcktTmp) {
	cktNode *cktTmp;

	for ( ; subcktTmp != NULL ; ) {
		printf("subckt name: %s\nmodule name: %s\nNodes: ", subcktTmp->Name, subcktTmp->moduleName);
		for (cktTmp = subcktTmp->first ; cktTmp != NULL ; ) {
			printf("%s ", cktTmp->Name);
			cktTmp = cktTmp->next;
		}
		printf(" number of nodes: %d\n", subcktTmp->num_node);
		subcktTmp = subcktTmp->next;
	}
}

