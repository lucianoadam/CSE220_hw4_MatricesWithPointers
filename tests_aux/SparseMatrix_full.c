#include "../tests/unit_tests.h"
#include "global.h"
int main() {
    int** M = initializeMatrix(4, 2, (int[]){0,4,0,5,0,0,2,0});
    int** S_act = initializeMatrix(3, 4, (int[]){0,0,0,0,0,0,0,0,0,0,0,0});
    int D[2] = {4,2};
    SparseMatrix(M, S_act, D);
    freeMatrix(4, M);
    freeMatrix(3, S_act);
    return 0;
}
