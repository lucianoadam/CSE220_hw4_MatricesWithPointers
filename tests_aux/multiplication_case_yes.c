#include "../tests/unit_tests.h"
#include "global.h"
int main() { 
    int** M = initializeMatrix(3, 2, (int[]){ 2, 3, 5, 4, 2, 3});
    int** N = initializeMatrix(2, 3, (int[]){3, 2, 1, 1, 3, 4});
    int** A_act = initializeMatrix(3, 3, (int[]){0, 0, 0, 0, 0, 0, 0, 0, 0});
    int D[6] = {3,2,2,3,3,3};
    Multiplication(M, N, A_act, D);
    freeMatrix(3, M); freeMatrix(2, N); freeMatrix(3, A_act);
    return 0;
}
