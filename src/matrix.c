#include "../include/global.h"

int SparseMatrix(int** M, int** S, int* D){
    const int LARGEST_DIMENSION = *D > *(D + 1) ? *D : *(D + 1);
    if (LARGEST_DIMENSION <= 0) return -1; // making sure largest dimension is a positive int
    int row, column, count = 0;
    for (row = 0; row < *(D + 1); row++){
        for (column = 0; column < *D; column++){
            const int VALUE = *(*(M + row) + column);
            if (VALUE != 0){
                *(*S + count) = row;
                *(*(S + 1) + count) = column;
                *(*(S + 2) + count) = VALUE;
                if (++count > LARGEST_DIMENSION) return -1;
            }
        }
    }
    int i;
    if (count < LARGEST_DIMENSION){
        for (i = count; i < LARGEST_DIMENSION; i++){
            *(*S + i) = 0;
            *(*(S + 1) + i) = 0;
            *(*(S + 2) + i) = 0;
        }
    }
    return count;
}

int Addition(int** M, int** N, int** A, int* D){
    int row, column;
    // Initializing result to 0
    for (row = 0; row < *(D + 4); row++)
        for (column = 0; column < *(D + 5); column++)
            *(*(A + row) + column) = 0;
    const int ROW_LIMIT    = *D <= *(D + 2) ? *D : *(D + 2),
              COLUMN_LIMIT = *(D + 1) <= *(D + 3) ? *(D + 1) : *(D + 3);
    char flag = 0;
    for (row = 0; row < ROW_LIMIT; row++){
        if (row >= *(D + 4)){
            flag = 1;
            break;
        }
        for (column = 0; column < COLUMN_LIMIT; column++){
            if (column >= *(D + 5)){
                flag = 1;
                break;
            }
            *(*(A + row) + column) = *(*(M + row) + column) + *(*(N + row) + column);
        }
    }
    if (flag){
        if (*(D + 1) == *(D + 3) && *D == *(D + 2)) return -3;
        return -2;
    }
    if (*D != *(D + 2) || *(D + 1) != *(D + 3)) return -1;
    if (ROW_LIMIT == *(D + 4) && COLUMN_LIMIT == *(D + 5)) return 1;
    return 2;
}

int Multiplication(int** M, int** N, int** A, int* D){
    int row, column, i;
    // Initializing result to 0
    for (row = 0; row < *(D + 4); row++)
        for (column = 0; column < *(D + 5); column++)
            *(*(A + row) + column) = 0;
    const int ROW_LIMIT    = *(D + 4) >= *D ? *D : *(D + 4),
              COLUMN_LIMIT = *(D + 5) >= *(D + 3) ? *(D + 3) : *(D + 5);
    for (row = 0; row < ROW_LIMIT; row++)
        for (column = 0; column < COLUMN_LIMIT; column++)
            for (i = 0; i < *(D + 1); i++)
                *(*(A + row) + column) += *(*(M + row) + i) * *(*(N + i) + column);
    return *(D + 4) >= *D && *(D + 5) >= *(D + 3) ? 
                *(D + 1) != *(D + 2) ? -1 : 
                *(D + 4) == *D && *(D + 5) == *(D + 3) ? 1 : 2 : 
           *(D + 1) != *(D + 2) ? -2 : -3;
}

int Transpose(int** A, int** AT, int* D){
    int row, column;
    // Initializing result to 0
    for (row = 0; row < *(D + 2); row++)
        for (column = 0; column < *(D + 3); column++)
            *(*(AT + row) + column) = 0;
    char flag = *D == *(D + 3) && *(D + 1) == *(D + 2) ? 1 : 2;
    for (row = 0; row < *D; row++){
        if (row >= *(D + 3)){
            flag = -1;
            break;
        }
        for (column = 0; column < *(D + 1); column++){
            if (column >= *(D + 2)){
                flag = -1;
                break;
            }
            *(*(AT + column) + row) = *(*(A + row) + column);
        }
    }
    return flag;
}

// Helper functions for Testing
int** initializeMatrix(int n, int m, int* values){
    int** ret = (int**)malloc(n*sizeof(int*));
    for(int i=0; i<n; i++){
        ret[i] = (int*)malloc(m*sizeof(int));
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            ret[i][j] = values[i*m+j];
        }
    }
    return ret;
}

void freeMatrix(int n, int** M){
    for(int i=0; i<n; i++){
        free(M[i]);
    }
    free(M);
}
