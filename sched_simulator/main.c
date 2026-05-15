#include "simulation.h"
#include "draw_gantt.h"

int main(){
    srand(time(NULL));
    // test_generate
    for(int i = 0; i < 1000; i++){
        it_ptr tcase = test_case_gen();
        char test_filename[100];
        char gantt_filename[100];
        // test case gen
        sprintf(test_filename, "./output_testcase/testcase_%d.txt", i);
        print_test_case_to_file(tcase, test_filename);
        // testing...
        sprintf(gantt_filename, "./output_gantt/%s/gantt_%d.txt", "fcfs", i);
        print_gantt_to_file(sched_simulation(tcase, fcfs), gantt_filename);

        sprintf(gantt_filename, "./output_gantt/%s/gantt_%d.txt", "rr", i);
        print_gantt_to_file(sched_simulation(tcase, rr), gantt_filename);

        sprintf(gantt_filename, "./output_gantt/%s/gantt_%d.txt", "sjf", i);
        print_gantt_to_file(sched_simulation(tcase, sjf), gantt_filename);

        sprintf(gantt_filename, "./output_gantt/%s/gantt_%d.txt", "pre_sjf", i);
        print_gantt_to_file(sched_simulation(tcase, pre_sjf), gantt_filename)
        ;
        sprintf(gantt_filename, "./output_gantt/%s/gantt_%d.txt", "prior", i);
        print_gantt_to_file(sched_simulation(tcase, prior), gantt_filename);

        sprintf(gantt_filename, "./output_gantt/%s/gantt_%d.txt", "pre_prior", i);
        print_gantt_to_file(sched_simulation(tcase, pre_prior), gantt_filename);
        // test finish
        test_case_destroyer(tcase);
        printf("%d case passed\n", i + 1);
    }
    
    // for each test case run scheduler and store the result in gant data structure

    // for each gant data structure draw chart

    // 

    return 0;
}