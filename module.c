#include "module.h"

moduleList *modList;

void printModule(moduleNode *node) {
	printf("module name: %s\n", node->Name);
	printNodeTable(node->tabList->first);
	node = node->next;
}

void insertModule(char* delim, char* token) {
	moduleNode* moduleTmp;
	tableNode* tableTmp;
	bool name = true;

	if(modList->first != NULL) { // not the first insertion

		moduleTmp = (moduleNode*) malloc(sizeof(moduleNode)); // allocate to the next module
		modList->last->next = moduleTmp; // last = tmp
		moduleTmp->resList = (resistorList*) malloc(sizeof(resistorList));
		moduleTmp->volList = (voltageList*) malloc(sizeof(voltageList));
		modList->last = moduleTmp;
		modList->length++;
	}
	else { // the first insertion

		modList->first = (moduleNode*) malloc(sizeof(moduleNode)); // allocate to the first
		modList->first->resList = (resistorList*) malloc(sizeof(resistorList));
		modList->first->volList = (voltageList*) malloc(sizeof(voltageList));
		modList->last = modList->first;
		modList->length++;
	}
	
	while ( (token = strtok(NULL, delim)) != NULL ) {

		if (!name) { 
			if (modList->last->tabList == NULL) {
				modList->last->tabList = (tableList*)malloc(sizeof(tableList));
				modList->last->tabList->last = modList->last->tabList->first = (tableNode*)malloc(sizeof(tableNode));
				modList->last->tabList->first->Name = (char*)malloc(sizeof(token));
				strcpy(modList->last->tabList->first->Name, token);
			}
			else {
				tableTmp = (tableNode*) malloc(sizeof(tableNode));
				tableTmp->Name = (char*) malloc(sizeof(token));
				strcpy(tableTmp->Name, token);
				modList->last->tabList->last->next = tableTmp;
				modList->last->tabList->last = tableTmp;
			}
		}
		else { // read the name of the module
			modList->last->Name = (char*) malloc(sizeof(token));
			strcpy(modList->last->Name, token);
			name = false;
		}
	}
}
