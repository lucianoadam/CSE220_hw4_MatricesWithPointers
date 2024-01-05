#include "../tests/unit_tests.h"
#include "global.h"
int main() { 
    int** M = initializeMatrix(3, 2, (int[]){ 2, 3, 1, 3, 4, 5});
    int** N = initializeMatrix(3, 2, (int[]){3, 3, 7, 2, 1,5});
    int** A_act = initializeMatrix(3, 2, (int[]){0, 0, 0, 0, 0,0});
    int D[6] = {3,2,3,2,3,2};
    Addition(M, N, A_act, D);
    freeMatrix(3, M); freeMatrix(3, N); freeMatrix(3, A_act);
    return 0;
}
