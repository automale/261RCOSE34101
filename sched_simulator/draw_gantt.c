#include "draw_gantt.h"

// 간트 차트 생성 및 파일 출력 함수
void print_gantt_to_file(schq_ptr queue, const char *filename) {
    if (queue == NULL || queue->front == NULL) {
        printf("출력할 간트 데이터가 없습니다.\n");
        return;
    }

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("파일 열기 실패");
        return;
    }

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

    fprintf(fp, "----------------------------------------------------\n");
    fprintf(fp, "End of Simulation\n");

    fclose(fp);
    printf("gantt chart drawing finished : output file = '%s\n", filename);
}