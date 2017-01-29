#include "file.h"
#include <time.h>

void bckEuler(int, int, double);
void Trapzoidal(int, int, double);
void gear2(int, int, double);
void generateB1();

int main(int argc, char* argv[]) {
	int i, j, size;
	clock_t  t, tt;
	double h = 0.1, duration = 1;
	double time_read, time_stamp, time_sym, time_LU;
	double time_fb, time_reorder, time_Mark;
	double *b, *b_copy;
	spElement* spTmp;
	int ODEType = GEAR2;

	tt = time_read = clock();
	tableNode_HashIndex = 21;
	if (argc != 2) {
		printf("number of arguments is wrong\n");
		return -1;
	}
	// read the file
	if (!readFile(argv[1])) {
		printf("Cannot open the file \"%s\"\n", argv[1]);
		return -1;
	}
	time_read = (double)(clock() - time_read) / CLOCKS_PER_SEC;

	printMOS1s();
	// printResistorTable(resList); // print resistor table, defined in res.h
	printVoltageTable(volList); // print voltage table, defined in vol.h
	printNodeTable(tabList->first); // print node table, defined in table.h

	t = clock();
	// printf("%d\n", capList->length);
	// printf("%d\n", tabList->length);
	// printf("%d\n", volList->length);

	size = tabList->length + volList->length + capList->length - 1; // size of the matrix
	sMat = spInit(size);
	resistorStamping(); // resistor stamping
	voltageStamping(); // voltage stamping
	if (ODEType != GEAR2)
		capacitorStamping(ODEType, h); // capacitor stamping
	else
		capacitorStamping(BCKEULER, h);

	time_stamp = (double)(clock() - t) / CLOCKS_PER_SEC;

	// printSparseMatrixFull(sMat);

	// t = clock();
	// Diagonalization(sMat); // let the diagonal elements be non-zero
	// time_reorder = (double)(clock() - t) / CLOCKS_PER_SEC;

	// t = clock();
	// MarkowitzSymbolic(sMat);
	// for (i = 0 ; i < sMat->size ; i++) { 	// assgin the diagonal element
	// 	for (spTmp = sMat->sprArr[i]; spTmp->r_index != spTmp->c_index ; spTmp = spTmp->NextInCol);
	// 	sMat->Diag[i] = spTmp;
	// }
	// time_Mark = (double)(clock() - t) / CLOCKS_PER_SEC;

	// b = (double*) malloc(sizeof(double) * size);
	// b_copy = (double*) malloc(sizeof(double) * size);
	// generateB(ODEType, size, h, b, b, volList, capList, sMat->IntToExtRowMap);
	// // initB(b, volList, sMat->IntToExtRowMap); // Ax = b, generate b
	// t = clock();
	// //MATsymFac(sMat); // symbolic factorization
	// time_sym = (double)(clock() - t) / CLOCKS_PER_SEC;

	// t = clock();
	// MATfact(sMat); // LU decomposition
	// time_LU = (double)(clock() - t) / CLOCKS_PER_SEC;

	// if (capList->first == NULL) {
	// 	// printf("x t\n");
	// 	t = clock();
	// 	fwdSubs(sMat, b); // forward substitution
	// 	bckSubs(sMat, b); // backward substitution
	// 	tt = clock() - tt;

	// 	time_fb = (double)(clock() - t) / CLOCKS_PER_SEC;
	// }

	// else {
	// 	// printf("need t\n");

	// 	// bckEuler(size, (int)(duration/h), h);
	// 	// Trapzoidal(size, (int)(duration / h), h);
	// 	gear2(size, (int)(duration / h), h);

	// 	// for (i = 0 ; i < 300 ; i++) {
	// 	// 	t = clock();
	// 	// 	fwdSubs(sMat, b); // forward substitution
	// 	// 	bckSubs(sMat, b); // backward substitution
	// 	// 	tt = clock() - tt;
	// 	// 	printf("%lf,", 0.1 * i);
	// 	// 	printArray(b, size);
	// 	// 	for (j = 0 ; j < size ; j++) b_copy[j] = b[j];
	// 	// 	generateB(size, 0.1, b, b_copy, volList, capList, sMat->IntToExtRowMap);
	// 	// 	time_fb = (double)(clock() - t) / CLOCKS_PER_SEC;
	// 	// }
	// }



	// int size1 = atoi(argv[2]) + 1; // used to print some answers to see the correctness
	// printf("%lf, %lf, %lf\n", b[sMat->IntToExtColMap[size1 - 1]], b[sMat->IntToExtColMap[size1 * (size1 - 1)]], b[sMat->IntToExtColMap[size1 * size1 - 2]]);
	// // printCapacitorTable(capList);
	// // printResistorTable(resList);
	// // time analysis
	// printf("readFile time: %lf\n", time_read);
	// printf("Init + stamping: %lf\n", time_stamp);
	// printf("reorder: %lf\n", time_reorder);
	// printf("Markowitz: %lf\n", time_Mark );
	// printf("symbolic factorization elapsed time: %lf\n", time_sym);
	// printf("LU factorization elapsed time: %lf\n", time_LU);
	// printf("solving equation time: %lf\n", time_fb);
	// printf("total matrix size = %d\n", sMat->size);
	// printf("init_size = %d\n", init_size);
	// printf("fill-in = %d\n", fill_in );
	// // printNodeTable(tabList->first);
	// printf("%lf\n", b[sMat->IntToExtColMap[0]]);
	// printf("%lf\n", b[sMat->IntToExtColMap[1]]);
	// printf("%lf\n", b[sMat->IntToExtColMap[2]]);
	// printf("%d,%d,%lf\n", init_size, fill_in,(double)tt/CLOCKS_PER_SEC );
	return 0;
}

void bckEuler(int size, int num, double h) {
	int i, j;
	double* b = (double*) malloc(sizeof(double) * size);
	double* b_copy = (double*) malloc(sizeof(double) * size);

	generateB(BCKEULER, size, h, b, b, volList, capList, sMat->IntToExtRowMap);

	for (i = 0 ; i < num ; i++) {
		fwdSubs(sMat, b); // forward substitution
		bckSubs(sMat, b); // backward substitution
		printf("%lf,", h * i);
		printArray(b, size);
		for (j = 0 ; j < size ; j++) b_copy[j] = b[j];
		generateB(BCKEULER, size, h, b, b_copy, volList, capList, sMat->IntToExtRowMap);
	}
}

void Trapzoidal(int size, int num, double h) {
	int i, j;
	double* b = (double*) malloc(sizeof(double) * size);
	double* b_copy = (double*) malloc(sizeof(double) * size);

	generateB(TRAPZOIDAL, size, h, b, b, volList, capList, sMat->IntToExtRowMap);

	for (i = 0 ; i < num ; i++) {
		fwdSubs(sMat, b); // forward substitution
		bckSubs(sMat, b); // backward substitution
		printf("%lf,", h * i);
		printArray(b, size);
		for (j = 0 ; j < size ; j++) b_copy[j] = b[j];
		generateB(TRAPZOIDAL, size, h, b, b_copy, volList, capList, sMat->IntToExtRowMap);
	}
}

void generateB1(int size, double h, double* arr_pre, double* arr, double* arr_c, voltageList* vlist, capacitorList* clist, int* map) {
	int i;
	int length = tabList->length - 1;
	int posIndex, negIndex;
	double step, vct, vcth, tmp;
	voltageNode* volTmp = vlist->first;
	capacitorNode* capTmp;

	if (arr == NULL) printf("array is not initialized in function initB\n");

	// init
	for (i = 0 ; i < size ; i++) arr[i] = 0;

	// update voltage difference
	for ( ; volTmp != NULL ; volTmp = volTmp->next, length++) {
		arr[map[length]] = volTmp->amplitude;
	}

	// stamping
	for ( capTmp = clist->first ; capTmp != NULL ; capTmp = capTmp->next, length++) {
		posIndex = capTmp->posNode->index;
		negIndex = capTmp->negNode->index;
		step = capTmp->cap / h;

		if (posIndex != -1 && negIndex != -1) {
			vct = arr_c[map[posIndex]] - arr_c[map[negIndex]];
			vcth = arr_pre[map[posIndex]] - arr_pre[map[negIndex]];

			arr[map[posIndex]] += 2.0 * step * (4.0 * vct - vcth);
			arr[map[negIndex]] -= 2.0 * step * (4.0 * vct - vcth);
			arr[length] = -0.5 * step * (4.0 * vct - vcth);

		}
		else if (posIndex != -1 && negIndex == -1) {
			vct = arr_c[map[posIndex]];
			vcth = arr_pre[map[posIndex]];

			printf("vct = %lf, vcth = %lf\n", vct,vcth);
			arr[map[posIndex]] += 2.0 * step * (4.0 * vct - vcth);
			arr[length] = -0.5 * step * (4.0 * vct - vcth);

		}
		else if (negIndex != -1 && posIndex == -1) {
			vct = -arr_c[map[negIndex]];
			vcth = -arr_pre[map[negIndex]];
			arr[map[negIndex]] -= 2.0 * step * (4.0 * vct - vcth);
			arr[length] = -0.5 * step * (4.0 * vct - vcth);

		}
	}

}

void gear2(int size, int num, double h) {
	int i, j;
	double *b_pre = (double*) calloc(size, sizeof(double));
	double *b_cur = (double*) calloc(size, sizeof(double));
	double *b_copy = (double*) calloc(size, sizeof(double));
	spElement* spTmp;

	// use bckEuler to find x(t)
	generateB(BCKEULER, size, h, b_cur, b_cur, volList, capList, sMat->IntToExtRowMap);
	fwdSubs(sMat, b_cur); // forward substitution
	bckSubs(sMat, b_cur); // backward substitution

	// printArray(b_cur, size);

	for (i = 0 ; i < size ; i++) b_pre[i] = b_cur[i];

	generateB(BCKEULER, size, h, b_cur, b_pre, volList, capList, sMat->IntToExtRowMap);
	fwdSubs(sMat, b_cur); // forward substitution
	bckSubs(sMat, b_cur); // backward substitution

	// printArray(b_cur, size);

	// construct the matrix for gear 2
	cleanMatrix(sMat);
	resistorStamping();
	// printSparseMatrixFull(sMat);
	capacitorStamping(GEAR2, h); // capacitor stamping
	// printSparseMatrixFull(sMat);

	voltageStamping();
	// printSparseMatrixFull(sMat);

	Diagonalization(sMat); // let the diagonal elements be non-zero
	// printSparseMatrixFull(sMat);

	// MarkowitzSymbolic(sMat);
	// for (i = 0 ; i < sMat->size ; i++) { 	// assgin the diagonal element
	// 	for (spTmp = sMat->sprArr[i]; spTmp->r_index != spTmp->c_index ; spTmp = spTmp->NextInCol);
	// 	sMat->Diag[i] = spTmp;
	// }
	printSparseMatrixFull(sMat);
	MATfact(sMat); // LU decomposition
	// printSparseMatrixFull(sMat);

	for (i = 0 ; i < num ; i++) {
		for (j = 0 ; j < size ; j++) b_copy[j] = b_cur[j];
		printf("\n\n");
		for (j = 0 ; j < size ; j++) printf("%lf ", b_pre[j]);
		printf("\n");
		for (j = 0 ; j < size ; j++) printf("%lf ", b_cur[j]);
		printf("\n");

		generateB1(size, h, b_pre, b_cur, b_copy, volList, capList, sMat->IntToExtRowMap);
		for (j = 0 ; j < size ; j++) printf("%lf ", b_cur[j]);
		printf("\n");
		printf("\n\n");

		fwdSubs(sMat, b_cur); // forward substitution
		bckSubs(sMat, b_cur); // backward substitution
		// printf("%lf,", h * i);
		printArray(b_cur, size);
		for (j = 0 ; j < size ; j++) b_pre[j] = b_copy[j];
	}
}