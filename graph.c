#include "graph.h"

void insertEdgeFast(vertex* a, vertex* b) {
	adjVertex* instance = (adjVertex*) malloc(sizeof(adjVertex));

	instance->v = b;
	instance->next = NULL;
	a->deg++;

	if (a->first == NULL) {
		a->first = a->last = instance;
	}
	else {
		a->last = a->last->next = instance;
	}
}

void insertEdge(vertex* a, vertex* b) {
	adjVertex* instance = (adjVertex*) malloc(sizeof(adjVertex));
	adjVertex* adjTmp;

	instance->v = b;
	instance->next = NULL;
	a->deg++;

	if (a->first == NULL) {
		a->first = a->last = instance;

	}
	else {
		if (a->first->v->v_index > b->v_index) {
			instance->next = a->first;
			a->first = instance;
		}
		else {
			for (adjTmp = a->first ; adjTmp->next != NULL && adjTmp->next->v->v_index < b->v_index ; adjTmp = adjTmp->next);
			if (adjTmp != NULL) {
				instance->next = adjTmp->next;
				adjTmp->next = instance;
			}
			else {
				a->last = a->last->next = instance;
			}
		}
	}
}

void deleteEdge(vertex* a, vertex *b) {
	adjVertex *adjTmp, *adjTmp1;

	if (a->first->v == b) {
		adjTmp = a->first;
		a->first = a->first->next;
		a->deg--;
		free(adjTmp);
	}
	else {
		for (adjTmp = a->first ; adjTmp->next != NULL && adjTmp->next->v != b ; adjTmp = adjTmp->next);
		if (adjTmp->next != NULL) {
			adjTmp1 = adjTmp->next;
			adjTmp->next = adjTmp1->next;
			a->deg--;
			free(adjTmp1);
		}
		else {
			printf("Error in deleteEdge, cannot find b\n");
		}
	}

}

void insertPairFast(vertex *a, vertex *b, vertex *c) {
	pair* instance = (pair*) malloc(sizeof(pair));
	// vertex *vTmp1 = b, *vTmp2 = c;
	vertex* vTmp;
	int tmp1 = b->v_index, tmp2 = c->v_index;

	if (tmp1 > tmp2) {
		vTmp = c;
		c = b;
		b = vTmp;
	}


	instance->a = b;
	instance->b = c;
	instance->next = NULL;

	a->fill_in++;

	if (a->pfirst == NULL) {
		a->pfirst = a->plast = instance;
	}
	else {
		a->plast = a->plast->next = instance;
	}
}

void printGraph(vertex** v, int n, bool printDeg, bool printFill) {
	int i;
	adjVertex* adjTmp;
	pair* pTmp;

	for (i = 0 ; i < n ; i++) {
		printf("vetex%d: ", v[i]->v_index);
		for (adjTmp = v[i]->first ; adjTmp != NULL ; adjTmp = adjTmp->next) {
			printf("%d ", adjTmp->v->v_index);
		}
		if (printDeg) printf(" degree = %d, ", v[i]->deg);
		if (printFill) printf("fill_in = %d", v[i]->fill_in);
		printf("\npotential fill-ins:\n");
		for (pTmp = v[i]->pfirst ; pTmp != NULL ; pTmp = pTmp->next) printf("(%d,%d) ", pTmp->a->v_index, pTmp->b->v_index);
		printf("\n");
	}
}

void deletePair(vertex *v, vertex* a, vertex* b) {
	pair *pTmp, *p = v->pfirst;
	vertex* vTmp;

	// change the order if a > b
	if (a->v_index > b->v_index) {
		vTmp = b;
		b = a;
		a = vTmp;
	}

	if (p->a == a && p->b == b) {
		pTmp = v->pfirst;
		v->pfirst = v->pfirst->next;
		v->fill_in--;
		// free(pTmp)
	}
	else {
		for ( ; p->next != NULL && !(p->next->a == a && p->next->b == b) ; p = p->next);
		// error checking
		if (p->next == NULL) printf("Error in deletePair, want to delete (%d,%d) in vertex %d\n", a->v_index, b->v_index, v->v_index);

		else {
			pTmp = p->next;
			p->next = p->next->next;
			v->fill_in--;
			// free(pTmp);
		}
	}

}

void deletePair1(vertex* v, vertex* a) {
	int i, cf, cr;
	pair* pTmp;
	pair **freeList = (pair**) malloc(sizeof(pair*) * v->fill_in);
	pair **reList = (pair**) malloc(sizeof(pair*) * v->fill_in);

	for (cf = cr = 0, pTmp = v->pfirst ; pTmp != NULL ; pTmp = pTmp->next) {
		if (pTmp->a == a || pTmp->b == a) {
			freeList[cf] = pTmp; cf++;
		}
		else {
			reList[cr] = pTmp; cr++;
		}
	}

	for (i = 0 ; i < cr - 1 ; i++) {
		reList[i]->next = reList[i + 1];
	}
	if (cr != 0) {
		v->pfirst = reList[0];
		v->plast = reList[cr - 1];
		reList[cr - 1]->next = NULL;
		v->fill_in = cr;
	}
	else {
		v->pfirst = v->plast = NULL;
		v->fill_in = 0;
	}

	for (i = 0 ; i < cf ; i++) {
		free(freeList[i]);
	}


	free(freeList);
	free(reList);
}

void insertPair(vertex *a, vertex *b, vertex *c) {
	// a: the pair is added to a
	// b and c are the elements of the pair
	int tmp1 = b->v_index, tmp2 = c->v_index;
	// vetex *vTmp1 = b, *vTmp2 = c;
	vertex* vTmp;
	pair* instance = (pair*) malloc(sizeof(pair));
	pair* pTmp;

	if (tmp1 > tmp2) {
		vTmp = c;
		c = b;
		b = vTmp;
	}

	instance->a = b;
	instance->b = c;
	instance->next = NULL;

	a->fill_in++;

	if (a->pfirst == NULL) { // if the pair list of a is NULL
		a->pfirst = a->plast = instance;
	}
	else {
		if (b->v_index < a->pfirst->a->v_index) { // if the pair is needed to be inserted in pfirst
			instance->next = a->pfirst;
			a->pfirst = instance;
		}
		else { // find the appropriate location

			for (pTmp = a->pfirst ; pTmp->next != NULL && pTmp->next->a->v_index < b->v_index ; pTmp = pTmp->next);
			if (pTmp != NULL) {
				instance->next = pTmp->next;
				pTmp->next = instance;
			}
			else {
				a->plast = a->plast->next = instance;
			}
		}
	}
}

void eliminate(vertex** v, int n, int i) {
	int tmp1, tmp2;
	pair* pTmp;
	vertex *verTmp1, *verTmp2;
	adjVertex *adjTmp1, *adjTmp2;

	// try to eliminate vertex v[i] in this graoh

	// printVertex(v[i]);

	// add the pair and update
	for (pTmp = v[i]->pfirst ; pTmp != NULL ; pTmp = pTmp->next) { // pTmp starts from the first pair of v[i]
		// printf("a = %d, b = %d\n",pTmp->a->v_index, pTmp->b->v_index );

		// printf("////////////////////////////////////////\n");
		// printVertex(pTmp->a);
		// printVertex(pTmp->b);

		// adjTmp1 starts from the adjacent vertex of a, adjTmp2 starts from the adjacent vertex of b
		for (adjTmp1 = pTmp->a->first, adjTmp2 = pTmp->b->first ; adjTmp1 != NULL && adjTmp2 != NULL ;) {

			// adjacent v of a is not connected to adjacent vertex of b
			if (adjTmp1->v->v_index < adjTmp2->v->v_index) {
				insertPair(pTmp->a, adjTmp1->v, pTmp->b);
				adjTmp1 = adjTmp1->next;
			}
			// adjacent v of b is not connected to adjacent vertex of a
			else if (adjTmp1->v->v_index > adjTmp2->v->v_index) {
				insertPair(pTmp->b, adjTmp2->v, pTmp->a);
				adjTmp2 = adjTmp2->next;
			}
			else {
				if (adjTmp1->v->v_index != v[i]->v_index) {
					// printf("Find the edge to be deleted: %d and %d\n", adjTmp1->v->v_index, adjTmp2->v->v_index );
					// printf("HI, delete (%d,%d)\n", pTmp->a->v_index, pTmp->b->v_index );
					deletePair(adjTmp1->v, pTmp->a, pTmp->b);
				}
				adjTmp1 = adjTmp1->next;
				adjTmp2 = adjTmp2->next;
			}
		}

		if (adjTmp1 != NULL) {
			for ( ; adjTmp1 != NULL ; adjTmp1 = adjTmp1->next) {
				insertPair(pTmp->a, adjTmp1->v, pTmp->b);
			}
		}
		else {
			for (; adjTmp2 != NULL ; adjTmp2 = adjTmp2->next) {
				insertPair(pTmp->b, adjTmp2->v, pTmp->a);
			}
		}


		insertEdge(pTmp->a, pTmp->b);
		insertEdge(pTmp->b, pTmp->a);
	}


	for (adjTmp1 = v[i]->first ; adjTmp1 != NULL ; adjTmp1 = adjTmp1->next) {
		deleteEdge(adjTmp1->v, v[i]);
		deletePair1(adjTmp1->v, v[i]);
	}

	v[i]->fill_in = -1;
}

void printVertex(vertex* v) {
	adjVertex *adjTmp;
	pair* pTmp;

	printf("vetex%d: ", v->v_index);
	for (adjTmp = v->first ; adjTmp != NULL ; adjTmp = adjTmp->next) {
		printf("%d ", adjTmp->v->v_index);
	}
	printf(" degree = %d, ", v->deg);
	printf("fill_in = %d", v->fill_in);
	printf("\npotential fill-ins:\n");
	for (pTmp = v->pfirst ; pTmp != NULL ; pTmp = pTmp->next) printf("(%d,%d) ", pTmp->a->v_index, pTmp->b->v_index);
	printf("\n");
}
