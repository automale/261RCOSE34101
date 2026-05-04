// data_stuct.h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define DEGREE_MULTI 20
#define MAX_IO_PER_TASK 10
#define MAX_PRIOR 20

// ========================================
// 0. IO timing and device number specificating struct
struct io_config{
    unsigned io_time;
    unsigned device_num;
    unsigned duration;
};
typedef io_config io_config;
typedef io_config *io_ptr;

// ========================================
// 1. ready queue data structure
struct common_task{
    pid_t pid;
    unsigned arrival;
    unsigned remaining;
    io_config io_list[MAX_IO_PER_TASK];
};
typedef common_task common_task;
typedef common_task *c_task_ptr;

//=========== FIFO queue ===========
struct task_q{
    common_task info;
    struct task_q* next;
}; 
typedef struct task_q task_q;
typedef struct task_q *tq_ptr;

struct rq_queue{
    tq_ptr front;
    tq_ptr end;
    unsigned size;
}; 
typedef struct rq_queue rq_Q;
typedef struct rq_queue *rq_Qptr;

rq_Qptr rq_queue_init(void);
int rq_queue_full(rq_Qptr rq);
int rq_queue_empty(rq_Qptr rq);
int rq_enqueue(rq_Qptr rq, tq_ptr new_task);
tq_ptr rq_dequeue(rq_Qptr rq, tq_ptr new_task);

//=========== priority ready queue ===========
struct task_pq{
    common_task info;
    unsigned priority; //smaller number = higher priority 
    // if SJF sched algorithm priority == remaining
}; 
typedef struct task_pq task_pq;
typedef struct task_pq *tpq_ptr;

struct rq_pqueue{
    tpq_ptr min_heap; // starting element in array(min heap)
    unsigned size;
};
typedef struct rq_pqueue rq_PQ;
typedef struct rq_pqueue *rq_PQptr;

int swap(tpq_ptr a, tpq_ptr b);
int min_heapify(rq_PQptr pq, int idx);
int decrease_key(rq_PQptr pq, int idx, unsigned new_key);
task_pq extract_min(rq_PQptr pq);
task_pq look_min(rq_PQptr pq);
int insert(rq_PQptr pq, task_pq new_task);

// ========================================
// 2. task test case data structure
struct input_task{
    common_task task; // sorted in non-decreasing order by arrival time
    io_config io;
    unsigned size;
};
