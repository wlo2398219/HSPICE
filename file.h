#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "res.h"
#include "cap.h"
#include "vol.h"
#include "ckt.h"
#include "table.h"
#include "module.h"
#include "mosset.h"
#include "mosload.h"
#include "spmatrix.h"
#include "macro_def.h"
#include "mosdefs.h"


void Initialize();
bool readFile(char*);

// table construction
void Insert(device*, char*, int);

// process of flattening
void flattenModule(); // process of flatten extra module
void Flatten(moduleNode*, int); // flatten module, 0:extra modules, 1:main ckt
void CopyDevice(device**, device**, tableNode**); // copy the device
char* appendName(char* header, char* tailer); // append two string
int path_len(moduleNode*); // hierarchy level of the module

// init function for Y and B
void initB(double*, voltageList*, int*); // Ax = b, initialize b
void updateB(double*, capacitorList*, int*, double);
void generateB(int type, int size, double h, double* arr, double* arr_c, voltageList* vlist, capacitorList* clist, int* map);
void printArray(double*, int); // print the array
void printIntArray(int*, int);

#endif
