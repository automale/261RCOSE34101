// data_stuct.h
#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MIN_DEGREE_MULTI 10
#define MAX_DEGREE_MULTI 100

#define MAX_IO_PER_TASK 10
#define MAX_IO_DURATION 50
#define MAX_IO_POSB_MOD 32

#define MIN_TOTAL_EXTIME 500
#define MAX_TOTAL_EXTIME 5000

#define QUANTA 5
#define MAX_PRIOR 100

#define DEVICES 10

// sched algorithm select
typedef enum {fcfs, rr, sjf, pre_sjf, prior, pre_prior} sched_alg;
// ========================================
// 0. IO timing and device number specificating struct
struct io_config{
    unsigned io_time;
    unsigned device_num;
    unsigned duration;
};
typedef struct io_config io_config;
typedef struct io_config *io_ptr;

// ========================================
// 1. ready queue data structure
struct common_task{
    // ps(task) info
    unsigned pid;
    unsigned arrival;
    unsigned burst;
    unsigned remaining;
    /* smaller number = higher priority 
    if SJF sched algorithm priority == remaining */
    unsigned priority;

    // io info
    io_ptr io_list;
    unsigned io_num;
    unsigned current_io_idx;
    unsigned current_io_remaining;
};
typedef struct common_task common_task;
typedef struct common_task *c_task_ptr;

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
tq_ptr rq_dequeue(rq_Qptr rq);

//=========== priority ready queue ===========
struct task_pq{
    common_task info;
    unsigned heap_idx;
}; 
typedef struct task_pq task_pq;
typedef struct task_pq *tpq_ptr;

struct rq_pqueue{
    tpq_ptr *min_heap; // starting element in array(min heap)
    unsigned size;
};
typedef struct rq_pqueue rq_PQ;
typedef struct rq_pqueue *rq_PQptr;

//=========== priority ready queue method ===========
rq_PQptr rpq_init();
int pq_empty(rq_PQptr pq);
// swap function
int swap_task_pq(tpq_ptr *a, tpq_ptr *b);
int min_heapify(rq_PQptr pq, tpq_ptr *node);
int decrease_key(rq_PQptr pq, tpq_ptr update, unsigned new_key);
tpq_ptr extract_min(rq_PQptr pq);
tpq_ptr look_min(rq_PQptr pq);
int insert(rq_PQptr pq, tpq_ptr new_task);
int delete(rq_PQptr pq, tpq_ptr del);

// ========================================
// 2. io device queue data structure
// typedef enum {fifo, prior} queue_mode;

struct ioq_node{
    void **rbuf_q;
    unsigned front, end, size, capacity;
};
typedef struct ioq_node ioq_node;
typedef struct ioq_node *ioq_ptr;

ioq_ptr init_io_queue(unsigned _capacity);
void io_enqueue(ioq_ptr q, void *task_ptr);
void* io_dequeue(ioq_ptr q);

// ========================================
// 3. task test case data structure
struct input_tasks{
    common_task *task_list; // sorted in non-decreasing order by arrival time
    unsigned size;
    unsigned next_arrival_idx;
};
typedef struct input_tasks input_tasks;
typedef struct input_tasks *it_ptr;

void rewind_input_task(it_ptr tcase);

// ========================================
// 4. gant chart data structure
struct sched{
    unsigned pid;
    unsigned start;
    unsigned end;
    struct sched *next;
};
typedef struct sched sched;
typedef struct sched *sch_ptr; 

struct sched_queue{
    sch_ptr front;
    sch_ptr end;
    unsigned size;
};
typedef struct sched_queue sched_queue;
typedef struct sched_queue *schq_ptr;

sch_ptr sched_init(unsigned _pid, unsigned _start, unsigned _end);
schq_ptr sched_queue_init();
int schq_enqueue(schq_ptr queue, sch_ptr node);
sched schq_dequeue(schq_ptr queue);