#include "data_struct.h"

int compare_arrival(const void *a, const void *b);

void sort_test_case_by_arrival(it_ptr tcase);

it_ptr test_case_gen();

int test_case_destroyer(it_ptr tcase);

int print_test_case_to_file(it_ptr tcase, const char* filename);