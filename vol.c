#include "vol.h"

voltageList *volList;

void insertVoltage(voltageList* list , char* volName, char* posNode, char* negNode, double volAmp) {

	voltageNode* tmp = (voltageNode*) malloc(sizeof(voltageNode));
	tmp->Name = (char*)malloc(sizeof(char) * (strlen(volName) + 1));
	strcpy(tmp->Name, volName);
	tmp->amplitude = volAmp;

	if (list->first != NULL) {
		list->last->next = tmp;
		list->last = tmp;
		list->length++;
	}
	else {
		list->first = list->last = tmp;
		list->length = 1;
	}

	if (!strcmp(posNode, "gnd") || !strcmp(negNode, "gnd")) list->last->is_float = false;
	else list->last->is_float = true;
}

void printVoltageTable(voltageList *list) {
	int i, j;
	voltageNode arr[4];
	voltageNode* r = list->first;

	printf("=======================\n");
	printf("= voltage node table =\n");
	printf("=======================\n");
	for (; r != NULL ;) {
		for (i = 0 ; i < 4 ; i++) {
			if (r != NULL) {
				arr[i] = *r;
				r = r->next;
			}
			else break;
		}
		printf("%-20s", "voltage name");
		for (j = 0 ; j < i ; j++) {
			printf("0:%-7s", arr[j].Name);
		}
		printf("\n");
		printf("%-20s", "node1");
		for (j = 0 ; j < i ; j++) {
			printf("0:%-7s", arr[j].posNode->Name);
		}
		printf("\n");
		printf("%-20s", "node2");
		for (j = 0 ; j < i ; j++) {
			printf("0:%-7s", arr[j].negNode->Name);
		}
		printf("\n");
		printf("%-20s", "type");
		for (j = 0 ; j < i ; j++) {
			printf("%-9s", "DC");
		}
		printf("\n");
		printf("%-20s", "DC value");
		for (j = 0 ; j < i ; j++) {
			printf("%-9.4lf", arr[j].amplitude);
		}
		printf("\n");
		printf("%-20s", "Floating");
		for (j = 0 ; j < i ; j++) {
			if (arr[j].is_float)
				printf("%-9.4s", "YES");
			else
				printf("%-9.4s", "NO");
		}
		// printf("\n");
		// printf("%-20s", "internal index");
		// for (j = 0 ; j < i ; j++) {
		// 	printf("%-9.4d", arr[j].in_index);
		// }
		printf("\n\n");
	}
}

void printVoltageNode(voltageNode* vol) {
	printf("Voltage Name: %s", vol->Name);
	printf("Value: %lf", vol->amplitude);
}

