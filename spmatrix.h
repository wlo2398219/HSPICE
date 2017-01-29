#ifndef SPMATRIX_H
#define SPMATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "res.h"
#include "vol.h"
#include "cap.h"
#include "table.h"
#include "graph.h"

// MACRO
#define INROW 0
#define INCOL 1

// element in the sparse matrix
typedef struct spElement
{
	int r_index;
	int c_index;
	double r_value;
	struct spElement* NextInRow;
	struct spElement* NextInCol;
} spElement;

// sparse matrix
typedef struct sMatrix
{
	int size; // size x size square Matrix
	spElement** Diag; // Diagonal elements
	spElement** sprArr; // first element in every row
	spElement** spcArr; // first element in every column
	int *ExtToIntColMap; // ExtToIntColMap[c_index_in_Ext] = c_index_in_Int
	int *ExtToIntRowMap; // ExtToIntRowMap[r_index_in_Ext] = r_index_in_Int 
	int *IntToExtColMap; // IntToExtColMap[c_index_in_Int] = c_index_in_Ext
	int *IntToExtRowMap; // IntToExtColMap[r_index_in_Int] = r_index_in_Ext
} Matrix;

// global variable
extern Matrix* sMat;
extern int fill_in;
extern int init_size;

// function for sparse matrix
Matrix* spInit(int); // initialize
void transpose(Matrix*); // transpose 
// void swapRow(Matrix*, int, int); // swap two rows
void swapColOnce(Matrix*, int*);
void swapRowOnce(Matrix*, int*);
// void swapCol(Matrix*, int, int); // swap two columns
spElement* getME(Matrix*, int, int); // access the node, if not existed, create it

// function for ckt analysis
void Diagonalization(Matrix*); // let the diagonal elements be non-zero
void cleanMatrix(Matrix*);
void Reorder(Matrix*,int*); // reorder the matrix to reduce fill-ins
void resistorStamping(); // stamping method for resistor
void voltageStamping(); // stamping method for voltage 	
void capacitorStamping(int, double); // stamping method for capacitor
void m_fill_in_algorithm(Matrix*, int*);
void MinFillin(Matrix*,int*);
void MarkowitzSymbolic(Matrix*);
void Markowitz(Matrix*); // reordered based on Markowitz number
void MATsymFac(Matrix*); // symbolic factorization
void MATfact(Matrix*); // LU decomposition
void fwdSubs(Matrix*, double*); // forward substitution
void bckSubs(Matrix*, double*); // backward substitution

// printFunction
void printOneLine(spElement*, int);
void printspElement(spElement*); // used to debug
void printSparseMatrix(Matrix*, int); // print matrix in column order or row order
void printSparseMatrixFull(Matrix*); // print matrix in normal display

#endif