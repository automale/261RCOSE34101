#include "simulation.h"
#include "draw_gantt.h"
#define BATCH 100

int main(){
    int alg_cnt, alg_num, test_cnt;
    double metric[3], avg_metric[6][3];
    bool alg_list[6] = {false, };
    it_ptr tcase;
    char test_filename[100], sched_filename[100], new_session;
    char alg_name[6][10] = {"fcfs", "rr", "sjf", "pre_sjf", "prior", "pre_prior"};
    schq_ptr sq;

    srand(time(NULL));
    // test_generate

    while(1){
        system("rm -rf ./output_testcase ./output_sched");
        system("mkdir -p ./output_testcase");

        printf("========= OS SCHEDULER SIMULATOR =========\n");
        printf("This program can simulate FCFS, Round Robin, SJF,\nPreemptive SJF, Priority, Preemptive Priority algorithms\n\n");

        while(1){
            printf("Please select # of scheduling algorithm to compare. ( 1 ~ 6 )\n>> "); 
            if (scanf("%d", &alg_cnt) < 1 ) {
                printf("Invalid input please type decimal number\n");
                while (getchar() != '\n');
            }
            else if( alg_cnt > 6 || alg_cnt < 1 ) printf("Please select in the range of 1 ~ 6.\n");
            else break;
        }

        printf("\nAlgorithm : (1) FCFS (2) Round Robin (3) SJF (4) Preemptive SJF (5) Priority (6) Preemptive Priority\n\n");
        for(int i = 0; i < alg_cnt; i++){
            while(1){
                printf("Select %d%s algorithm >> ", (i + 1), (i == 0) ? "st" : (i == 1) ? "nd" : (i == 2) ? "rd" : "th" );
                if(scanf("%d", &alg_num) < 1) {
                    printf("Invalid input please type decimal number.\n");
                    while (getchar() != '\n');
                }
                else if(alg_num < 1 || alg_num > 6) printf("Please select in the range of 1 ~ 6.\n");
                else if(alg_list[alg_num - 1] == true) printf("%d had already been selected.\n", alg_num);
                else {alg_list[alg_num - 1] = true; break;}
            } 
            alg_list[alg_num-1] = true;
        }
        printf("\n");

        while(1){
            printf("Please choose # of test cases. (1 ~ )\n>> ");
            if (scanf("%d", &test_cnt) < 1 ) {
                printf("Invalid input please type decimal number.\n");
                while (getchar() != '\n');
            }
            else if(test_cnt < 1) printf("Please select more than 1 testcase.\n");
            else { break; }
        }

        for(sched_alg alg = fcfs; alg <= pre_prior; alg++){
            if(alg_list[alg] == true){
                switch(alg){
                    case fcfs:      system("mkdir -p ./output_sched/fcfs");      break;
                    case rr:        system("mkdir -p ./output_sched/rr");        break;
                    case sjf:       system("mkdir -p ./output_sched/sjf");       break;
                    case pre_sjf:   system("mkdir -p ./output_sched/pre_sjf");   break;
                    case prior:     system("mkdir -p ./output_sched/prior");     break;
                    case pre_prior: system("mkdir -p ./output_sched/pre_prior"); break;
                } 
            }
        }

        // initialize metric data before simulation
        for(int i = 0; i < 6; i++){
            for(int j = 0; j < 3; j++){
                avg_metric[i][j] = 0;
            }
        }

        // simulation for each test case
        for(int i = 0; i < test_cnt; i++){
            tcase = test_case_gen();
            sprintf(test_filename, "./output_testcase/testcase_%d.txt", i+1);
            print_test_case_to_file(tcase, test_filename);

            for(sched_alg alg = fcfs; alg <= pre_prior; alg++){
                if(alg_list[alg] == true){
                    sprintf(sched_filename, "./output_sched/%s/%s_gantt_%d.txt", alg_name[alg], alg_name[alg], i+1);
                    sq = sched_simulation(tcase, alg);
                    print_sched_to_file( sq, tcase, sched_filename, metric );

                    avg_metric[alg][0] += metric[0];
                    avg_metric[alg][1] += metric[1];
                    avg_metric[alg][2] += metric[2];

                    free_sched(sq);
                }
            }
            test_case_destroyer(tcase);
        }

        printf("\n########### METRIC SUMMARIZE ###########\n\n");
        printf("----------------------------------------------------------------------------\n");
        printf("| algorithm | cpu utilization | avg turnaround time | average waiting time |\n");
        printf("----------------------------------------------------------------------------\n");
        for(sched_alg alg = fcfs; alg <= pre_prior; alg++){
            if(alg_list[alg] == true){
                printf("| %9s | %15f | %19f | %20f |\n", \
                    alg_name[alg], avg_metric[alg][0] / test_cnt, avg_metric[alg][1] / test_cnt, avg_metric[alg][2] / test_cnt);
            }
        }
        printf("----------------------------------------------------------------------------\n");
        printf("\n########### Simulation terminated ###########\n\n");

        // algorithm selection initialize
        for(sched_alg alg = 0; alg < 6; alg++){alg_list[alg] = false;}

        printf("PRESS Y to remove output files and start a new session\n");
        printf("and PRESS T to terminate the simulator.\n");
        while(1){
            while (getchar() != '\n');
            printf(">> ");
            if ( scanf("%c", &new_session) < 1 ){
                while (getchar() != '\n');
            }
            else if(new_session == 'y' || new_session == 'Y') break;
            else if(new_session == 't' || new_session == 'T') {
                return 0;
            }
        }
        printf("\n");
    }

    return 0;
}