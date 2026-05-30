#include "draw_gantt.h"

// 간트 차트 생성 및 파일 출력 함수
void print_sched_to_file(schq_ptr queue, it_ptr tcase, const char *filename, double *metric) {
    if (queue == NULL || queue->front == NULL) {
        printf("NO schedule data to print out.\n");
        return;
    }

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("file open failed.");
        return;
    }

    unsigned total_time = 0;
    unsigned idle_time = 0;
    
    // PID를 인덱스로 사용하여 프로세스별 최종 완료 시간(Completion Time)을 추적할 배열
    unsigned *comp_time = malloc(tcase->size * sizeof(unsigned)); 
    
    sch_ptr temp = queue->front;
    while (temp != NULL) {
        if (temp->pid == (unsigned)-1) {
            idle_time += (temp->end - temp->start); // IDLE time accumulation
        } else {
            comp_time[temp->pid] = temp->end; // termination time check
        }
        total_time = temp->end; // last sched node's end time is total end time...
        temp = temp->next;
    }

    double cpu_util = (total_time > 0) ? ((double)(total_time - idle_time) / total_time) * 100.0 : 0.0;
    double total_tat = 0.0;
    double total_wt = 0.0;

    for (unsigned i = 0; i < tcase->size; i++) {
        unsigned pid = tcase->task_list[i].pid;
        unsigned arr = tcase->task_list[i].arrival;
        unsigned burst = tcase->task_list[i].burst;

        unsigned io_total = 0;
        for (unsigned j = 0; j < tcase->task_list[i].io_num; j++) {
            io_total += tcase->task_list[i].io_list[j].duration; // total I/O waiting time
        }

        // Turnaround Time = termination time - arrival time
        unsigned tat = comp_time[pid] - arr;
        // Waiting Time = turnaround time - burst - I/O waiting time
        unsigned wt = tat - burst - io_total;

        total_tat += tat;
        total_wt += wt;
    }

    double avg_tat = total_tat / tcase->size;
    double avg_wt = total_wt / tcase->size;
    
    free(comp_time); 
    // metric computation ends

    fprintf(fp, "====================================================\n");
    fprintf(fp, "               OS Scheduler Gantt Chart             \n");
    fprintf(fp, "====================================================\n\n");

    // 1. gantt chart print
    fprintf(fp, "Visual Timeline:\n");
    fprintf(fp, "----------------------------------------------------\n");
    
    sch_ptr curr = queue->front;
    // 1st line : | (start time) Pn  | ..... | (start time) Pm  |(end of gantt : ~~~)  
    fprintf(fp, "|");
    while (curr != NULL) {
        if (curr->pid == (unsigned)(-1)) {
            fprintf(fp, "(%d)  IDLE   |", curr->start);
        } else {
            fprintf(fp, "(%d)  P%-3d  |", curr->start, curr->pid);
        }
        if(curr->next == NULL){
            fprintf(fp, "(end of gantt : %d)", curr->end);
        }
        curr = curr->next;
    }
    fprintf(fp, "\n");

    fprintf(fp, "----------------------------------------------------\n\n");

    // 2. table form scheduling print out
    fprintf(fp, "Detailed Execution Log:\n");
    fprintf(fp, "%-10s | %-10s | %-10s | %-10s\n", "Sequence", "PID", "Start", "End");
    fprintf(fp, "----------------------------------------------------\n");

    curr = queue->front;
    int seq = 1;
    while (curr != NULL) {
        if (curr->pid == (unsigned)-1) {
            fprintf(fp, "[%02d]       | %-10s | %-10u | %-10u\n", seq++, "IDLE", curr->start, curr->end);
        } else {
            fprintf(fp, "[%02d]       | P%-9u | %-10u | %-10u\n", seq++, curr->pid, curr->start, curr->end);
        }
        curr = curr->next;
    }

    fprintf(fp, "----------------------------------------------------\n\n");

    fprintf(fp, "====================================================\n");
    fprintf(fp, "                 PERFORMANCE METRICS                \n");
    fprintf(fp, "====================================================\n");
    fprintf(fp, "- Total Time        : %u ticks\n", total_time);
    fprintf(fp, "- CPU Utilization   : %.2f %%\n", cpu_util);
    fprintf(fp, "- Avg Turnaround    : %.2f ticks\n", avg_tat);
    fprintf(fp, "- Avg Waiting Time  : %.2f ticks\n", avg_wt);
    fprintf(fp, "====================================================\n");

    fprintf(fp, "End of Simulation\n");

    fclose(fp);
    printf("sched (gantt, metric) chart drawing finished : output file = '%s\n", filename);
    
    metric[0] = cpu_util;
    metric[1] = avg_tat;
    metric[2] = avg_wt;
}