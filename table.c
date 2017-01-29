#include "table.h"

tableList* tabList;
tableNode** tableNodeCache;
hashList** hList;
int tableNode_HashIndex;

tableNode* insertDevice(tableList* list, char* Name, char* nodeName, int type, device* ptr) {
	int i, hashNum;
	bool get, preserved = false;
	tableNode *ttmp;
	device* tmp = (device*) malloc(sizeof(device));
	
	tmp->type = type; // type of the device
	tmp->ptr = (int*)ptr; // ptr to the name
	// tmp->Name = (char*)malloc(strlen(Name) + 1);
	tmp->Name = Name;
	tmp->next = NULL;
	// strcpy(tmp->Name, Name);

	hashNum = getHashIndex(nodeName, tableNode_HashIndex);

	if (type == Voltage) preserved = true;

	if (list->first != NULL) { // if the table is empty

		// check in the cache
		for (/*get = false, */i = 0 ; i < 2 ; i++) {
			if (!strcmp(tableNodeCache[i]->Name, nodeName)) {
				// get = true;
				if (tableNodeCache[i]->last == NULL) {
					tableNodeCache[i]->first = tableNodeCache[i]->last = tmp;
					tableNodeCache[i]->preserved = preserved;
				}
				else {
					tableNodeCache[i]->last->next = tmp;
					tableNodeCache[i]->preserved = (tableNodeCache[i]->preserved || preserved);
				}

				tableNodeCache[i]->last = tmp;
				return tableNodeCache[i];
			}
		}

		ttmp = getNode(hList[hashNum], nodeName);

		if(ttmp != NULL) {
			if(ttmp->last == NULL) {
				ttmp->first = ttmp->last = tmp;
				ttmp->preserved = preserved;
			}
			else {
				ttmp->last = ttmp->last->next = tmp;
				ttmp->preserved = (ttmp->preserved || preserved); 
			}
			return ttmp;
		}

		if (ttmp == NULL) {

			list->last = list->last->next = (tableNode*) malloc(sizeof(tableNode));
			list->last->first = list->last->last = tmp; // insert the device
			list->length++; // update the number of node
			list->last->Name = (char*) malloc(strlen(nodeName) + 1);
			list->last->preserved = preserved;
			list->last->next = NULL;
			
			strcpy(list->last->Name, nodeName);
			tableNodeCache[1] = tableNodeCache[0];
			tableNodeCache[0] = list->last;

			insertHashNode(hList[hashNum], list->last);
		}

	}

	else {
		list->first->first = list->first->last = tmp; // insert the device
		list->length = 1; // update the number of node
		list->first->Name = (char*) malloc(strlen(nodeName) + 1);
		list->last->preserved = preserved;
		strcpy(list->first->Name, nodeName);

		insertHashNode(hList[hashNum], list->first);
	}

	list->last->index = list->length - 2;

	return list->last;
}

void inserttableNode(tableList* list, char* nodeName) {
	tableNode* ttmp = list->first;
	int hashNum = getHashIndex(nodeName, tableNode_HashIndex);

	if (list->first != NULL) {

		for ( ; ttmp != NULL ; ttmp = ttmp->next ) {
			if (strcmp(ttmp->Name, nodeName) == 0) break;
		}
		if (ttmp == NULL) {
			list->last = list->last->next = (tableNode*) malloc(sizeof(tableNode));
			list->length += 1; // update the number of node
			list->last->Name = (char*) malloc(strlen(nodeName) + 1);
			list->last->preserved = false;
			strcpy(list->last->Name, nodeName);

			insertHashNode(hList[hashNum], list->last);
		}
	}
	else {

		list->first = list->last = (tableNode*) malloc(sizeof(tableNode));
		list->length = 1; // update the number of node
		list->first->Name = (char*) malloc(strlen(nodeName) + 1);
		list->last->preserved = false;
		strcpy(list->first->Name, nodeName);

		insertHashNode(hList[hashNum], list->first);
	}
	
	list->last->index = list->length - 2;

}
void printNodeTable(tableNode* node) {
	device* ins;

	// tableNode* node = list->first;
	printf("======================\n");
	printf("= element node table =\n");
	printf("======================\n");
	for ( ; node != NULL; ) {
		printf("%-17s", node->Name);
		for ( ins = node->first ; ins != NULL ;) {
			printf("%-17s", ins->Name);
			ins = ins->next;
		}
		printf("%-17d", node->index);
		printf("\n");
		node = node->next;
	}
	printf("\n");
}

hashList** initHash(int len) {
	int i;
	hashList** list = (hashList**) malloc(sizeof(hashList*) * len);

	for (i = 0 ; i < len ; i++) {
		list[i] = (hashList*) malloc(sizeof(hashList));
		list[i]->first = list[i]->last = NULL;
		list[i]->length = 0;
	}

	return list;
}

int getHashIndex(char* Name, int cap) {
	int i = 0, hashNum = 0, count = 1;

	for(i = strlen(Name) ; i >= 0 ; i--) {
		if(Name[i] >= 48 && Name[i] <= 57){
			hashNum += (Name[i]-48) * count;
			count *= 10;
		}
		else
			hashNum += Name[i];
	}

	// for ( ; Name[i] != '\0' ; i++) {
	// 	if(Name[i] >= 48 && Name[i] <= 57)

	// 	hashNum += (int)(Name[i]);
	// }

	hashNum %= cap;

	return hashNum;
}

void insertHashNode(hashList* list, tableNode* Node) {
	hashNode* instance = (hashNode*) malloc(sizeof(hashNode));
	instance->Node = Node;
	instance->next = NULL;

	list->length++;

	if (list->first != NULL) {
		list->last = list->last->next = instance;
	}
	else {
		list->first = list->last = instance;
	}

}

tableNode* getNode(hashList* list, char* Name) {
	hashNode* hashTmp;

	for (hashTmp = list->first ; hashTmp != NULL && strcmp(hashTmp->Node->Name, Name) ; hashTmp = hashTmp->next);

	if (hashTmp == NULL) return NULL;
	else return hashTmp->Node;
}

void printHash(hashList** list, int len) {
	int i, count;
	hashNode* hashTmp;

	for (i = 0 ; i < len ; i++) {
		printf("Hash%d: %d\n", i, list[i]->length);
		// printf("Hash%d: ", i);
		// count = 0;
		// for (hashTmp = list[i]->first ; hashTmp != NULL ; count++, hashTmp = hashTmp->next) {
		// 	printf("%s ", hashTmp->Node->Name);
		// 	if (count % 10 == 0) printf("\n");
		// }

		// printf("\n");
	}

}
