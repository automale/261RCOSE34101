// data_stuct.c

#include "data_struct.h"

//=========== FIFO ready queue method ===========
rq_Qptr rq_queue_init(void){
    rq_Qptr new_rq = (struct rq_queue *)malloc(sizeof(rq_Q));
    new_rq->front = NULL;
    new_rq->end = NULL;
    new_rq->size = 0;
    return new_rq;
}

int rq_queue_full(rq_Qptr rq){
    if(rq->size == DEGREE_MULTI) return 1;
    else return 0;
}

int rq_queue_empty(rq_Qptr rq){
    if(rq->size == 0) return 1;
    else return 0;
}

int rq_enqueue(rq_Qptr rq, tq_ptr new_task){
    if(rq_queue_full(rq)) return 0;
    else if(rq_queue_empty(rq)){
        rq->front = new_task;
        rq->end = new_task;
    }
    else{
        rq->end->next = new_task;
        rq->end = new_task;
    }
    rq->size += 1;
    return 1;
}

tq_ptr rq_dequeue(rq_Qptr rq, tq_ptr new_task){
    tq_ptr output = NULL;
    if(!rq_queue_empty(rq)){
        output = rq->front;
        rq->front = rq->front->next;
        if(rq->size == 1) rq->end = NULL;
    }
    rq->size -= 1;
    return output;
}

//=========== priority ready queue method ===========
// swap function
int swap(tpq_ptr a, tpq_ptr b){
    if(!a || !b) return 0;
    task_pq temp;
    temp = *a;
    *a = *b;
    *b = temp;
    return 1;
}

// min heapify
int min_heapify(rq_PQptr pq, int idx){
    if(idx > pq->size) return 0;
    int left, right, midx = idx;
    left = 2 * idx;
    right = left + 1;

    if(left <= pq->size && pq->min_heap[idx].priority > pq->min_heap[left].priority){
        midx = left;
    }
    else if(right <= pq->size && pq->min_heap[midx].priority > pq->min_heap[right].priority){
        midx = right;
    }
    if(idx != midx){
        swap(&(pq->min_heap[idx]), &(pq->min_heap[midx]));
        min_heapify(pq, midx);
    }
    return 1;
}

// decrease key
int decrease_key(rq_PQptr pq, int idx, unsigned new_key){
    if(new_key >= pq->min_heap[idx].priority) return 0;
    int current = idx;
    pq->min_heap[idx].priority = new_key;
    while(pq->min_heap[current].priority > pq->min_heap[current/2].priority){
        swap(&(pq->min_heap[current]), &(pq->min_heap[current/2]));
        current /= 2;
    }
    return 1;
}

// extract min
task_pq extract_min(rq_PQptr pq){
    if(pq->size == 0) return (task_pq){0};
    else{
        task_pq min_task = pq->min_heap[1];
        pq->size -= 1;
        if(pq->size > 0){
            swap(&(pq->min_heap[1]), &(pq->min_heap[pq->size]));
            min_heapify(pq, 1);
        }
        return min_task;
    }
}

// look up min
task_pq look_min(rq_PQptr pq){
    if(pq->size == 0) return (task_pq){0};
    else return pq->min_heap[1];
}

// insert
int insert(rq_PQptr pq, task_pq new_task){
    unsigned new_key = new_task.priority;
    pq->size += 1;
    pq->min_heap[pq->size] = new_task;
    pq->min_heap[pq->size].priority = MAX_PRIOR + 1;
    decrease_key(pq, pq->size, new_key);
    return 1;
}