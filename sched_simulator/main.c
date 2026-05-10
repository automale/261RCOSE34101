#include "simulation.h"

int main(){
    // test_generate
    it_ptr tcase = test_case_gen();
    print_test_case_to_file(tcase, "output_01.txt");
    test_case_destroyer(tcase);
    
    // for each test case run scheduler and store the result in gant data structure

    // for each gant data structure draw chart

    // 

    return 0;
}