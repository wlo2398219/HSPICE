#include "hash.h"

int getHash(char* Name, int cap) {
	int i = 0, hashNum = 0;

	for ( ; Name[i] != '\0' ; i++) {
		hashNum += (int)(Name[i]);
	}

	hashNum %= cap;

	return hashNum;
}

void insertHashNode(hashList* list, char* Name) {
	hashNode* instance = (hashNode*) malloc(sizeof(hashNode));
	instance->Name = (char*) malloc(sizeof(char) * (strlen(Name) + 1));
	strcpy(instance->Name, Name);

	if(list->first != NULL) {
		list->first = list->last = instance;
	}
	else {
		list->last = list->last->next = instance;
	}

}