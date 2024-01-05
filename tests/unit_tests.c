/*
DO NOT CHANGE THE CONTENTS OF THIS FILE IN CASE A NEW VERSION IS DISTRIBUTED.
PUT YOUR OWN TEST CASES IN student_tests.c
*/

#include "global.h"
#include "unit_tests.h"

static char test_log_outfile[100];

int run_using_system(char *test_name) {
    char executable[100];
    sprintf(executable, "./bin/%s", test_name);
    assert(access(executable, F_OK) == 0);

    char cmd[500];
    sprintf(test_log_outfile, "%s/%s.log", TEST_OUTPUT_DIR, test_name);
#if defined(__linux__)
    sprintf(cmd, "valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes --error-exitcode=37 ./bin/%s > %s 2>&1",
	    test_name, test_log_outfile);
#else
    cr_log_warn("Skipping valgrind tests. Run tests on Linux or GitHub for full output.\n");
    sprintf(cmd, "./bin/%s > %s 2>&1", test_name, test_log_outfile);
#endif
    return system(cmd);
}

void assert_normal_exit(int status) {
    cr_expect_eq(status, 0, "The program did not exit normally (status = 0x%x).\n", status);
}

void assert_error_exit(int status) {
    cr_expect_eq(WEXITSTATUS(status), 0xff,
		 "The program did not exit with status 0xff (status was 0x%x).\n", status);
}

void assert_no_valgrind_errors(int status) {
    cr_expect_neq(WEXITSTATUS(status), 37, "Valgrind reported errors -- see %s", test_log_outfile);
    if (WEXITSTATUS(status) == 37) {
        char cmd[200];
        sprintf(cmd, "cat %s", test_log_outfile);
        system(cmd);
    }
}

TestSuite(base_suite, .timeout=TEST_TIMEOUT);

bool areMatricesEqual(int** matrix1, int** matrix2, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix1[i][j] != matrix2[i][j]) {
                return false;  // Matrices have different elements.
            }
        }
    }
    return true;  // Matrices are equal.
}
Test(base_suite, SparseMatrix_full, .description="Full Sparse Matrix") {
    int** M = initializeMatrix(4, 2, (int[]){0,4,0,5,0,0,2,0});
    int** S_act = initializeMatrix(3, 4, (int[]){0,0,0,0,0,0,0,0,0,0,0,0});
    int** S_exp = initializeMatrix(3, 4, (int[]){0,1,3,0,1,1,0,0,4,5,2,0});
    int D[2] = {2,4};
    int ret_act = SparseMatrix(M, S_act, D);
    int ret_exp = 3;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(S_act, S_exp, 3, 4), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(4, M); freeMatrix(3, S_act); freeMatrix(3, S_exp);
}
Test(base_suite, SparseMatrix_full_valgrind) {
    assert_no_valgrind_errors(run_using_system("SparseMatrix_full"));
}

Test(base_suite, addition_case_yes, .description="Matrices are compatible for addition") {
    int** M = initializeMatrix(3, 2, (int[]){ 2, 3, 1, 3, 4, 5});
    int** N = initializeMatrix(3, 2, (int[]){3, 3, 7, 2, 1,5});
    int** A_act = initializeMatrix(3, 2, (int[]){0, 0, 0, 0, 0,0});
    int** A_exp = initializeMatrix(3, 2, (int[]){5, 6, 8, 5, 5,10});
    int D[6] = {3,2,3,2,3,2};
    int ret_act = Addition(M, N, A_act, D);
    int ret_exp = 1;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 3, 2), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(3, N); freeMatrix(3, A_act); freeMatrix(3, A_exp);
}
Test(base_suite, addition_case_yes_valgrind) {
    assert_no_valgrind_errors(run_using_system("addition_case_yes"));
}
Test(base_suite, addition_case_no, .description="Matrices are compatible for addition, but not A. A also contains garbage values") {
    int** M = initializeMatrix(3, 2, (int[]){ 2, 3, 1, 3, 4, 5});
    int** N = initializeMatrix(3, 2, (int[]){3, 3, 7, 2, 1,5});
    int** A_act = initializeMatrix(3, 3, (int[]){5, 6, 0, 8, 5, 0, 5, 10, 0});
    int** A_exp = initializeMatrix(3, 3, (int[]){5, 6, 0, 8, 5, 0, 5, 10, 0});
    int D[6] = {3,2,3,2,3,3};
    int ret_act = Addition(M, N, A_act, D);
    int ret_exp = 2;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 3, 3), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(3, N); freeMatrix(3, A_act); freeMatrix(3, A_exp);
}
Test(base_suite, addition_case_no_valgrind) {
    assert_no_valgrind_errors(run_using_system("addition_case_no"));
}
Test(base_suite, addition_case_no_small, .description="Matrices are incompatible for addition, and A has little space") {
    int** M = initializeMatrix(3, 2, (int[]){ 2, 3, 1, 3, 4, 5});
    int** N = initializeMatrix(4, 2, (int[]){5, 6, 8, 5, 5, 10, 5,10});
    int** A_act = initializeMatrix(2, 2, (int[]){0, 0, 0, 0});
    int** A_exp = initializeMatrix(2, 2, (int[]){7, 9, 9, 8});
    int D[6] = {3,2,4,2,2,2};
    int ret_act = Addition(M, N, A_act, D);
    int ret_exp = -2;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 2, 2), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(4, N); freeMatrix(2, A_act); freeMatrix(2, A_exp);
}
Test(base_suite, addition_case_no_big, .description="Matrices are incompatible for addition, and A has enough space") {
    int** M = initializeMatrix(3, 2, (int[]){ 2, 3, 1, 3, 4, 5});
    int** N = initializeMatrix(4, 2, (int[]){5, 6, 8, 5, 5, 10, 5,10});
    int** A_act = initializeMatrix(5, 2, (int[]){0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    int** A_exp = initializeMatrix(5, 2, (int[]){7, 9, 9, 8, 9, 15, 0, 0, 0, 0});
    int D[6] = {3,2,4,2,5,2};
    int ret_act = Addition(M, N, A_act, D);
    int ret_exp = -1;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 5, 2), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(4, N); freeMatrix(5, A_act); freeMatrix(5, A_exp);
}
Test(base_suite, addition_case_yes_small, .description="Matrices are compatible, but A is small") {
    int** M = initializeMatrix(3, 2, (int[]){ 2, 3, 1, 3, 4, 5});
    int** N = initializeMatrix(3, 2, (int[]){3, 3, 7, 2, 1, 5});
    int** A_act = initializeMatrix(2, 2, (int[]){0, 0, 0, 0});
    int** A_exp = initializeMatrix(2, 2, (int[]){5, 6, 8, 5});
    int D[6] = {3,2,3,2,2,2};
    int ret_act = Addition(M, N, A_act, D);
    int ret_exp = -3;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 2, 2), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(3, N); freeMatrix(2, A_act); freeMatrix(2, A_exp);
}
Test(base_suite, multiplication_case_yes, .description="Matrices are compatible for multiplication") {
    int** M = initializeMatrix(3, 2, (int[]){ 2, 3, 5, 4, 2, 3});
    int** N = initializeMatrix(2, 3, (int[]){3, 2, 1, 1, 3, 4});
    int** A_act = initializeMatrix(3, 3, (int[]){0, 0, 0, 0, 0, 0, 0, 0, 0});
    int** A_exp = initializeMatrix(3, 3, (int[]){9, 13, 14, 19, 22, 21, 9, 13, 14});
    int D[6] = {3,2,2,3,3,3};
    int ret_act = Multiplication(M, N, A_act, D);
    int ret_exp = 1;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 3, 3), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(2, N); freeMatrix(3, A_act); freeMatrix(3, A_exp);
}
Test(base_suite, multiplication_case_yes_valgrind) {
    assert_no_valgrind_errors(run_using_system("multiplication_case_yes"));
}
Test(base_suite, multiplication_case_no_small, .description="Matrices are incompatible for multiplicaiton, and A is small") {
    int** M = initializeMatrix(3, 2, (int[]){ 2, 3, 5, 4, 2, 3});
    int** N = initializeMatrix(3, 2, (int[]){3, 3, 7, 2, 1, 5});
    int** A_act = initializeMatrix(2, 2, (int[]){0, 0, 0, 0});
    int** A_exp = initializeMatrix(2, 2, (int[]){27, 12, 43, 23});
    int D[6] = {3,2,3,2,2,2};
    int ret_act = Multiplication(M, N, A_act, D);
    int ret_exp = -2;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 2, 2), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(3, N); freeMatrix(2, A_act); freeMatrix(2, A_exp);
}
Test(base_suite, multiplication_case_no_big, .description="Matrices are incompatible for multiplicaiton, but A is big") {
    int** M = initializeMatrix(3, 2, (int[]){ 2, 3, 5, 4, 2, 3});
    int** N = initializeMatrix(3, 2, (int[]){3, 3, 7, 2, 1, 5});
    int** A_act = initializeMatrix(4, 3, (int[]){0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    int** A_exp = initializeMatrix(4, 3, (int[]){27,12,0,43,23,0,27,12,0,0,0,0});
    int D[6] = {3,2,3,2,4,3};
    int ret_act = Multiplication(M, N, A_act, D);
    int ret_exp = -1;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 4, 3), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(3, N); freeMatrix(4, A_act); freeMatrix(4, A_exp);
}
Test(base_suite, multiplication_case_yes_big, .description="Matrices are compatible for multiplicaiton, but A is big") {
    int** M = initializeMatrix(3, 2, (int[]){ 2, 3, 5, 4, 2, 3});
    int** N = initializeMatrix(2, 3, (int[]){3, 2, 1, 1, 3, 4});
    int** A_act = initializeMatrix(4, 4, (int[]){0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0});
    int** A_exp = initializeMatrix(4, 4, (int[]){9,13,14,0,19,22,21,0,9,13,14,0,0,0,0,0});
    int D[6] = {3,2,2,3,4,4};
    int ret_act = Multiplication(M, N, A_act, D);
    int ret_exp = 2;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 4, 4), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(2, N); freeMatrix(4, A_act); freeMatrix(4, A_exp);
}
Test(base_suite, multiplication_case_yes_incorrect, .description="Matrices are compatible for multiplicaiton, but A is incorrect") {
    int** M = initializeMatrix(3, 2, (int[]){ 2, 3, 5, 4, 2, 3});
    int** N = initializeMatrix(2, 3, (int[]){3, 2, 1, 1, 3, 4});
    int** A_act = initializeMatrix(2, 4, (int[]){0, 0, 0, 0, 0, 0, 0, 0});
    int** A_exp = initializeMatrix(2, 4, (int[]){9,13,14,0,19,22,21,0});
    int D[6] = {3,2,2,3,2,4};
    int ret_act = Multiplication(M, N, A_act, D);
    int ret_exp = -3;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 2, 4), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(2, N); freeMatrix(2, A_act); freeMatrix(2, A_exp);
}
Test(base_suite, transpose_case_yes, .description="Matrices are compatible for transpose") {
    int** M = initializeMatrix(3, 2, (int[]){9, 9, 1, 2, 9, 3});
    int** A_act = initializeMatrix(2, 3, (int[]){0, 0, 0, 0, 0, 0});
    int** A_exp = initializeMatrix(2, 3, (int[]){9, 1, 9, 9, 2, 3});
    int D[4] = {3,2,2,3};
    int ret_act = Transpose(M, A_act, D);
    int ret_exp = 1;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 2, 3), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(2, A_act); freeMatrix(2, A_exp);
}
Test(base_suite, transpose_case_yes_valgrind) {
    assert_no_valgrind_errors(run_using_system("transpose_case_yes"));
}
Test(base_suite, transpose_case_no, .description="Matrices are incompatible for transpose") {
    int** M = initializeMatrix(3, 2, (int[]){9, 9, 1, 2, 9, 3});
    int** A_act = initializeMatrix(3, 2, (int[]){0, 0, 0, 0, 0, 0});
    int** A_exp = initializeMatrix(3, 2, (int[]){9, 1, 9, 2, 0, 0});
    int D[4] = {3,2,3,2};
    int ret_act = Transpose(M, A_act, D);
    int ret_exp = -1;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 3, 2), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(3, A_act); freeMatrix(3, A_exp);
}
Test(base_suite, transpose_case_no_valgrind) {
    assert_no_valgrind_errors(run_using_system("transpose_case_no"));
}
Test(base_suite, transpose_case_no_big, .description="Matrices are incompatible for transpose") {
    int** M = initializeMatrix(3, 2, (int[]){9, 9, 1, 2, 9, 3});
    int** A_act = initializeMatrix(4, 4, (int[]){0, 0, 0, 0, 0, 0,0,0,0,0,0,0});
    int** A_exp = initializeMatrix(4, 4, (int[]){9, 1, 9, 0, 9,2,3,0,0,0,0,0});
    int D[4] = {3,2,4,4};
    int ret_act = Transpose(M, A_act, D);
    int ret_exp = 2;
    cr_expect_eq(ret_act, ret_exp, "Return value was %d, but it should have been %d.\n", ret_act, ret_exp);
    cr_assert(areMatricesEqual(A_act, A_exp, 4, 4), "Contents of matrix incorrect. See unit_tests.c for expected values.");
    freeMatrix(3, M); freeMatrix(4, A_act); freeMatrix(4, A_exp);
}
/*
DO NOT CHANGE THE CONTENTS OF THIS FILE IN CASE A NEW VERSION IS DISTRIBUTED.
PUT YOUR OWN TEST CASES IN student_tests.c
*/
