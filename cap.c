#include "cap.h"

capacitorList *capList;

void insertCapacitor(capacitorList* list, char* capName , double cap) {
	capacitorNode* capTmp = (capacitorNode*) malloc(sizeof(capacitorNode));

	capTmp->Name = (char*) malloc(sizeof(char) * (strlen(capName) + 1));
	strcpy(capTmp->Name, capName);
	capTmp->cap = cap;
	capTmp->next = NULL;
	capTmp->Ic = 0;

	if (list->first != NULL) {
		list->last = list->last->next = capTmp;
	}
	else {
		list->last = list->first = capTmp;
	}

	list->length++;
}

void printCapacitorTable(capacitorList* list) {
	int i, j;
	capacitorNode arr[4];
	capacitorNode *r = list->first;

	printf("=======================\n");
	printf("= resistor node table =\n");
	printf("=======================\n");
	if (r == NULL)
		printf("NULL POINTER\n");
	for (; r != NULL ;) {
		for (i = 0 ; i < 4; i++) {
			if (r != NULL) {
				arr[i] = *r;
				r = r->next;
			}
			else break;
		}
		printf("%-20s", "element name");
		for (j = 0 ; j < i ; j++) {
			printf("0:%-9s", arr[j].Name);
		}
		printf("\n");
		printf("%-20s", "node1");
		for (j = 0 ; j < i ; j++) {
			printf("0:%-9s", arr[j].posNode->Name);
		}
		printf("\n");
		printf("%-20s", "node2");
		for (j = 0 ; j < i ; j++) {
			printf("0:%-9s", arr[j].negNode->Name);
		}
		printf("\n");
		printf("%-20s", "capeff");
		for (j = 0 ; j < i ; j++) {
			printf("%-11.4lf", arr[j].cap);
		}
		printf("\n\n");
	}
}

void printCapacitorNode(capacitorNode* cap) {
	printf("Capacitor name: %s\n", cap->Name);
	printf("1st Node: %s, 2nd Node: %s\n", cap->posNode->Name, cap->negNode->Name);
	printf("capacitancem: %lf\n", cap->cap);
}
