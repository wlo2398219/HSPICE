#ifndef GRAPH_H
#define GRAPH_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct adjVertex {
	struct vertex* v;
	bool fill;
	struct adjVertex* next;
} adjVertex;

typedef struct pair {
	struct vertex *a, *b;
	struct pair *next;
} pair;

typedef struct vertex {
	int v_index, deg, fill_in;
	struct adjVertex *first, *last;
	struct pair *pfirst, *plast;
} vertex;

void insertPairFast(vertex*,vertex*,vertex*);
void insertEdgeFast(vertex*,vertex*);
void printGraph(vertex**, int, bool, bool);
void printVertex(vertex*);
void eliminate(vertex**,int,int);

#endif
