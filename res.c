#include "res.h"
// resistorList *resList; // external variable from res.h

void insertResistor(resistorList* list, char* resName, double resResistance){ 
    resistorNode* resTmp = (resistorNode*) malloc(sizeof(resistorNode)); 
    
    // create the resistor device
    resTmp->Name = (char*)malloc(sizeof(char)*(strlen(resName)+1));
    strcpy(resTmp->Name,resName);
    resTmp->resistance = resResistance;
    resTmp->next = NULL;

    if(list->first != NULL){ // not the 1st insertion
    	list->last->next = resTmp;
    	list->last = resTmp;
    }
    else{ // the 1st insertion
        list->first = list->last = resTmp;
    }
}

void printResistorTable(resistorList* list) {
	int i, j;
	resistorNode arr[4];
	resistorNode *r = list->first;

	printf("=======================\n");
	printf("= resistor node table =\n");
	printf("=======================\n");
	if(r == NULL) 
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
		printf("%-20s", "reseff");
		for (j = 0 ; j < i ; j++) {
			printf("%-11.4lf", arr[j].resistance);
		}
		printf("\n\n");
	}
}

void printResistorNode(resistorNode* res){
	printf("Resistor name: %s\n", res->Name);
	printf("1st Node: %s, 2nd Node: %s\n",res->posNode->Name,res->negNode->Name);
	printf("Resistance: %lf\n", res->resistance);
}
