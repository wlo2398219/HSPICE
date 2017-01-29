#include "file.h"

hashList** hList;

bool readFile(char* fileName) {
	int i, len_tmp;
	char *token, oneLine[40];
	char *Name, *posNode, *negNode;
	char delim[3];
	double resistance, amplitude;
	int state = Main;
	FILE* fp;

	resistorNode *rftmp, *rltmp, *resTmp;
	capacitorNode *cftmp, *cltmp, *capTmp;
	voltageNode *vftmp, *vltmp, *volTmp;
	tableNode *tftmp, *tltmp, *tableTmp;
	subcktNode *subcktTmp, *sftmp, *sltmp;
	moduleNode *moduleTmp;
	device *device_tmp;

	fp = fopen(fileName, "r");
	if (fp == NULL) // error checking
		return false;

	// get rid of the header
	token = fgets(oneLine, 40, fp);
	token = fgets(oneLine, 40, fp);

	Initialize(); // initialize the list
	strcpy(delim, " \n"); // set delimiter
	// Build the module
	while ((token = fgets(oneLine, 40, fp)) != NULL) {
		while (1) { // skip "\n"
			if (!strcmp(oneLine, "\n"))
				token = fgets(oneLine, 40 , fp);
			else
				break;
		}

		if (state == ToModule) { // time to read the module
			// store the result in main process
			rftmp = resList->first; rltmp = resList->last;
			vftmp = volList->first; vltmp = volList->last;
			tftmp = tabList->first; tltmp = tabList->last;
			sftmp = scktList->first; sltmp = scktList->last;
			len_tmp = tabList->length;

			resList->first = NULL; volList->first = NULL;
			scktList->first = NULL;

			tabList->first = modList->last->tabList->first;
			tabList->last = modList->last->tabList->last;
			tabList->length = 0;
			state = Module; // change the state to moduling

		}
		else if (state == ToMain) { // time to back to the main ckt
			// get the data from xtmp
			resList->first = rftmp; resList->last = rltmp;
			volList->first = vftmp; volList->last = vltmp;
			tabList->first = tftmp; tabList->last = tltmp;
			scktList->first = sftmp; scktList->last = sltmp;
			tabList->length = len_tmp;
			state = Main;
		}

		token = strtok(oneLine, delim); // identify the type. Ex: resistor, voltage, module ...

		if (token[0] == RESISTOR) {
			// get the information
			Name = token;
			posNode = strtok(NULL, delim);
			negNode = strtok(NULL, delim);
			resistance = atof(strtok(NULL, delim));

			insertResistor(resList, Name, resistance); // insert to resistor table
			resList->last->posNode = insertDevice(tabList, resList->last->Name, posNode, Resistor, (device*)resList->last);
			resList->last->negNode = insertDevice(tabList, resList->last->Name, negNode, Resistor, (device*)resList->last);
		}
		else if (token[0] == VOLTAGE) {
			// get the information
			Name = token;
			posNode = strtok(NULL, delim);
			negNode = strtok(NULL, delim);
			amplitude = atof(strtok(NULL, delim));

			insertVoltage(volList, Name, posNode, negNode, amplitude); // insert to voltage table
			volList->last->posNode = insertDevice(tabList, volList->last->Name, posNode, Voltage, (device*)volList->last);
			volList->last->negNode = insertDevice(tabList, volList->last->Name, negNode, Voltage, (device*)volList->last);

		}
		else if (token[0] == CAPACITOR) {
			Name = token;
			posNode = strtok(NULL, delim);
			negNode = strtok(NULL, delim);
			amplitude = atof(strtok(NULL, delim));

			insertCapacitor(capList, Name, amplitude);
			capList->last->posNode = insertDevice(tabList, capList->last->Name, posNode, Capacitor, (device*)volList->last);
			capList->last->negNode = insertDevice(tabList, capList->last->Name, negNode, Capacitor, (device*)volList->last);
		}
		else if (token[0] == MOS) {
			insertMOS(mos1Models,delim,token);
		}
		else if (token[0] == SUBCKT) {

			if (scktList->first != NULL) {
				subcktTmp = (subcktNode*) malloc(sizeof(subcktNode));
				subcktTmp->first = (cktNode*) malloc(sizeof(cktNode));
				subcktTmp->next = NULL;
				subcktTmp->num_node = 0;

				subcktTmp->Name = (char*) malloc(sizeof(token));
				strcpy(subcktTmp->Name, token);
				scktList->last->next = subcktTmp;
				scktList->last = subcktTmp;

				readSubckt(tabList, scktList->last, delim, token);
			}
			else {
				scktList->first = (subcktNode*) malloc(sizeof(subcktNode)); // allocate the memory
				scktList->first->first = (cktNode*) malloc(sizeof(cktNode));
				scktList->first->num_node = 0;

				scktList->first->Name = (char*) malloc(sizeof(token)); // copy the name to the subckt
				strcpy(scktList->first->Name, token);

				scktList->last = scktList->first;
				scktList->first->next = NULL;

				readSubckt(tabList, scktList->first, delim, token);

			}
		}
		else if (token[0] == INFO) {  // INFO == .
			if (strcmp(token, ".subckt") == 0) {
				// create module
				insertModule(delim, token);
				state = ToModule;
			}
			else if (strcmp(token, ".end") == 0) {
				if (state == Module) {
					state = ToMain;
					modList->last->subckt = scktList->first;
					modList->last->tabList->first = tabList->first;
					modList->last->tabList->last = tabList->last;
					modList->last->tabList->length = len_tmp;
					modList->last->resList->first = resList->first;
					modList->last->resList->last = resList->last;
					modList->last->volList->first = volList->first;
					modList->last->volList->last = volList->last;
				}
			}

		}
	}
	if (state == ToMain) {

		resList->first = rftmp; resList->last = rltmp;
		volList->first = vftmp; volList->last = vltmp;
		tabList->first = tftmp; tabList->last = tltmp;
		scktList->first = sftmp; scktList->last = sltmp;
		tabList->length = len_tmp;
	}

	if (modList->last != NULL) {
		flattenModule();
		Flatten(modList->first, 1);
	}


	// free the hash table
	hashNode *hCur, *hNext;

	for (i = 0 ; i < tableNode_HashIndex ; i++) {
		if (hList[i]->first != NULL) {
			hCur = hList[i]->first;
			hNext = hCur->next;
			for ( ; hCur != NULL && hNext != NULL ; hCur = hNext , hNext = hNext->next ) free(hCur);
			free(hCur);
		}
		free(hList[i]);
	}
	free(hList);

	return true;
}

void Initialize() {
	char *gnd = (char*) malloc(sizeof(char) * (4));
	strcpy(gnd, "gnd");
	resList = (resistorList*) malloc(sizeof(resistorList));
	capList = (capacitorList*) malloc(sizeof(capacitorList));
	volList = (voltageList*) malloc(sizeof(voltageList));
	scktList = (subcktList*) malloc(sizeof(subcktList));
	tabList = (tableList*) malloc(sizeof(tableList));
	modList = (moduleList*) malloc(sizeof(moduleList));
	mos1Models = (mos1Model*) malloc(sizeof(mos1Model));

	resList->length = capList->length = volList->length = 0;
	modList->length = scktList->length = tabList->length = 0;

	hList = initHash(tableNode_HashIndex); // build hash table

	inserttableNode(tabList, gnd); // insert "gnd" to the node table
	tableNodeCache = (tableNode**) malloc(sizeof(tableNode*) * 2);
	tableNodeCache[0] = tableNodeCache[1] = tabList->first;
	tabList->first->index = -1; // gnd is assigned to -1


}

char* appendName(char* header, char* tailer) {
	char* Name = (char*) malloc(sizeof(char) * (strlen(header) + strlen(tailer) + 1));

	strcat(Name, header);
	strcat(Name, ".");
	strcat(Name, tailer);
	return Name;
}

void Flatten(moduleNode *node, int type) {
	int i, a, b;
	int devType;
	char a1[50][50], b1[50][50];
	char *nameTmp, *nodeNameTmp;
	char *posNode, *negNode;
	subcktNode* stmp;
	resistorList* resListToInsert;
	voltageList* volListToInsert;
	tableList* tableToInsert;
	tableNode* tableTmp;
	cktNode* cktTmp;
	moduleNode *moduleTmp;

	if (type == 0) {
		stmp = node->subckt; 	// get the subckt of the module
		tableToInsert = node->tabList;
		resListToInsert = node->resList;
		volListToInsert = node->volList;
	}
	else {
		stmp = scktList->first;
		tableToInsert = tabList;
		resListToInsert = resList;
		volListToInsert = volList;
	}
	for ( ; stmp != NULL ; stmp = stmp->next ) {
		// find the corresponding module
		for (moduleTmp = modList->first ; moduleTmp != NULL ; ) {
			if (strcmp(stmp->moduleName, moduleTmp->Name) == 0) break;
			else	moduleTmp = moduleTmp->next;
		}

		for (i = 0, cktTmp = stmp->first, tableTmp = moduleTmp->tabList->first ; i < stmp->num_node ; cktTmp = cktTmp->next, tableTmp = tableTmp->next, i++) {
			strcpy(a1[i], tableTmp->Name);
			strcpy(b1[i], cktTmp->Name);
			// printf("%s %s\n",a1[i],b1[i] );
		}

		resistorNode* resTmp = moduleTmp->resList->first;
		for ( ; resTmp != NULL ; resTmp = resTmp -> next ) {
			a = b = stmp->num_node;
			for ( i = 0, tableTmp = moduleTmp->tabList->first ;   i < stmp->num_node ; i++, tableTmp = tableTmp->next ) {
				if (!strcmp(resTmp->posNode->Name, tableTmp->Name)) a = i;
				if (!strcmp(resTmp->negNode->Name, tableTmp->Name)) b = i;
			}

			nameTmp = appendName(stmp->Name, resTmp->Name);
			if (a != stmp->num_node)	posNode = b1[a];
			else posNode = appendName(stmp->Name, resTmp->posNode->Name);
			if (b != stmp->num_node) negNode = b1[b];
			else negNode = appendName(stmp->Name, resTmp->negNode->Name);

			insertResistor(resListToInsert, nameTmp, resTmp->resistance);

			resListToInsert->last->posNode = insertDevice(tableToInsert, nameTmp, posNode, Resistor, (device*)resTmp);
			resListToInsert->last->negNode = insertDevice(tableToInsert, nameTmp, negNode, Resistor, (device*)resTmp);

		}

		voltageNode* volTmp = moduleTmp->volList->first;
		for ( ; volTmp != NULL ; volTmp = volTmp -> next ) {
			a = b = stmp->num_node;
			for ( i = 0, tableTmp = moduleTmp->tabList->first ;   i < stmp->num_node ; i++, tableTmp = tableTmp->next ) {
				if (!strcmp(volTmp->posNode->Name, tableTmp->Name)) a = i;
				if (!strcmp(volTmp->negNode->Name, tableTmp->Name)) b = i;
			}

			nameTmp = appendName(stmp->Name, volTmp->Name);
			if (a != stmp->num_node)	posNode = b1[a];
			else posNode = appendName(stmp->Name, volTmp->posNode->Name);
			if (b != stmp->num_node) negNode = b1[b];
			else negNode = appendName(stmp->Name, volTmp->negNode->Name);

			insertVoltage(volListToInsert, nameTmp, posNode, negNode, volTmp->amplitude);

			volListToInsert->last->posNode = insertDevice(tableToInsert, nameTmp, posNode, Voltage, (device*)volTmp);
			volListToInsert->last->negNode = insertDevice(tableToInsert, nameTmp, negNode, Voltage, (device*)volTmp);

		}
	}
}

void flattenModule() {
	subcktNode* subcktTmp;
	moduleNode *moduleTmp;
	int m_len = modList->length; // number of the modules
	int i, count, length , max = 0;
	int *track = (int*) malloc(sizeof(int) * m_len);

	// get the hierarchy level of each element
	for (i = 0, moduleTmp = modList->first; i < m_len ; i++) {
		track[i] = path_len(moduleTmp);
		if (max < track[i]) max = track[i];
		moduleTmp = moduleTmp->next;
		printf("track = %d\n", track[i] );
	}
	printf("length = %d\n", m_len );

	for (count = 2 ; count <= max ; count++) {
		for (i = 0, moduleTmp = modList->first ; i < m_len ; i++) {
			if (track[i] == count) {
				Flatten(moduleTmp, 0);
				printModule(moduleTmp);
			}
			moduleTmp = moduleTmp->next;
		}
	}
}

int path_len(moduleNode* m) {
	subcktNode *subcktTmp;
	moduleNode *moduleTmp;

	if (m->subckt == NULL) return 1;
	else {
		int max = 0, tmp;
		subcktNode *stmp = m->subckt;
		for ( ; stmp != NULL ; ) {

			for (moduleTmp = modList->first ; moduleTmp != NULL ; ) {
				if (strcmp(stmp->moduleName, moduleTmp->Name) == 0) {
					break;
				}
				else
					moduleTmp = moduleTmp->next;
			}
			tmp = 1 + path_len(moduleTmp);
			if (tmp > max) max = tmp;
			stmp = stmp->next;
		}
		return max;
	}
}

void initB(double* arr, voltageList* list, int* map) {
	int length = tabList->length - 1;

	voltageNode* volTmp = list->first;
	if (arr == NULL) printf("array is not initialized in function initB\n");


	for ( ; volTmp != NULL ; volTmp = volTmp->next, length++) {
		arr[map[length]] = volTmp->amplitude;
	}
}

void updateB(double* arr, capacitorList* list, int* map, double h) {

	capacitorNode* capTmp = list->first;

	if (arr == NULL) printf("array is not initialized in function updateB\n");

	for ( ; capTmp != NULL ; capTmp = capTmp->next ) {

	}
}

void generateB(int type, int size, double h, double* arr, double* arr_c, voltageList* vlist, capacitorList* clist, int* map) {
	int i;
	int length = tabList->length - 1;
	int posIndex, negIndex;
	double step;
	voltageNode* volTmp = vlist->first;
	capacitorNode* capTmp;

	if (arr == NULL) printf("array is not initialized in function initB\n");

	for (i = 0 ; i < size ; i++) arr[i] = 0;

	for ( ; volTmp != NULL ; volTmp = volTmp->next, length++) {
		arr[map[length]] = volTmp->amplitude;
	}

	if (type == TRAPZOIDAL) h /= 2.0;

	for ( capTmp = clist->first ; capTmp != NULL ; capTmp = capTmp->next, length++) {
		posIndex = capTmp->posNode->index;
		negIndex = capTmp->negNode->index;
		step = capTmp->cap / h;

		if (posIndex != -1 && negIndex != -1) {
			arr[map[posIndex]] += step * (arr_c[map[posIndex]] - arr_c[map[negIndex]]);
			arr[map[negIndex]] += step * (arr_c[map[negIndex]] - arr_c[map[posIndex]]);
			if (type == TRAPZOIDAL) {
				arr[map[posIndex]] += arr_c[length];
				arr[map[negIndex]] += arr_c[length];
				arr[length] = step * (arr_c[map[negIndex]] - arr_c[map[posIndex]]) - arr_c[length];
			}
			else {
				arr[length] = step * (arr_c[map[negIndex]] - arr_c[map[posIndex]]);
			}
		}
		else if (posIndex != -1 && negIndex == -1) {
			arr[map[posIndex]] += step * (arr_c[map[posIndex]]);
			if (type == TRAPZOIDAL) {
				arr[map[posIndex]] += arr_c[length];
				arr[length] = step * (-arr_c[map[posIndex]]) - arr_c[length];
			}
			else {
				arr[length] = step * (-arr_c[map[posIndex]]);
			}
		}
		else if (negIndex != -1 && posIndex == -1) {
			arr[map[negIndex]] += step * (arr_c[map[negIndex]]);
			if (type == TRAPZOIDAL) {
				arr[map[negIndex]] += arr_c[length];
				arr[length] = step * (arr_c[map[negIndex]]) - arr_c[length];
			}
			else {
				arr[length] = step * (arr_c[map[negIndex]]);
			}
		}
	}

	// update the current

	// if (type == TRAPZOIDAL) h *= 2.0;

	// for ( capTmp = clist->first ; capTmp != NULL ; capTmp = capTmp->next) {
	// 	posIndex = capTmp->posNode->index;
	// 	negIndex = capTmp->negNode->index;
	// 	step = capTmp->cap / h;

	// 	if (posIndex != -1 && negIndex != -1) {
	// 		capTmp->Ic = step * (arr_c[map[posIndex]] - arr_c[map[negIndex]]);
	// 	}
	// 	else if (posIndex != -1 && negIndex == -1) {
	// 		capTmp->Ic = step * (arr_c[map[posIndex]]);
	// 	}
	// 	else if (negIndex != -1 && posIndex == -1) {
	// 		capTmp->Ic = step * (-arr_c[map[negIndex]]);
	// 	}
	// }

}

void printArray(double* arr, int size) {
	int i, j, count, gap = 10;
	if (arr == NULL) printf("array is not initialized in function printArray\n");
	for (i = 0, count = 0 ; i < size ; count += gap ) {
		for (j = 0; j < gap && i < size ; j++, i++) {
			printf("%lf, ", arr[i]);
		}
		printf("\n");
	}
}

int addVolInternal(int table_len, voltageNode* node) {
	int len = 0;
	voltageNode* volTmp = node;

	for ( ; volTmp != NULL ; volTmp = volTmp->next ) {
		if (volTmp->is_float) { // floating voltage should be added with a internal node and allowed to resistor stamping
			volTmp->in_index = len + table_len;
			len++;
			volTmp->posNode->preserved = false;
			volTmp->negNode->preserved = false;
		}
		else {
			volTmp->in_index = 0;
		}
	}

	return len;
}

void printIntArray(int* arr, int size) {
	int i, j;
	for (i = 0 ; i < size; ) {
		for (j = 0 ; j < 10 && i < size; i++, j++) {
			printf("%d ", arr[i]);
		}
		printf("\n");
	}
}
