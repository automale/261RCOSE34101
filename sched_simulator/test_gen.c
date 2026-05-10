#include "test_gen.h"

// qsort comparator function
int compare_arrival(const void *a, const void *b) {
    // common_task pointer casting
    const common_task *taskA = (const common_task *)a;
    const common_task *taskB = (const common_task *)b;

    if (taskA->arrival < taskB->arrival) return -1; // A arrives eariler
    if (taskA->arrival > taskB->arrival) return 1;  // B arrives eariler
    
    // arrival time tie breaker (pid increasing order)
    if (taskA->pid < taskB->pid) return -1;
    if (taskA->pid > taskB->pid) return 1;
    
    return 0;
}

// task_list sort
void sort_test_case_by_arrival(it_ptr tcase) {
    // exception handling
    if (tcase == NULL || tcase->task_list == NULL || tcase->size == 0) {
        return; 
    }

    // qsort(array addr, element #, element size, compare function)
    qsort(tcase->task_list, tcase->size, sizeof(common_task), compare_arrival);
}

it_ptr test_case_gen(){
    srand(time(NULL));
    unsigned mdegree, Mextime_per_task, max_arrival;
    mdegree = rand() % (MAX_DEGREE_MULTI - MIN_DEGREE_MULTI) + MIN_DEGREE_MULTI;
    Mextime_per_task = (rand() % (MAX_TOTAL_EXTIME - MIN_TOTAL_EXTIME) + MIN_TOTAL_EXTIME) / mdegree;
    max_arrival = Mextime_per_task * (rand() % mdegree);

    it_ptr tcase = malloc(sizeof(input_tasks));
    tcase->task_list = malloc(sizeof(common_task) * mdegree);
    tcase->size = mdegree;
    tcase->next_arrival_idx = 0;
    for(int id = 0; id < (int)mdegree; id++){
        unsigned io_partition, io_posb_mod, task_burst;
        // pid & arrival & remaining(cpu burst) setting
        tcase->task_list[id].pid = id;
        tcase->task_list[id].arrival = rand() % max_arrival;
        task_burst = tcase->task_list[id].burst = rand() % Mextime_per_task + 1;
        tcase->task_list[id].remaining = task_burst;
        // priority setting
        tcase->task_list[id].priority = rand() % MAX_PRIOR;
        // io info setting
        tcase->task_list[id].current_io_idx = 0;
        tcase->task_list[id].current_io_remaining = 0;
        // generate io events
        io_partition = rand() % task_burst + 1;
        io_posb_mod = rand() % MAX_IO_POSB_MOD + 1;
        
        io_ptr temp_io_list = malloc(sizeof(io_config) * io_partition);
        unsigned io_idx = 0;
        for(unsigned i = 0; i < io_partition; i++){
            if(!(rand() % io_posb_mod)){
                temp_io_list[io_idx].io_time = ((i * task_burst) / io_partition) + (rand() % (task_burst / io_partition));  
                temp_io_list[io_idx].device_num = rand() % DEVICES;
                temp_io_list[io_idx].duration = rand() % MAX_IO_DURATION;
                io_idx++;
            }
        }
        tcase->task_list[id].io_num = io_idx;
        tcase->task_list[id].io_list = realloc(temp_io_list, sizeof(io_config) * tcase->task_list[id].io_num);
    }
    // sort tasks in increasing order, key = (arrival_time, pid)
    sort_test_case_by_arrival(tcase);

    return tcase;
}

int test_case_destroyer(it_ptr tcase){
    if (tcase == NULL) {
        return -1;
    }
    if (tcase->task_list != NULL) {
        for(unsigned i = 0; i < tcase->size; i++){free(tcase->task_list[i].io_list);}
        free(tcase->task_list);
    }
    free(tcase);
    return 0;
}

int print_test_case_to_file(it_ptr tcase, const char* filename) {
    // 1. 예외 처리 방어 코드
    if (tcase == NULL || tcase->task_list == NULL) {
        printf("Error: Test case is empty or NULL.\n");
        return -1;
    }

    // 2. 파일 열기 (쓰기 모드)
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: Failed to open file '%s' for writing.\n", filename);
        return -1;
    }

    fprintf(fp, "========================================================================================\n");
    fprintf(fp, "                               OS SCHEDULER TEST CASE DATA.                             \n");
    fprintf(fp, "========================================================================================\n\n");

    // =======================================================================
    // Section 1: Task-centric view (도착 시간 순서대로 정렬된 태스크 출력)
    // =======================================================================
    fprintf(fp, "----------------------------------------------------------------------------------------\n");
    fprintf(fp, "[SECTION 1] TASK-CENTRIC VIEW (Sorted by Arrival Time)\n");
    fprintf(fp, "----------------------------------------------------------------------------------------\n");
    
    for (unsigned i = 0; i < tcase->size; i++) {
        common_task *t = &tcase->task_list[i];
        
        fprintf(fp, "Task [PID: %3u] | Arrival: %4u | CPU Burst: %4u | I/O Count: %2u | Priority : %3u\n", \
                t->pid, t->arrival, t->remaining, t->io_num, t->priority);
        
        if (t->io_num > 0) {
            fprintf(fp, "    -> I/O Requests:\n");
            for (unsigned j = 0; j < t->io_num; j++) {
                io_config *io = &t->io_list[j];
                // io_time은 해당 프로세스가 CPU를 점유한 누적 시간 기준입니다.
                fprintf(fp, "       - Request at CPU Time %4u : Device %2u (Duration: %4u ms)\n", \
                        io->io_time, io->device_num, io->duration);
            }
        }
        fprintf(fp, "\n");
    }

    // =======================================================================
    // Section 2: Device-centric view (디바이스별 대기 큐 예상 요청 목록)
    // =======================================================================
    fprintf(fp, "----------------------------------------------------------------------------------------\n");
    fprintf(fp, "[SECTION 2] DEVICE-CENTRIC VIEW (Expected Requests per Device)\n");
    fprintf(fp, "----------------------------------------------------------------------------------------\n");
    
    // DEVICES 매크로(10)를 기준으로 각 디바이스에 꽂힐 I/O 요청을 긁어모읍니다.
    for (unsigned dev = 0; dev < DEVICES; dev++) {
        int has_req = 0;
        fprintf(fp, "[Device %2u]\n", dev);

        for (unsigned i = 0; i < tcase->size; i++) {
            common_task *t = &tcase->task_list[i];
            
            for (unsigned j = 0; j < t->io_num; j++) {
                if (t->io_list[j].device_num == dev) {
                    if (!has_req) {
                        // 해당 디바이스에 요청이 하나라도 있으면 헤더를 출력합니다.
                        fprintf(fp, "    Req PID | Req CPU Time | Duration \n");
                        fprintf(fp, "   ---------+--------------+----------\n");
                        has_req = 1;
                    }
                    fprintf(fp, "      %3u   |     %4u     |   %4u   \n", \
                            t->pid, t->io_list[j].io_time, t->io_list[j].duration);
                }
            }
        }
        
        // 해당 디바이스를 호출하는 프로세스가 하나도 없을 경우
        if (!has_req) {
            fprintf(fp, "    (No I/O requests for this device)\n");
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "========================================================================================\n");
    fprintf(fp, "                                      END OF DATA                                       \n");
    fprintf(fp, "========================================================================================\n");

    // 3. 파일 닫기
    fclose(fp);
    printf("Successfully wrote test case to '%s'\n", filename);
    
    return 0;
}