#include "spmatrix.h"

// external variables
Matrix* sMat;
int fill_in, init_size;

Matrix* spInit(int len) {
    int i;

    // allocate memories
    Matrix *Mat = (Matrix*) malloc(sizeof(Matrix));
    Mat->size = len;
    Mat->sprArr = (spElement**) malloc(sizeof(spElement*)*len);
    Mat->spcArr = (spElement**) malloc(sizeof(spElement*)*len);
    Mat->Diag = (spElement**) malloc(sizeof(spElement*)*len);
    Mat->ExtToIntColMap = (int*) malloc(sizeof(int) * len);
    Mat->ExtToIntRowMap = (int*) malloc(sizeof(int) * len);
    Mat->IntToExtColMap = (int*) malloc(sizeof(int) * len);
    Mat->IntToExtRowMap = (int*) malloc(sizeof(int) * len);

    // initialization
    fill_in = init_size = 0;
    for (i = 0 ; i < len ; i++) {
        Mat->sprArr[i] = Mat->spcArr[i] = Mat->Diag[i] = NULL;
        Mat->ExtToIntRowMap[i] = Mat->ExtToIntColMap[i] = Mat->IntToExtRowMap[i] = Mat->IntToExtColMap[i] = i;
    }

    return Mat;
}

spElement* getME(Matrix* mat, int r_index, int c_index) {
    int i;
    int len = mat->size;
    bool updated = false;
    spElement *spElementInstance, *spElementPre, *spElementNext;

    if (r_index >= len || c_index >= len || r_index < 0 || c_index < 0)
        printf("Invalid index in getME: r_index = %d, c_index = %d, largest = %d\n", r_index, c_index, len - 1);

    if (mat->sprArr[r_index] == NULL) {
        spElementInstance = (spElement*) malloc(sizeof(spElement)); // create the element and initialize
        spElementInstance->r_index = r_index;
        spElementInstance->c_index = c_index;
        spElementInstance->r_value = 0;
        spElementInstance->NextInCol = spElementInstance->NextInRow = NULL;
        mat->sprArr[r_index] = spElementInstance;
        init_size++;
    }
    else {
        if (mat->sprArr[r_index]->c_index > c_index) {
            spElementInstance = (spElement*) malloc(sizeof(spElement)); // create the element and initialize
            spElementInstance->r_index = r_index;
            spElementInstance->c_index = c_index;
            spElementInstance->r_value = 0;
            spElementInstance->NextInCol = spElementInstance->NextInRow = NULL;
            spElementNext = mat->sprArr[r_index];
            mat->sprArr[r_index] = spElementInstance;
            mat->sprArr[r_index]->NextInCol = spElementNext;
            init_size++;
        }
        else {
            spElementPre = mat->sprArr[r_index];

            // if (r_index > c_index)   spElementPre = mat->sprArr[r_index];
            // else spElementPre = mat->Diag[r_index];
            spElementNext = spElementPre->NextInCol;


            for ( ; spElementNext != NULL ; spElementPre = spElementNext , spElementNext = spElementNext->NextInCol) {
                if (spElementNext->c_index >= c_index || spElementPre->c_index == c_index) break;
            }
            if (spElementPre->c_index == c_index) { // existed node, can directly return
                return spElementPre;
            }
            else if (spElementNext == NULL) {
                spElementInstance = (spElement*) malloc(sizeof(spElement)); // create the element and initialize
                spElementInstance->r_index = r_index;
                spElementInstance->c_index = c_index;
                spElementInstance->r_value = 0;
                spElementInstance->NextInCol = spElementInstance->NextInRow = NULL;
                spElementPre->NextInCol = spElementInstance;
                init_size++;
            }
            else if (spElementNext->c_index == c_index) { // existed node, can directly return
                // free(spElementInstance);
                return spElementNext;
            }
            else {
                spElementInstance = (spElement*) malloc(sizeof(spElement)); // create the element and initialize
                spElementInstance->r_index = r_index;
                spElementInstance->c_index = c_index;
                spElementInstance->r_value = 0;
                spElementInstance->NextInCol = spElementInstance->NextInRow = NULL;
                spElementPre->NextInCol = spElementInstance;
                spElementInstance->NextInCol = spElementNext;
                init_size++;
            }

        }
    }

    if (mat->spcArr[c_index] == NULL) {
        mat->spcArr[c_index] = spElementInstance;
    }
    else if (mat->spcArr[c_index] != NULL) {
        if (mat->spcArr[c_index]->r_index > r_index) {
            spElementNext = mat->spcArr[c_index];
            mat->spcArr[c_index] = spElementInstance;
            mat->spcArr[c_index]->NextInRow = spElementNext;
        }
        else {
            spElementPre = mat->spcArr[c_index];
            // if (c_index > r_index)   spElementPre = mat->spcArr[c_index];
            // else spElementPre = mat->Diag[c_index];

            spElementNext = spElementPre->NextInRow;

            for ( ; spElementNext != NULL ; spElementPre = spElementNext , spElementNext = spElementNext->NextInRow) {
                if (spElementNext->r_index >= r_index) break;
            }

            if (spElementNext == NULL) spElementPre->NextInRow = spElementInstance;
            else {
                spElementPre->NextInRow = spElementInstance;
                spElementInstance->NextInRow = spElementNext;
            }
        }
    }
    return spElementInstance;
}

void resistorStamping() {
    resistorNode* resTmp = resList->first;
    tableNode* tableTmp = tabList->first;
    int posIndex, negIndex;
    double gk;

    for ( ; resTmp != NULL ; resTmp = resTmp->next ) {

        // posIndex = negIndex = -1;
        gk = 1 / resTmp->resistance;

        posIndex = resTmp->posNode->index;
        negIndex = resTmp->negNode->index;

        if (posIndex != -1 && negIndex != -1) {
            getME(sMat, posIndex, posIndex)->r_value += gk;
            getME(sMat, negIndex, negIndex)->r_value += gk;
            getME(sMat, posIndex, negIndex)->r_value -= gk;
            getME(sMat, negIndex, posIndex)->r_value -= gk;
        }
        else if (negIndex == -1 && posIndex != -1) {
            getME(sMat, posIndex, posIndex)->r_value += gk;
        }
        else if (posIndex == -1 && negIndex != -1) {
            getME(sMat, negIndex, negIndex)->r_value += gk;
        }
    }
}

void voltageStamping() {
    int posIndex, negIndex;
    int length, count, tmp;
    voltageNode *volTmp;

    for (volTmp = volList->first, count = 0, length = tabList->length - 1 ; volTmp != NULL ; volTmp = volTmp->next, count++) {
        posIndex = volTmp->posNode->index;
        negIndex = volTmp->negNode->index;
        tmp = length + count;

        // in the below, -1 means the node is "gnd"
        if (negIndex == -1 && posIndex != -1) { // negNode = gnd, posNode != gnd
            getME(sMat, posIndex, tmp)->r_value = 1;  // adding branch current
            getME(sMat, tmp, posIndex)->r_value = 1;  // voltage drop equations
        }
        else if (posIndex != -1 && negIndex != -1) { // both negNode and posNode != gnd
            getME(sMat, posIndex, tmp)->r_value = 1; // adding branch current
            getME(sMat, negIndex, tmp)->r_value = -1; // adding branch current
            getME(sMat, tmp, posIndex)->r_value = 1; // voltage drop equations
            getME(sMat, tmp, negIndex)->r_value = -1; // voltage drop equations
        }
        else if (posIndex == -1 && negIndex != -1) { // posNode = gnd, negNode != gnd
            getME(sMat, negIndex, tmp)->r_value = -1; // adding branch current
            getME(sMat, tmp, negIndex)->r_value = -1; // voltage drop equations
        }

    }
}

void capacitorStamping(int type, double h) {
    int count, length, posIndex, negIndex;
    double step;
    capacitorNode* capTmp;

    if ( type != GEAR2) {
        if (type == TRAPZOIDAL) h /= 2.0;
        count = tabList->length + volList->length - 1;
        for (capTmp = capList->first ; capTmp != NULL ; capTmp = capTmp->next, count++) {
            posIndex = capTmp->posNode->index;
            negIndex = capTmp->negNode->index;
            step = capTmp->cap / h;

            getME(sMat, count, count)->r_value = 1; // add Ic(t+h)
            if (negIndex == -1 && posIndex != -1) { // negNode = gnd, posNode != gnd
                getME(sMat, posIndex, posIndex)->r_value += step;
                getME(sMat, count, posIndex)->r_value -= step;
            }
            else if (posIndex != -1 && negIndex != -1) { // both negNode and posNode != gnd
                getME(sMat, posIndex, posIndex)->r_value += step;
                getME(sMat, posIndex, negIndex)->r_value -= step;
                getME(sMat, negIndex, negIndex)->r_value += step;
                getME(sMat, negIndex, posIndex)->r_value -= step;
                getME(sMat, count, posIndex)->r_value -= step;
                getME(sMat, count, negIndex)->r_value += step;
            }
            else if (posIndex == -1 && negIndex != -1) { // posNode = gnd, negNode != gnd
                getME(sMat, negIndex, negIndex)->r_value += step;
                getME(sMat, count, negIndex)->r_value += step;
            }
        }

    }

    else {
        h /= 1.5;
        count = tabList->length + volList->length - 1;
        for (capTmp = capList->first ; capTmp != NULL ; capTmp = capTmp->next, count++) {
            posIndex = capTmp->posNode->index;
            negIndex = capTmp->negNode->index;
            step = capTmp->cap / h;

            getME(sMat, count, count)->r_value = 1; // add Ic(t+h)
            if (negIndex == -1 && posIndex != -1) { // negNode = gnd, posNode != gnd
                getME(sMat, posIndex, posIndex)->r_value += step;
                getME(sMat, count, posIndex)->r_value -= step;
            }
            else if (posIndex != -1 && negIndex != -1) { // both negNode and posNode != gnd
                getME(sMat, posIndex, posIndex)->r_value += step;
                getME(sMat, posIndex, negIndex)->r_value -= step;
                getME(sMat, negIndex, negIndex)->r_value += step;
                getME(sMat, negIndex, posIndex)->r_value -= step;
                getME(sMat, count, posIndex)->r_value -= step;
                getME(sMat, count, negIndex)->r_value += step;
            }
            else if (posIndex == -1 && negIndex != -1) { // posNode = gnd, negNode != gnd
                getME(sMat, negIndex, negIndex)->r_value += step;
                getME(sMat, count, negIndex)->r_value += step;
            }
        }
    }


}

////////////////////////////////////////////////////////////////
////////////////// used for swap Row////////////////////////////
////////////////////////////////////////////////////////////////
void exchangeCol(Matrix* mat, spElement* e1, spElement* e2, int c1, int c2, int row, int type) {
    int state = 0; // used to stop the loop if the replacement is done
    double dtmp;
    spElement *e1Pre, *e1Next, *e2Pre, *e2Next;
    spElement *spPre, *spCur;
    if (type == 0) {
        // one does not exist
        // c1 = r_skip
        // c2 = r_insert
        // e1 = the element to be inserted
        // both c_index of e1 and e2 are row

        // skip
        if (mat->sprArr[row] == e1) mat->sprArr[row] = e1->NextInCol;
        else {
            for (spPre = mat->sprArr[row], spCur = spPre->NextInCol ; spCur != NULL ; spPre = spCur, spCur = spCur->NextInCol) {
                if (spCur == e1) {
                    spPre->NextInCol = e1->NextInCol;
                }
            }
        }

        // reconnection
        if (mat->sprArr[row] == NULL) mat->sprArr[row] = e1;
        else if (mat->sprArr[row]->c_index > c2) {
            e1->NextInCol = mat->sprArr[row];
            mat->sprArr[row] = e1;
        }
        else {
            for (spPre = mat->sprArr[row], spCur = spPre->NextInCol ; spCur != NULL ; spPre = spCur, spCur = spCur->NextInCol) {
                if (spPre->c_index < c2 && spCur->c_index > c2) {
                    spPre->NextInCol = e1; e1->NextInCol = spCur; break;
                }
            }
            if (spCur == NULL) {
                spPre->NextInCol = e1; e1->NextInCol = NULL;
            }
        }
        e1->c_index = c2;
    }
    else {
        // both exist
        // e1 change to c_index = c2
        // e2 change to c_index = c1 and c1 < c2
        // both c_index of e1 and e2 are row

        // initialization
        e1Pre = e1Next = e2Pre = e2Next = NULL; // these four variables used to connect e1 and e2

        // situations that spcArray needed to be changed
        if (e1 == mat->sprArr[row] && e2 != e1->NextInCol) {
            // printf("1\n");
            e2Next = e1->NextInCol; state++;
        }
        else if (e1 == mat->sprArr[row] && e2 == e1->NextInCol) {
            e1Next = e2->NextInCol; e2->NextInCol = e1;
            e1->NextInCol = e1Next; mat->sprArr[row] = e2;
            state = 3;
        }

        for (spPre = mat->sprArr[row], spCur = spPre->NextInCol ; spCur != NULL && state < 2 ; spPre = spCur, spCur = spCur->NextInCol) {
            if (spPre == e1 && spCur == e2) { // change value
                // printf("Continue\n");
                e1Next = e2->NextInCol; e2->NextInCol = e1;
                e2Pre->NextInCol = e2; e1->NextInCol = e1Next;
                state = 3;
            }
            else if (spCur == e2) {
                e1Pre = spPre; e1Next = spCur->NextInCol; state++;
            }
            else if (spCur == e1) {
                e2Pre = spPre; e2Next = spCur->NextInCol; state++;
            }
        }

        // error checking
        if (state != 3 && state != 2) printf("Errors in connect\n");
        else if (state == 2) {
            // reconnection
            if (e1Pre == NULL) {
                mat->sprArr[row] = e1; e1->NextInCol = e1Next;
            }
            else {
                e1Pre->NextInCol = e1; e1->NextInCol = e1Next;
            }

            if (e2Pre == NULL) {
                mat->sprArr[row] = e2; e2->NextInCol = e2Next;
            }
            else {
                e2Pre->NextInCol = e2; e2->NextInCol = e2Next;
            }
        }

        e1->c_index = c2; e2->c_index = c1;
    }
}
void exchangeRow(Matrix* mat, spElement* e1, spElement* e2, int r1, int r2, int col, int type) {
    int state = 0; // used to stop the loop if the replacement is done
    double dtmp;
    spElement *e1Pre, *e1Next, *e2Pre, *e2Next;
    spElement *spPre, *spCur;
    if (type == 0) {
        // one does not exist
        // r1 = r_skip
        // r2 = r_insert
        // e1 = the element to be inserted
        // both c_index of e1 and e2 are col

        // skip
        if (mat->spcArr[col] == e1) mat->spcArr[col] = e1->NextInRow;
        else {
            for (spPre = mat->spcArr[col], spCur = spPre->NextInRow ; spCur != NULL ; spPre = spCur, spCur = spCur->NextInRow) {
                if (spCur == e1) {
                    spPre->NextInRow = e1->NextInRow;
                }
            }
        }

        // reconnection
        if (mat->spcArr[col] == NULL) mat->spcArr[col] = e1;
        else if (mat->spcArr[col]->r_index > r2) {
            e1->NextInRow = mat->spcArr[col];
            mat->spcArr[col] = e1;
        }
        else {
            for (spPre = mat->spcArr[col], spCur = spPre->NextInRow ; spCur != NULL ; spPre = spCur, spCur = spCur->NextInRow) {
                if (spPre->r_index < r2 && spCur->r_index > r2) {
                    spPre->NextInRow = e1; e1->NextInRow = spCur; break;
                }
            }
            if (spCur == NULL) {
                spPre->NextInRow = e1; e1->NextInRow = NULL;
            }
        }
        e1->r_index = r2;
    }
    else {
        // both exist
        // e1 change to r_index = r2
        // e2 change to r_index = r1 and r1 < r2
        // both c_index of e1 and e2 are col

        // initialization
        e1Pre = e1Next = e2Pre = e2Next = NULL; // these four variables used to connect e1 and e2

        // situations that spcArray needed to be changed
        if (e1 == mat->spcArr[col] && e2 != e1->NextInRow) {
            // printf("1\n");
            e2Next = e1->NextInRow; state++;
        }
        else if (e1 == mat->spcArr[col] && e2 == e1->NextInRow) {
            e1Next = e2->NextInRow; e2->NextInRow = e1;
            e1->NextInRow = e1Next; mat->spcArr[col] = e2;
            state = 3;
        }

        for (spPre = mat->spcArr[col], spCur = spPre->NextInRow ; spCur != NULL && state < 2 ; spPre = spCur, spCur = spCur->NextInRow) {
            if (spPre == e1 && spCur == e2) { // change value
                e1Next = e2->NextInRow; e2->NextInRow = e1;
                e2Pre->NextInRow = e2; e1->NextInRow = e1Next;
                state = 3;
            }
            else if (spCur == e2) {
                e1Pre = spPre; e1Next = spCur->NextInRow; state++;
            }
            else if (spCur == e1) {
                e2Pre = spPre; e2Next = spCur->NextInRow; state++;
            }
        }

        // error checking
        if (state != 3 && state != 2) printf("Errors in connect\n");
        else if (state == 2) {
            // reconnection
            if (e1Pre == NULL) {
                mat->spcArr[col] = e1; e1->NextInRow = e1Next;
            }
            else {
                e1Pre->NextInRow = e1; e1->NextInRow = e1Next;
            }

            if (e2Pre == NULL) {
                mat->spcArr[col] = e2; e2->NextInRow = e2Next;
            }
            else {
                e2Pre->NextInRow = e2; e2->NextInRow = e2Next;
            }
        }

        e1->r_index = r2; e2->r_index = r1;
    }
}


void swapRow(Matrix* mat, int r1, int r2) {
    spElement *spR1, *spR2, *spTmp;

    if (r1 >= mat->size || r2 >= mat->size) printf("Errors in swapRow\n");

    for (spR1 = mat->sprArr[r1], spR2 = mat->sprArr[r2] ; spR2 != NULL && spR1 != NULL ;) {
        // printf("entry\n");
        if (spR1->c_index == spR2->c_index) {
            exchangeRow(sMat, spR1, spR2, r1, r2, spR1->c_index, 1);
            spR1 = spR1->NextInCol; spR2 = spR2->NextInCol;
        }
        else if (spR1->c_index > spR2->c_index) {
            exchangeRow(sMat, spR2, NULL, r2, r1, spR2->c_index, 0);
            spR2 = spR2->NextInCol;
        }
        else {
            exchangeRow(sMat, spR1, NULL, r1, r2, spR1->c_index, 0);
            spR1 = spR1->NextInCol;
        }
    }

    if (spR1 != NULL) {
        for ( ; spR1 != NULL; spR1 = spR1->NextInCol ) {
            // printf("entry1\n");
            exchangeRow(sMat, spR1, NULL, r1, r2, spR1->c_index, 0);
        }
    }
    else if (spR2 != NULL) {
        for ( ; spR2 != NULL; spR2 = spR2->NextInCol ) {
            // printf("entry2\n");
            exchangeRow(sMat, spR2, NULL, r2, r1, spR2->c_index, 0);
        }
    }

    spTmp = mat->sprArr[r1]; mat->sprArr[r1] = mat->sprArr[r2];
    mat->sprArr[r2] = spTmp;


}

void swapCol(Matrix* mat, int c1, int c2) {
    spElement *spC1, *spC2, *spTmp;

    if (c1 >= mat->size || c2 >= mat->size) printf("Errors in swapCol\n");

    for (spC1 = mat->spcArr[c1], spC2 = mat->spcArr[c2] ; spC2 != NULL && spC1 != NULL ;) {
        // printf("entry\n");
        if (spC1->r_index == spC2->r_index) {
            exchangeCol(sMat, spC1, spC2, c1, c2, spC1->r_index, 1);
            spC1 = spC1->NextInRow; spC2 = spC2->NextInRow;
        }
        else if (spC1->r_index > spC2->r_index) {
            exchangeCol(sMat, spC2, NULL, c2, c1, spC2->r_index, 0);
            spC2 = spC2->NextInRow;
        }
        else {
            exchangeCol(sMat, spC1, NULL, c1, c2, spC1->r_index, 0);
            spC1 = spC1->NextInRow;
        }
    }

    if (spC1 != NULL) {
        for ( ; spC1 != NULL; spC1 = spC1->NextInRow ) {
            // printf("entry1\n");
            exchangeCol(sMat, spC1, NULL, c1, c2, spC1->r_index, 0);
        }
    }
    else if (spC2 != NULL) {
        for ( ; spC2 != NULL; spC2 = spC2->NextInRow ) {
            // printf("entry2\n");
            exchangeCol(sMat, spC2, NULL, c2, c1, spC2->r_index, 0);
        }
    }
    spTmp = mat->spcArr[c1]; mat->spcArr[c1] = mat->spcArr[c2];
    mat->spcArr[c2] = spTmp;
}

////////////////////////////////////////////////////////////////
//////////////////////// End ///////////////////////////////////
////////////////////////////////////////////////////////////////


spElement* quickSearchwoCreate(spElement* center, int r_index, int c_index) {
    spElement *spTmp = center;
    for ( ; spTmp != NULL && spTmp->c_index < c_index ; spTmp = spTmp->NextInCol);
    if (spTmp->c_index == c_index) return spTmp;
    else {
        printf("Error in quickSearchwoCreate, the element does not exist\n"); return NULL;
    }
}


void MATfact(Matrix* mat) {
    int i, j, k;
    int n = mat->size;
    spElement *spTmp, *spRow, *spCol, *spDiag;

    for (i = 0 ; i < n ; i++) {
        spDiag = mat->Diag[i];
        for (spTmp = spDiag->NextInRow ; spTmp != NULL ; spTmp = spTmp->NextInRow) {
            spTmp->r_value /= spDiag->r_value;
        }
        for ( spRow = mat->Diag[i]->NextInRow ; spRow != NULL ; spRow = spRow->NextInRow) {
            spTmp = spRow;
            for ( spCol = mat->Diag[i]->NextInCol ; spCol != NULL ; spCol = spCol->NextInCol) {
                spTmp = quickSearchwoCreate(spTmp, spRow->r_index, spCol->c_index);
                spTmp->r_value -= spRow->r_value * spCol->r_value;
            }
        }
    }
}

void fwdSubs(Matrix* mat, double* y) {
    int i, j, n = mat->size;
    spElement *spTmp;

    for ( i = 0 ; i < n ; i++ ) {
        for (spTmp = mat->Diag[i]->NextInRow ; spTmp != NULL ; spTmp = spTmp->NextInRow) {
            y[spTmp->r_index] -= spTmp->r_value * y[spTmp->c_index];
        }
    }
}

void bckSubs(Matrix* mat, double* x) {
    int i, n = mat->size;
    spElement* spTmp;

    for ( i = n - 1 ; i >= 0 ; i-- ) {
        x[i] /= mat->Diag[i]->r_value;

        for (spTmp = mat->spcArr[i] ; spTmp != mat->Diag[i] ; spTmp = spTmp->NextInRow ) {
            x[spTmp->r_index] -= spTmp->r_value * x[spTmp->c_index];
        }
    }
}

void Diagonalization(Matrix* mat) {
    int posIndex, t_len = tabList->length - 1;
    int i, tmp1, tmp2;
    voltageNode *volTmp;
    spElement* spTmp;

    for ( volTmp = volList->first ; t_len < mat->size && volTmp != NULL ; volTmp = volTmp->next, t_len++ ) {
        posIndex = volTmp->posNode->index;
        swapRow(mat, posIndex, t_len);

        tmp1 = mat->ExtToIntRowMap[posIndex];
        tmp2 = mat->ExtToIntRowMap[t_len];
        mat->ExtToIntRowMap[posIndex] = tmp2;
        mat->ExtToIntRowMap[t_len] = tmp1;

        mat->IntToExtRowMap[tmp2] = posIndex;
        mat->IntToExtRowMap[tmp1] = t_len;
    }

    for (i = 0 ; i < sMat->size ; i++) {
        for (spTmp = sMat->sprArr[i]; spTmp->r_index != spTmp->c_index ; spTmp = spTmp->NextInCol);
        sMat->Diag[i] = spTmp;
    }
}



void exchangeRowFast(Matrix * mat, spElement * spR1, spElement * spR2, spElement** spPre, int r1, int r2, int col, int type, int state) {
    spElement *spTmp, *spTmp1;

    if (state == 0) { // R1:x R2:o

        if (*spPre != NULL) {
            // error checking
            if ((*spPre)->NextInRow != spR1) {
                printf("0: Error in the situation x o to o x\n");
                printspElement((*spPre)->NextInRow);
                printspElement(spR1);
                printf("\n");
            }
            (*spPre)->NextInRow = spR1->NextInRow; // break the connection
            for (spTmp = *spPre ; spTmp->NextInRow != NULL && spTmp->NextInRow->r_index < r2 ; spTmp = spTmp->NextInRow);

            if (spTmp->NextInRow == NULL) { // the bottom of the list
                spTmp->NextInRow = spR1;
                spR1->NextInRow = NULL;
            }
            else { // in the middle of the list
                spR1->NextInRow = spTmp->NextInRow;
                spTmp->NextInRow = spR1;
            }
        }
        else {
            if (spR1 != mat->spcArr[col]) {
                printf("1: Error in the situation x o to o x\n");
            }
            if (mat->spcArr[col]->NextInRow != NULL && mat->spcArr[col]->NextInRow->r_index < r2) {
                mat->spcArr[col] = spR1->NextInRow;
                for (spTmp = mat->spcArr[col] ; spTmp->NextInRow != NULL && spTmp->NextInRow->r_index < r2 ; spTmp = spTmp->NextInRow);
                if (spTmp->NextInRow == NULL) {
                    spTmp->NextInRow = spR1;
                    spR1->NextInRow = NULL;
                }
                else if (spTmp != spR1) { // if order will be changed
                    spR1->NextInRow = spTmp->NextInRow;
                    spTmp->NextInRow = spR1;
                }
            }

        }
    }

    else if (state == 1) { // o x -> x o
        if (*spPre != NULL) {
            // break the connection
            for (spTmp = *spPre ; spTmp->NextInRow != spR2 ; spTmp = spTmp->NextInRow);
            if (spTmp != *spPre) { // if the order needs to be changed
                spTmp->NextInRow = spR2->NextInRow; // skip spR2 in the list
                spR2->NextInRow = (*spPre)->NextInRow; // reconnect spR2
                (*spPre)->NextInRow = spR2;
            }
        }
        else {
            if (mat->spcArr[col] != spR2) { // if the order needs to be changed
                for (spTmp = mat->spcArr[col] ; spTmp->NextInRow != spR2 ; spTmp = spTmp->NextInRow);
                spTmp->NextInRow = spR2->NextInRow;
                spR2->NextInRow = mat->spcArr[col];
                mat->spcArr[col] = spR2;
            }
        }
        *spPre = spR2;
    }

    else if (state == 2) {
        if (*spPre != NULL) {
            if ((*spPre)->NextInRow != spR1) {
                printf("0:Error in x x to x x\n");
            }
            for (spTmp = spR1; spTmp->NextInRow != spR2; spTmp = spTmp->NextInRow);
            if (spTmp == spR1) { // exchange issue
                spR1->NextInRow = spR2->NextInRow;
                spR2->NextInRow = spR1;
                (*spPre)->NextInRow = spR2;
            }
            else {
                spTmp1 = spR1->NextInRow;
                spTmp->NextInRow = spR1; spR1->NextInRow = spR2->NextInRow;
                (*spPre)->NextInRow = spR2; spR2->NextInRow = spTmp1;
            }
        }
        else {
            if (mat->spcArr[col] != spR1) printf("0:Error in x x to x x\n");
            for (spTmp = spR1; spTmp->NextInRow != spR2; spTmp = spTmp->NextInRow);

            if (spTmp == spR1) { // exchange issue
                spR1->NextInRow = spR2->NextInRow;
                spR2->NextInRow = spR1;
            }
            else {
                spTmp1 = spR1->NextInRow;
                spTmp->NextInRow = spR1; // connect the latter
                spR1->NextInRow = spR2->NextInRow;
                spR2->NextInRow = spTmp1; // connect the former
                // (*spPre)->NextInRow = spR2; spR2->NextInRow = spTmp1;
            }

            mat->spcArr[col] = spR2;
        }

        *spPre = spR2;
    }


}


void swapRowFast(Matrix * mat, int r1, int r2, spElement** spPre) {
    spElement *spR1, *spR2, *spTmp;

    // error checking
    if (r1 >= mat->size || r2 >= mat->size) printf("Errors in swapRow\n");

    for (spR1 = mat->sprArr[r1], spR2 = mat->sprArr[r2] ; spR2 != NULL && spR1 != NULL ;) {

        if (spR1->c_index == spR2->c_index) { // both exist
            exchangeRowFast(mat, spR1, spR2, &spPre[spR1->c_index], r1, r2, spR2->c_index, 0, 2);
            spR1->r_index = r2; spR2->r_index = r1;
            spR1 = spR1->NextInCol; spR2 = spR2->NextInCol;
        }
        else if (spR1->c_index > spR2->c_index) {
            exchangeRowFast(mat, NULL, spR2, &spPre[spR2->c_index], r1, r2, spR2->c_index, 0, 1);
            spR2->r_index = r1;
            spR2 = spR2->NextInCol;
        }
        else {
            exchangeRowFast(mat, spR1, NULL, &spPre[spR1->c_index], r1, r2, spR1->c_index, 0, 0);
            spR1->r_index = r2;
            spR1 = spR1->NextInCol;
        }
    }

    if (spR1 != NULL) {
        for ( ; spR1 != NULL; spR1 = spR1->NextInCol ) {
            exchangeRowFast(mat, spR1, NULL, &spPre[spR1->c_index], r1, r2, spR1->c_index, 0, 0);
            spR1->r_index = r2;
        }
    }
    else if (spR2 != NULL) {
        for ( ; spR2 != NULL ; spR2 = spR2->NextInCol ) {
            exchangeRowFast(mat, NULL, spR2, &spPre[spR2->c_index], r1, r2, spR2->c_index, 0, 1);
            spR2->r_index = r1;
        }
    }

    spTmp = mat->sprArr[r1]; mat->sprArr[r1] = mat->sprArr[r2];
    mat->sprArr[r2] = spTmp;

}

void exchangeColFast(Matrix * mat, spElement * spC1, spElement * spC2, spElement** spPre, int c1, int c2, int row, int type, int state) {
    spElement *spTmp, *spTmp1;

    if (state == 0) { // c1:x c2:o

        if (*spPre != NULL) {
            // error checking
            if ((*spPre)->NextInCol != spC1) {
                printf("0: Error in the situation x o to o x\n");
                printspElement((*spPre)->NextInCol);
                printspElement(spC1);
                printf("\n");
            }
            // printf("I am here, c2 = %d\n", c2);
            (*spPre)->NextInCol = spC1->NextInCol; // break the connection
            for (spTmp = *spPre ; spTmp->NextInCol != NULL && spTmp->NextInCol->c_index < c2 ; spTmp = spTmp->NextInCol);

            if (spTmp->NextInCol == NULL) { // the bottom of the list
                spTmp->NextInCol = spC1;
                spC1->NextInCol = NULL;
            }
            else { // in the middle of the list
                // printf("I am here again\n");
                spC1->NextInCol = spTmp->NextInCol;
                spTmp->NextInCol = spC1;
            }
        }
        else {
            if (spC1 != mat->sprArr[row]) {
                printf("1: Error in the situation x o to o x\n");
            }
            if (mat->sprArr[row]->NextInCol != NULL && mat->sprArr[row]->NextInCol->c_index < c2) {
                mat->sprArr[row] = spC1->NextInCol;
                for (spTmp = mat->sprArr[row] ; spTmp->NextInCol != NULL && spTmp->NextInCol->c_index < c2 ; spTmp = spTmp->NextInCol);
                if (spTmp->NextInCol == NULL) {
                    spTmp->NextInCol = spC1;
                    spC1->NextInCol = NULL;
                }
                else if (spTmp != spC1) { // if order will be changed
                    spC1->NextInCol = spTmp->NextInCol;
                    spTmp->NextInCol = spC1;
                }
            }
        }
    }

    else if (state == 1) { // o x -> x o
        if (*spPre != NULL) {
            // break the connection
            for (spTmp = *spPre ; spTmp->NextInCol != spC2 ; spTmp = spTmp->NextInCol);
            if (spTmp != *spPre) { // if the order needs to be changed
                spTmp->NextInCol = spC2->NextInCol; // skip spC2 in the list
                spC2->NextInCol = (*spPre)->NextInCol; // reconnect spC2
                (*spPre)->NextInCol = spC2;
            }
        }
        else {
            if (mat->sprArr[row] != spC2) { // if the order needs to be changed
                for (spTmp = mat->sprArr[row] ; spTmp->NextInCol != spC2 ; spTmp = spTmp->NextInCol);
                spTmp->NextInCol = spC2->NextInCol;
                spC2->NextInCol = mat->sprArr[row];
                mat->sprArr[row] = spC2;
            }
        }
        *spPre = spC2;
    }

    else if (state == 2) {
        if (*spPre != NULL) {
            if ((*spPre)->NextInCol != spC1) {
                printf("0:Error in x x to x x\n");
            }
            for (spTmp = spC1; spTmp->NextInCol != spC2; spTmp = spTmp->NextInCol);
            if (spTmp == spC1) { // exchange issue
                spC1->NextInCol = spC2->NextInCol;
                spC2->NextInCol = spC1;
                (*spPre)->NextInCol = spC2;
            }
            else {
                spTmp1 = spC1->NextInCol;
                spTmp->NextInCol = spC1; spC1->NextInCol = spC2->NextInCol;
                (*spPre)->NextInCol = spC2; spC2->NextInCol = spTmp1;
            }
        }
        else {
            if (mat->sprArr[row] != spC1) printf("0:Error in x x to x x\n");
            for (spTmp = spC1; spTmp->NextInCol != spC2; spTmp = spTmp->NextInCol);

            if (spTmp == spC1) { // exchange issue
                // printf("exchange\n");

                spC1->NextInCol = spC2->NextInCol;
                spC2->NextInCol = spC1;
                // (*spPre)->NextInCol = spC2;
            }
            else {
                // printf("NO\n");
                spTmp1 = spC1->NextInCol;
                spTmp->NextInCol = spC1; // connect the latter
                spC1->NextInCol = spC2->NextInCol;
                spC2->NextInCol = spTmp1; // connect the former
                // (*spPre)->NextInCol = spC2; spC2->NextInCol = spTmp1;
            }

            mat->sprArr[row] = spC2;
        }

        *spPre = spC2;
    }


}


void swapColFast(Matrix * mat, int c1, int c2, spElement** spPre) {
    spElement *spC1, *spC2, *spTmp;

    // error checking
    if (c1 >= mat->size || c2 >= mat->size) printf("Errors in swapRow\n");

    for (spC1 = mat->spcArr[c1], spC2 = mat->spcArr[c2] ; spC2 != NULL && spC1 != NULL ;) {

        if (spC1->r_index == spC2->r_index) { // both exist
            exchangeColFast(mat, spC1, spC2, &spPre[spC1->r_index], c1, c2, spC2->r_index, 0, 2);
            spC1->c_index = c2; spC2->c_index = c1;
            spC1 = spC1->NextInRow; spC2 = spC2->NextInRow;
        }
        else if (spC1->r_index > spC2->r_index) {
            exchangeColFast(mat, NULL, spC2, &spPre[spC2->r_index], c1, c2, spC2->r_index, 0, 1);
            spC2->c_index = c1;
            spC2 = spC2->NextInRow;
        }
        else {
            exchangeColFast(mat, spC1, NULL, &spPre[spC1->r_index], c1, c2, spC1->r_index, 0, 0);
            spC1->c_index = c2;

            spC1 = spC1->NextInRow;
        }
    }

    if (spC1 != NULL) {
        for ( ; spC1 != NULL; spC1 = spC1->NextInRow ) {
            exchangeColFast(mat, spC1, NULL, &spPre[spC1->r_index], c1, c2, spC1->r_index, 0, 0);
            spC1->c_index = c2;
        }
    }
    else if (spC2 != NULL) {
        for ( ; spC2 != NULL ; spC2 = spC2->NextInRow ) {
            exchangeColFast(mat, NULL, spC2, &spPre[spC2->r_index], c1, c2, spC2->r_index, 0, 1);
            spC2->c_index = c1;
        }
    }

    spTmp = mat->spcArr[c1]; mat->spcArr[c1] = mat->spcArr[c2];
    mat->spcArr[c2] = spTmp;

}

spElement* quickSearch2(Matrix* mat, spElement* R, spElement** Cpre, int r_index, int c_index, bool* exist) {
    spElement *spTmp;
    spElement *spPre, *spNext;

    // spTmp = (spElement*) malloc(sizeof(spElement));
    // spTmp->r_index = r_index; spTmp->c_index = c_index;
    // spTmp->NextInCol = spTmp->NextInRow = NULL;

    // row processing
    for (spPre = R, spNext = R->NextInCol ; spNext != NULL && spNext->c_index < c_index ; spPre = spNext, spNext = spNext->NextInCol);
    if (spNext == NULL) {
        spTmp = (spElement*) malloc(sizeof(spElement));
        spTmp->r_index = r_index; spTmp->c_index = c_index;
        spTmp->NextInCol = spTmp->NextInRow = NULL;
        spPre->NextInCol = spTmp;
        fill_in++;
    }
    else if (spNext->c_index > c_index) {
        spTmp = (spElement*) malloc(sizeof(spElement));
        spTmp->r_index = r_index; spTmp->c_index = c_index;
        spTmp->NextInCol = spTmp->NextInRow = NULL;
        spPre->NextInCol = spTmp;
        spTmp->NextInCol = spNext;
        fill_in++;
    }
    else if (spNext->c_index == c_index) {
        *exist = true;
        *Cpre = spNext;
        // free(spTmp);
        return spNext;
    }


    // connect in the column order
    if (mat->spcArr[c_index]->r_index > r_index) {
        spNext = mat->spcArr[c_index];
        mat->spcArr[c_index] = spTmp;
        mat->spcArr[c_index]->NextInRow = spNext;
    }
    else {
        if (*Cpre == NULL) {
            if (c_index > r_index) spPre = mat->spcArr[c_index];
            else spPre = mat->Diag[c_index];
        }
        else {
            spPre = *Cpre;
        }

        spNext = spPre->NextInRow;

        for ( ; spNext != NULL ; spPre = spNext , spNext = spNext->NextInRow) {
            if (spNext->r_index >= r_index) break;
        }

        if (spNext == NULL) spPre->NextInRow = spTmp;
        else {
            spPre->NextInRow = spTmp;
            spTmp->NextInRow = spNext;
        }
    }

    *Cpre = spTmp;
    return spTmp;
}

void printSparseMatrixFullSym(Matrix * mat) {
    tableNode* tableTmp = tabList->first->next;
    spElement* spElementTmp;
    voltageNode* volTmp;
    int i, j, gap = 0;

    for ( ; tableTmp != NULL ; tableTmp = tableTmp->next) printf("%-9.5s ", tableTmp->Name);
    for ( volTmp = volList->first ; volTmp != NULL ; volTmp = volTmp->next) printf("i.%-7.5s ", volTmp->Name);
    printf("\n");

    for ( i = 0 ; i < mat->size ; i++) {

        gap = -1;
        for (spElementTmp = mat->sprArr[i] ; spElementTmp != NULL ; spElementTmp = spElementTmp->NextInCol) {
            gap = spElementTmp->c_index - gap - 1;
            for (j = 0 ; j < gap ; j++) printf("o");
            if (spElementTmp->r_value != 0.0) printf("x");
            else printf("v");
            // printf("%-9.5lf ", spElementTmp->r_value);
            gap = spElementTmp->c_index;
        }
        printf("\n");
    }

    printf("\n");
}


void MarkowitzSymbolic(Matrix * mat) {
    int i, j, min, index, tmp1, tmp2, maxTie, in = 0;
    int *rowCount = (int*) malloc(sizeof(int) * mat->size);
    int *colCount = (int*) malloc(sizeof(int) * mat->size);
    int *MarkNumber = (int*) malloc(sizeof(int) * mat->size);
    int *tieBreakIndex = (int*) malloc(sizeof(int) * mat->size);
    bool exist;
    spElement *spTmp, *spRow, *spCol;
    spElement **spPreRow = (spElement**) malloc(sizeof(spElement*) * mat->size);
    spElement **spPreCol = (spElement**) malloc(sizeof(spElement*) * mat->size);
    spElement **spSymRow = (spElement**) malloc(sizeof(spElement*) * mat->size);
    spElement **spSymCol = (spElement**) malloc(sizeof(spElement*) * mat->size);



    clock_t t_sym = 0, t, t_find = 0, t_swap = 0;

    // initialize
    for (i = 0 ; i < mat->size ; i++) {
        rowCount[i] = colCount[i] = tieBreakIndex[i] = 0;
        for (spTmp = mat->sprArr[i] ; spTmp != NULL ; tieBreakIndex[i] += spTmp->c_index * spTmp->c_index, spTmp = spTmp->NextInCol, rowCount[i]++);
        for (spTmp = mat->spcArr[i] ; spTmp != NULL ; spTmp = spTmp->NextInRow, colCount[i]++);
        MarkNumber[i] = (rowCount[i] - 1) * (colCount[i] - 1);
        tieBreakIndex[i] -= i * i;
        spPreRow[i] = spPreCol[i] = spSymCol[i] = spSymRow[i] = NULL;
        // printf("%d ", tieBreakIndex[i]);
    }
    // printf("\n");


    for (i = 0 ; i < mat->size - 1 ; i++) {
        //choose the min in the remaining
        t = clock();

        for (j = i + 1, min = MarkNumber[i], maxTie = tieBreakIndex[i], index = i ; min != 0 && j < mat->size ; j++) {
            if (min > MarkNumber[j]) {
                min = MarkNumber[j]; index = j; maxTie = tieBreakIndex[j];
            }
            else if (min == MarkNumber[j] && maxTie < tieBreakIndex[j]) {
                index = j; maxTie = tieBreakIndex[j];
            }
        }

        t_find += clock() - t;
        //pivot
        t = clock();
        if (index != i) {
            in++;

            swapRowFast(mat, i, index, spPreRow);
            spTmp = spPreCol[i]; spPreCol[i] = spPreCol[index];
            spPreCol[index] = spTmp;
            swapColFast(mat, i, index, spPreCol);
            spTmp = spPreRow[i]; spPreRow[i] = spPreRow[index];
            spPreRow[index] = spTmp;

            tmp1 = mat->ExtToIntRowMap[i];
            tmp2 = mat->ExtToIntRowMap[index];

            mat->ExtToIntRowMap[i] = tmp2;
            mat->ExtToIntRowMap[index] = tmp1;
            mat->IntToExtRowMap[tmp2] = i;
            mat->IntToExtRowMap[tmp1] = index;

            tmp1 = mat->ExtToIntColMap[i];
            tmp2 = mat->ExtToIntColMap[index];

            mat->ExtToIntColMap[i] = tmp2;
            mat->ExtToIntColMap[index] = tmp1;
            mat->IntToExtColMap[tmp2] = i;
            mat->IntToExtColMap[tmp1] = index;

            spTmp = mat->Diag[i];
            mat->Diag[i] = mat->Diag[index];
            mat->Diag[index] = spTmp;

            tmp1 = rowCount[i];
            rowCount[i] = rowCount[index];
            rowCount[index] = tmp1;

            tmp1 = colCount[i];
            colCount[i] = colCount[index];
            colCount[index] = tmp1;

            tmp1 = MarkNumber[i];
            MarkNumber[i] = MarkNumber[index];
            MarkNumber[index] = tmp1;

            tmp1 = tieBreakIndex[i];
            tieBreakIndex[i] = tieBreakIndex[index];
            tieBreakIndex[index] = tmp1;

            // printf("Change %d and %d\n", i, index);

            // printSparseMatrixFull(sMat);
            // for(j = 0 ; j < mat->size ; j++) printf("%d ", tieBreakIndex[j]);
            //     printf("\n");
            // printf("%d and %d swap\n",i,index );
        }
        else if ( i == index ) {
            for (spTmp = mat->spcArr[i] ; spTmp != NULL ; spTmp = spTmp->NextInRow) spPreCol[spTmp->r_index] = spTmp;
            for (spTmp = mat->sprArr[i] ; spTmp != NULL ; spTmp = spTmp->NextInCol) spPreRow[spTmp->c_index] = spTmp;

        }
        t_swap += clock() - t;
        // printf("%lf\n", (double)(clock()-t)/CLOCKS_PER_SEC);

        //partial update
        for (spTmp = mat->Diag[i]->NextInCol ; spTmp != NULL ; spTmp = spTmp->NextInCol) {
            colCount[spTmp->c_index]--; MarkNumber[spTmp->c_index] -= (rowCount[spTmp->c_index] - 1);
        }

        for (spTmp = mat->Diag[i]->NextInRow ; spTmp != NULL ; spTmp = spTmp->NextInRow) {
            rowCount[spTmp->r_index]--; MarkNumber[spTmp->r_index] -= (colCount[spTmp->r_index] - 1);
            tieBreakIndex[spTmp->r_index] -= i * i;
        }

        //symbolic
        t = clock();
        for (j = i + 1 ; j < mat->size ; j++) spSymCol[j] = spPreRow[j];
        for ( spRow = mat->Diag[i]->NextInRow ; spRow != NULL ; spRow = spRow->NextInRow) {
            spTmp = spRow;
            for ( spCol = mat->Diag[i]->NextInCol ; spCol != NULL ; spCol = spCol->NextInCol) {
                exist = false;
                spTmp = quickSearch2(mat, spTmp, &spSymCol[spCol->c_index], spRow->r_index, spCol->c_index, &exist);
                if (!exist) {
                    tieBreakIndex[spRow->r_index] += spCol->c_index * spCol->c_index;
                    rowCount[spRow->r_index]++; MarkNumber[spRow->r_index] += (colCount[spRow->r_index] - 1);
                    colCount[spCol->c_index]++; MarkNumber[spCol->c_index] += (rowCount[spCol->c_index] - 1);
                }
            }
        }
        t_sym += (clock() - t);
        // printSparseMatrixFullSym(mat);
    }
    // printf("in: %d, swap time: %lf\n", in, (double)(t_swap) / CLOCKS_PER_SEC );
    // printf("Candidate time: %lf\n", (double)(t_find) / CLOCKS_PER_SEC );
    // printf("symbolic time: %lf\n", (double)(t_sym) / CLOCKS_PER_SEC );

    free(rowCount); free(colCount); free(spPreRow); free(spPreCol); free(tieBreakIndex);
}

// function below is used to print //

void printspElement(spElement * e) {
    printf("(%d, ", e->r_index );
    printf("%d,", e->c_index );
    printf("%lf) ", e->r_value );
}

void printOneLine(spElement * e, int type) {
    if (type == INROW) {
        printf("INROW\n");
        for ( ; e != NULL ; e = e->NextInCol) {
            printf("(%d,%d,%-7.5lf) -> ", e->r_index, e->c_index, e->r_value);
        }
        printf("\n");
    }
    else if (type == INCOL) {
        printf("INCOL\n");
        for ( ; e != NULL ; e = e->NextInRow) {
            printf("(%d,%d,%-7.5lf) -> ", e->r_index, e->c_index, e->r_value);
        }
        printf("\n");

    }
}

void printSparseMatrix(Matrix * mat, int type) {
    int i;
    spElement *spTmp;
    if (type == INROW) {
        printf("INROW:\n");
        for (i = 0 ; i < mat->size ; i++) {
            for (spTmp = mat->sprArr[i] ; spTmp != NULL ; spTmp = spTmp->NextInCol) {
                printf("(%d,%d,%-7.5lf) -> ", spTmp->r_index, spTmp->c_index, spTmp->r_value);
            }
            printf("\n");
        }
    }
    else {
        printf("INCOL:\n");
        for (i = 0 ; i < mat->size ; i++) {
            for (spTmp = mat->spcArr[i] ; spTmp != NULL ; spTmp = spTmp->NextInRow) {
                printf("(%d,%d,%-7.5lf) -> ", spTmp->r_index, spTmp->c_index, spTmp->r_value);
            }
            printf("\n");
        }
    }
}

void printSparseMatrixFull(Matrix * mat) {
    tableNode* tableTmp = tabList->first->next;
    spElement* spElementTmp;
    voltageNode* volTmp;
    capacitorNode* capTmp;
    int i, j, gap = 0;

    for ( ; tableTmp != NULL ; tableTmp = tableTmp->next) printf("%-9.5s ", tableTmp->Name);
    for ( volTmp = volList->first ; volTmp != NULL ; volTmp = volTmp->next) printf("i.%-7.5s ", volTmp->Name);
    for ( capTmp = capList->first ; capTmp != NULL ; capTmp = capTmp->next) printf("i.%-7.5s", capTmp->Name);
    printf("\n");

    for ( i = 0 ; i < mat->size ; i++) {

        gap = -1;
        for (spElementTmp = mat->sprArr[i] ; spElementTmp != NULL ; spElementTmp = spElementTmp->NextInCol) {
            gap = spElementTmp->c_index - gap - 1;
            for (j = 0 ; j < gap ; j++) printf("          ");
            printf("%-9.5lf ", spElementTmp->r_value);
            gap = spElementTmp->c_index;
        }
        printf("\n");
    }

    printf("\n");
}

void cleanMatrix(Matrix* mat) {
    int i;
    spElement* spElementTmp;

    for (i = 0 ; i < mat->size ; i++) {

        for (spElementTmp = mat->sprArr[i] ; spElementTmp != NULL ; spElementTmp = spElementTmp->NextInCol) {
            spElementTmp->r_value = 0;
        }
    }
    for(i = 0 ; i < mat->size ; i++) {
        mat->sprArr[i] = mat->spcArr[i] = mat->Diag[i] = NULL;
        mat->ExtToIntRowMap[i] = mat->ExtToIntColMap[i] = mat->IntToExtRowMap[i] = mat->IntToExtColMap[i] = i;
    }
}


// void m_fill_in_algorithm(Matrix* mat, int* order) {
//     int i, j, deg, minDeg, min, min1, index, loc, n = mat->size;
//     spElement *spTmp, *spRow, *spCol;
//     vertex** graph = (vertex**) malloc(sizeof(vertex*) * n);
//     vertex *verTmp, *verTmp1;
//     vertex** arr;
//     adjVertex *adjTmp, *adjTmp1, *adjTmp2;


//     // allocate memories for graph
//     for (i = 0 ; i < n ; i++) {
//         graph[i] = (vertex*) malloc(sizeof(vertex));
//         graph[i]->deg = graph[i]->fill_in = 0;
//         graph[i]->v_index = i;
//         graph[i]->first = graph[i]->last = NULL;
//     }

//     // initialize for graph
//     for (i = 0 ; i < n ; i++) {
//         for (spRow = mat->Diag[i]->NextInRow, spCol = mat->Diag[i]->NextInCol ; spRow != NULL && spCol != NULL ; ) {
//             if (spRow->r_index == spCol->c_index) {
//                 insertEdgeFast(graph[i], graph[spRow->r_index]);
//                 insertEdgeFast(graph[spRow->r_index], graph[i]);
//                 spRow = spRow->NextInRow; spCol = spCol->NextInCol;
//             }
//             else if (spRow->r_index < spCol->c_index) {
//                 insertEdgeFast(graph[i], graph[spRow->r_index]);
//                 insertEdgeFast(graph[spRow->r_index], graph[i]);
//                 spRow = spRow->NextInRow;
//             }
//             else {
//                 insertEdgeFast(graph[i], graph[spCol->c_index]);
//                 insertEdgeFast(graph[spCol->c_index], graph[i]);
//                 spCol = spCol->NextInCol;
//             }
//         }
//         if (spRow != NULL) {
//             for ( ; spRow != NULL ; spRow = spRow->NextInRow) {
//                 insertEdgeFast(graph[i], graph[spRow->r_index]);
//                 insertEdgeFast(graph[spRow->r_index], graph[i]);
//             }
//         }
//         else if ( spCol != NULL ) {
//             for ( ; spCol != NULL ; spCol = spCol->NextInCol) {
//                 insertEdgeFast(graph[i], graph[spCol->c_index]);
//                 insertEdgeFast(graph[spCol->c_index], graph[i]);
//             }
//         }
//     }

//     // printSparseMatrixFull(mat);
//     // printGraph(graph, n, true, true);

//     // initialize fill-ins
//     // printf("HI\n");
//     clock_t t, t_el;

//     for (i = 0 ; i < n ; i++) {
//         deg = graph[i]->deg;
//         if (deg != 0) {
//             for (adjTmp = graph[i]->first ; adjTmp->next != NULL ; adjTmp = adjTmp->next) {
//                 for (adjTmp1 = adjTmp->v->first, adjTmp2 = adjTmp->next ; adjTmp1 != NULL && adjTmp2 != NULL; ) {
//                     if (adjTmp1->v->v_index < adjTmp2->v->v_index) adjTmp1 = adjTmp1->next;
//                     else if (adjTmp1->v->v_index > adjTmp2->v->v_index) {
//                         // printf("fill-in: %d and %d\n", adjTmp->v->v_index, adjTmp2->v->v_index );
//                         insertPairFast(graph[i], adjTmp->v, adjTmp2->v);
//                         // graph[i]->fill_in++;
//                         adjTmp2 = adjTmp2->next;
//                     }
//                     else {
//                         adjTmp2 = adjTmp2->next;
//                         adjTmp1 = adjTmp1->next;
//                     }
//                 }
//                 if (adjTmp1 == NULL) {
//                     for ( ; adjTmp2 != NULL ; adjTmp2 = adjTmp2->next) {
//                         // graph[i]->fill_in++;
//                         insertPairFast(graph[i], adjTmp->v, adjTmp2->v);
//                         // printf("fill-in: %d and %d\n", adjTmp->v->v_index, adjTmp2->v->v_index );
//                     }
//                 }
//             }
//         }

//     }
//     // find the order
//     // printGraph(graph, n, true, true);
//     // int tmp1, tmp2;
//     for (i = 0 ; i < n ; i++) {
//         // find the minimum
//         // printf("%d\n", i);
//         // printGraph(graph, n, true, true);
//         // printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");

//         // minimum fill-in + deg
//         // for (min = graph[i]->fill_in, index = graph[i]->v_index, minDeg = graph[i]->deg, loc = i, j = i + 1 ; j < n ; j++) {
//         //     if (graph[j]->fill_in < min) {
//         //         min = graph[j]->fill_in;
//         //         loc = j;
//         //         index = graph[j]->v_index;
//         //         minDeg = graph[j]->deg;
//         //     }
//         //     else if (graph[j]->fill_in == min && graph[j]->deg < minDeg) {
//         //         loc = j;
//         //         index = graph[j]->v_index;
//         //         minDeg = graph[j]->deg;
//         //     }
//         // }


//         // minimum fill-in
//         for (min = graph[i]->fill_in, index = graph[i]->v_index, loc = i, j = i + 1 ; j < n ; j++) {
//             if (graph[j]->fill_in < min) {
//                 min = graph[j]->fill_in;
//                 loc = j;
//                 index = graph[j]->v_index;
//             }
//         }


//         // minimum degree
//         // for (min = graph[i]->deg, index = graph[i]->v_index, loc = i, j = i + 1 ; j < n ; j++) {
//         //     // printf("index = %d, deg = %d\n", index, min);
//         //     if (graph[j]->deg < min) {
//         //         min = graph[j]->deg;
//         //         loc = j;
//         //         index = graph[j]->v_index;
//         //     }
//         // }

//         // printf("ooooooooooooooooooooooooooooooooooooooooooooooooo\n");
//         // printf("choose %d, round%d\n", index, i+1);
//         // printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
//         // printGraph(graph, n, true, true);

//         verTmp = graph[i];
//         graph[i] = graph[loc];
//         graph[loc] = verTmp;
//         order[i] = index;
//         // printf("indexx = %d\n", graph[i]->v_index );
//         t = clock();
//         bool bTmp = false;
//         int dTmp, fTmp;
//         // printVertex(graph[i]);
//         // printf("vertex %d, degree = %d, fill-in = %d\n", graph[i]->v_index, graph[i]->deg, graph[i]->fill_in);
//         if(graph[i]->fill_in > 0) {
//             bTmp = true;
//             dTmp = graph[i]->deg;
//             fTmp = graph[i]->fill_in;
//         }

//         eliminate(graph, n, i);
//         t_el += (clock() - t);
//         if(bTmp)
//         printf("vertex %d, degree = %d, fill-in = %d\nround%d : %lf\n", graph[i]->v_index, dTmp, fTmp, i, (double)(clock() - t) / CLOCKS_PER_SEC);
//         // if (min != 0)
//     }

//     printf("Time used to eliminate: %lf\n", (double)t_el/CLOCKS_PER_SEC );
//     // printf("end\n");
//     // printGraph(graph,n,true,true);

//     // for (i = 0 ; i < n ; i++) printf("%d ", order[i]);
//     // printf("\n");

// }

// spElement* quickSearch1(Matrix* mat, spElement* R, int r_index, int c_index, bool* exist) {
//     spElement *spTmp;
//     spElement *spPre, *spNext;

//     // row processing
//     for (spPre = R, spNext = R->NextInCol ; spNext != NULL && spNext->c_index < c_index ; spPre = spNext, spNext = spNext->NextInCol);
//     if (spNext == NULL) {
//         spTmp = (spElement*) malloc(sizeof(spElement));
//         spTmp->r_index = r_index; spTmp->c_index = c_index;
//         spTmp->NextInCol = spTmp->NextInRow = NULL;

//         spPre->NextInCol = spTmp;
//         fill_in++;
//     }
//     else if (spNext->c_index > c_index) {
//         spTmp = (spElement*) malloc(sizeof(spElement));
//         spTmp->r_index = r_index; spTmp->c_index = c_index;
//         spTmp->NextInCol = spTmp->NextInRow = NULL;

//         spPre->NextInCol = spTmp;
//         spTmp->NextInCol = spNext;
//         fill_in++;
//     }
//     else if (spNext->c_index == c_index) {
//         *exist = true;
//         free(spTmp);
//         return spNext;
//     }

//     if (mat->spcArr[c_index]->r_index > r_index) {
//         spNext = mat->spcArr[c_index];
//         mat->spcArr[c_index] = spTmp;
//         mat->spcArr[c_index]->NextInRow = spNext;
//     }
//     else {
//         if (c_index > r_index)   spPre = mat->spcArr[c_index];
//         else spPre = mat->Diag[c_index];

//         spNext = spPre->NextInRow;

//         for ( ; spNext != NULL ; spPre = spNext , spNext = spNext->NextInRow) {
//             if (spNext->r_index >= r_index) break;
//         }

//         if (spNext == NULL) spPre->NextInRow = spTmp;
//         else {
//             spPre->NextInRow = spTmp;
//             spTmp->NextInRow = spNext;
//         }
//     }

//     return spTmp;
// }

// void transpose(Matrix* mat) {
//     int i, tmp;
//     spElement *matTmp, *matTmp1;
//     for (i = 0 ; i < mat->size ; i++) {
//         for (matTmp = mat->sprArr[i] ; matTmp != NULL ; matTmp = matTmp1) {
//             matTmp1 = matTmp->NextInCol;
//             matTmp->NextInCol = matTmp->NextInRow;
//             matTmp->NextInRow = matTmp1;
//             tmp = matTmp->c_index;
//             matTmp->c_index = matTmp->r_index; matTmp->r_index = tmp;
//         }
//     }
//     for (i = 0 ; i < mat->size ; i++) {
//         matTmp = mat->spcArr[i];
//         mat->spcArr[i] = mat->sprArr[i];
//         mat->sprArr[i] = matTmp;
//     }
// }

// void Reorder(Matrix* mat, int* order) {
//     int i, n = mat->size;
//     int* tmp = (int*) malloc(sizeof(int) * n);
//     spElement** spTmp = (spElement**) malloc(sizeof(spElement*) * n);

//     swapRowOnce(mat, order);
//     swapColOnce(mat, order);

//     for (i = 0 ; i < n ; i++) {
//         tmp[i] = mat->ExtToIntRowMap[i];
//     }

//     for (i = 0 ; i < n ; i++) {
//         mat->ExtToIntRowMap[i] = tmp[order[i]];
//     }

//     for (i = 0 ; i < n ; i++) {
//         tmp[i] = mat->ExtToIntColMap[i];
//     }

//     for (i = 0 ; i < n ; i++) {
//         mat->ExtToIntColMap[i] = tmp[order[i]];
//     }

//     for (i = 0 ; i < n ; i++) {
//         mat->IntToExtRowMap[mat->ExtToIntRowMap[i]] = i;
//         mat->IntToExtColMap[mat->ExtToIntColMap[i]] = i;
//     }

//     for (i = 0 ; i < n ; i++) {
//         spTmp[i] = mat->Diag[i];
//     }
//     for (i = 0 ; i < n ; i++) {
//         mat->Diag[i] = spTmp[order[i]];
//     }

//     // free the allocated memory
//     free(tmp); free(spTmp);

// }

// //

// void bubble(int *a, int* track, int size) {
//     int i = 0, tmp ;
//     bool change = true;
//     for ( ; change == true ; ) {
//         change = false;
//         for (i = 0 ; i < size - 1 ; i++) {
//             if (a[i] > a[i + 1]) {
//                 tmp = a[i]; a[i] = a[i + 1];
//                 a[i + 1] = tmp;
//                 tmp = track[i]; track[i] = track[i + 1];
//                 track[i + 1] = tmp;
//                 change = true;
//             }
//         }
//     }

// }

// void swapColOnce(Matrix* mat, int* order) {
//     int i, j, size;
//     int n = mat->size;
//     int *track = (int*) malloc(sizeof(int) * n);
//     int* indices = (int*) malloc(sizeof(int) * n);
//     spElement* spTmp;
//     spElement **spRow = (spElement**) malloc(sizeof(spElement*) * n);


//     for ( i = 0 ; i < n ; i++) {
//         for (spTmp = mat->spcArr[order[i]] ; spTmp != NULL ; spTmp = spTmp->NextInRow) {
//             spTmp->c_index = i; // 4 0 1 2 3
//         }
//         spRow[i] = mat->spcArr[i]; // 0 1 2 3 4
//     }

//     for ( i = 0 ; i < n ; i++) {
//         mat->spcArr[i] = spRow[order[i]];
//     }


//     for ( i = 0 ; i < n ; i++ ) {
//         for (size = 0, spTmp = mat->sprArr[i] ; spTmp != NULL ; spTmp = spTmp->NextInCol, size++) {
//             spRow[size] = spTmp;
//             indices[size] = spTmp->c_index;
//         }
//         for (j = 0 ; j < size ; j++) track[j] = j;
//         bubble(indices, track, size);
//         mat->sprArr[i] = spRow[track[0]];
//         for (j = 0 ; j < size - 1 ; j++) {
//             spRow[track[j]]->NextInCol = spRow[track[j + 1]];
//         }
//         spRow[track[size - 1]]->NextInCol = NULL;
//     }

//     free(track); free(indices); free(spRow);
// }

// void swapRowOnce(Matrix* mat, int* order) {
//     int i, j, size;
//     int n = mat->size;
//     int *track = (int*) malloc(sizeof(int) * n);
//     int* indices = (int*) malloc(sizeof(int) * n);
//     spElement* spTmp;
//     spElement **spCol = (spElement**) malloc(sizeof(spElement*) * n);

//     for ( i = 0 ; i < n ; i++) {
//         for (spTmp = mat->sprArr[order[i]] ; spTmp != NULL ; spTmp = spTmp->NextInCol) {
//             spTmp->r_index = i; // 4 0 1 2 3
//         }
//         spCol[i] = mat->sprArr[i]; // 0 1 2 3 4
//     }

//     for ( i = 0 ; i < n ; i++) {
//         mat->sprArr[i] = spCol[order[i]];
//     }


//     for ( i = 0 ; i < n ; i++ ) {
//         for (size = 0, spTmp = mat->spcArr[i] ; spTmp != NULL ; spTmp = spTmp->NextInRow, size++) {
//             spCol[size] = spTmp;
//             indices[size] = spTmp->r_index;
//         }
//         for (j = 0 ; j < size ; j++) track[j] = j;
//         bubble(indices, track, size);
//         mat->spcArr[i] = spCol[track[0]];
//         for (j = 0 ; j < size - 1 ; j++) {
//             spCol[track[j]]->NextInRow = spCol[track[j + 1]];
//         }
//         spCol[track[size - 1]]->NextInRow = NULL;
//     }

//     free(track); free(indices); free(spCol);
// }

spElement* quickSearch(Matrix* mat, spElement* R, int r_index, int c_index) {
    spElement *spTmp;
    spElement *spPre, *spNext;

    spTmp = (spElement*) malloc(sizeof(spElement));
    spTmp->r_index = r_index; spTmp->c_index = c_index;
    spTmp->NextInCol = spTmp->NextInRow = NULL;

    // row processing
    for (spPre = R, spNext = R->NextInCol ; spNext != NULL && spNext->c_index < c_index ; spPre = spNext, spNext = spNext->NextInCol);
    if (spNext == NULL) {
        spPre->NextInCol = spTmp;
        fill_in++;
    }
    else if (spNext->c_index > c_index) {
        spPre->NextInCol = spTmp;
        spTmp->NextInCol = spNext;
        fill_in++;
    }
    else if (spNext->c_index == c_index) {
        free(spTmp);
        return spNext;
    }

    if (mat->spcArr[c_index]->r_index > r_index) {
        spNext = mat->spcArr[c_index];
        mat->spcArr[c_index] = spTmp;
        mat->spcArr[c_index]->NextInRow = spNext;
    }
    else {
        if (c_index > r_index)   spPre = mat->spcArr[c_index];
        else spPre = mat->Diag[c_index];

        spNext = spPre->NextInRow;

        for ( ; spNext != NULL ; spPre = spNext , spNext = spNext->NextInRow) {
            if (spNext->r_index >= r_index) break;
        }

        if (spNext == NULL) spPre->NextInRow = spTmp;
        else {
            spPre->NextInRow = spTmp;
            spTmp->NextInRow = spNext;
        }
    }

    return spTmp;
}

void MATsymFac(Matrix* mat) {
    int i, j, k;
    int n = mat->size;

    bool* rtr = (bool*) calloc(sizeof(bool), n);
    bool* ctr = (bool*) calloc(sizeof(bool), n);
    spElement *spRow, *spCol, *spTmp, *spTmp1;


    for ( spRow = mat->Diag[0]->NextInRow ; spRow != NULL ; spRow = spRow->NextInRow) {
        spTmp = spRow;
        for ( spCol = mat->Diag[0]->NextInCol ; spCol != NULL ; spCol = spCol->NextInCol) {
            spTmp = quickSearch(mat, spTmp, spRow->r_index, spCol->c_index);
        }
    }

    for (i = 1 ; i < n ; i++) {
        for ( j = mat->Diag[i]->r_index ; j < n ; j++) rtr[j] = false;
        for ( j = mat->Diag[i]->c_index ; j < n ; j++) ctr[j] = false;
        for ( spRow = mat->Diag[i - 1]; spRow != NULL ; spRow = spRow->NextInRow ) rtr[spRow->r_index] = true;
        for ( spCol = mat->Diag[i - 1]; spCol != NULL ; spCol = spCol->NextInCol ) ctr[spCol->c_index] = true;
        for ( spRow = mat->Diag[i]->NextInRow ; spRow != NULL ; spRow = spRow->NextInRow) {
            spTmp = spRow;
            for ( spCol = mat->Diag[i]->NextInCol ; spCol != NULL ; spCol = spCol->NextInCol) {
                if (!(ctr[spCol->c_index] && rtr[spRow->r_index]))
                    spTmp = quickSearch(mat, spTmp, spRow->r_index, spCol->c_index);
            }
        }
        // printf("Fill-in : %d\n",fill_in );
    }
}

