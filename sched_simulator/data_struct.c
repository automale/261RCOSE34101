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
    if(rq->size == MAX_DEGREE_MULTI) return 1;
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

tq_ptr rq_dequeue(rq_Qptr rq){
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
rq_PQptr rpq_init(){
    rq_PQptr rpq = malloc(sizeof(rq_PQ));
    rpq->min_heap = malloc(sizeof(tpq_ptr) * MAX_DEGREE_MULTI);
    rpq->size = 0;
    return rpq;
}

int pq_empty(rq_PQptr pq){
    return (pq->size == 0);
}

// swap function
int swap_task_pq(tpq_ptr *a, tpq_ptr *b){
    if(!a || !b) return -1;
    tpq_ptr temp;
    unsigned temp_idx;
    // swap task itself
    temp = *a;
    *a = *b;
    *b = temp;
    // swap heap idx
    temp_idx = (*a)->heap_idx;
    (*a)->heap_idx = (*b)->heap_idx;
    (*b)->heap_idx = temp_idx;
    return 1;
}

// min heapify
int min_heapify(rq_PQptr pq, tpq_ptr *node){
    unsigned idx = (*node)->heap_idx;
    if(idx > pq->size) return 0;
    unsigned left, right, midx = idx;
    left = 2 * idx;
    right = left + 1;

    if(left <= pq->size && pq->min_heap[idx]->info.priority > pq->min_heap[left]->info.priority){
        midx = left;
    }
    if(right <= pq->size && pq->min_heap[midx]->info.priority > pq->min_heap[right]->info.priority){
        midx = right;
    }
    if(idx != midx){
        swap_task_pq(&(pq->min_heap[idx]), &(pq->min_heap[midx]));
        min_heapify(pq, &pq->min_heap[midx]);
    }
    return 1;
}

// decrease key
int decrease_key(rq_PQptr pq, tpq_ptr update, unsigned new_key){
    if(new_key >= update->info.priority) return 0;
    unsigned idx = update->heap_idx;
    pq->min_heap[idx]->info.priority = new_key;
    while(idx > 1 && pq->min_heap[idx]->info.priority < pq->min_heap[idx/2]->info.priority){
        swap_task_pq(&(pq->min_heap[idx]), &(pq->min_heap[idx/2]));
        idx /= 2;
    }
    return 1;
}

// extract min
tpq_ptr extract_min(rq_PQptr pq){
    if(pq->size == 0) return NULL;
    else{
        tpq_ptr min_task = pq->min_heap[1];
        swap_task_pq(&(pq->min_heap[1]), &(pq->min_heap[pq->size]));
        pq->min_heap[pq->size] = NULL;
        pq->size -= 1;

        if(pq->size > 0){
            min_heapify(pq, &pq->min_heap[1]);
        }
        return min_task;
    }
}

// look up min
tpq_ptr look_min(rq_PQptr pq){
    if(pq->size == 0) return NULL;
    else return pq->min_heap[1];
}

// insert
int insert(rq_PQptr pq, tpq_ptr new_task){
    unsigned new_key = new_task->info.priority;
    pq->size += 1;
    pq->min_heap[pq->size] = new_task;
    pq->min_heap[pq->size]->info.priority = MAX_PRIOR + 1;
    pq->min_heap[pq->size]->heap_idx = pq->size;
    decrease_key(pq, pq->min_heap[pq->size], new_key);
    return 1;
}

int delete(rq_PQptr pq, tpq_ptr del){
    if(pq->size == 0) return -1;

    unsigned idx = del->heap_idx;
    if (idx == pq->size){
        pq->size -= 1;
        return 1;
    }

    swap_task_pq(&pq->min_heap[idx], &pq->min_heap[pq->size]);
    pq->size -= 1;

    // smaller than parent (like decrease_key)
    if(idx > 1 && pq->min_heap[idx]->info.priority < pq->min_heap[idx/2]->info.priority) {
        while(idx > 1 && pq->min_heap[idx]->info.priority < pq->min_heap[idx/2]->info.priority){
            swap_task_pq(&(pq->min_heap[idx]), &(pq->min_heap[idx/2]));
            idx /= 2;
        }
    } 
    // equal or larger than parent
    else {
        min_heapify(pq, &pq->min_heap[idx]);
    }
    
    return 1;
}

//=========== io waiting ring buffer queue method ===========
// init ring buffer queue whose capacity is designated (maybe DEGREE_MULTI)
ioq_ptr init_io_queue(unsigned _capacity){
    ioq_ptr q = malloc(sizeof(ioq_node));
    q->rbuf_q = malloc(sizeof(tpq_ptr) * _capacity);
    q->front = 0;
    q->end = 0;
    q->size = 0;
    q->capacity = _capacity;
    return q;
}

// Enqueue
void io_enqueue(ioq_ptr q, void *task_ptr) {
    if (q->size == q->capacity) return;
    q->rbuf_q[q->end] = task_ptr;
    q->end = (q->end + 1) % q->capacity;
    q->size++;
}

// Dequeue
void* io_dequeue(ioq_ptr q) {
    if (q->size == 0) return NULL;
    void *task_ptr = q->rbuf_q[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    return task_ptr;
}

// ============== gant chart method ==============
sch_ptr shced_init(unsigned _pid, unsigned _start, unsigned _end){
    sch_ptr ret_sched = malloc(sizeof(sched));
    ret_sched->pid = _pid;
    ret_sched->start = _start;
    ret_sched->end = _end;
    ret_sched->next = NULL;
}

schq_ptr sched_queue_init(){
    schq_ptr sq = malloc(sizeof(sched_queue));
    sq->front = sq->end = NULL;
    sq->size = 0;
}

int schq_enqueue(schq_ptr queue, sch_ptr node){
    // if queue was empty
    if (queue->size == 0){
        queue->front = queue->end = node;
        queue->size += 1;
    }
    // if queue was not empty
    else{
        queue->end->next = node;
        queue->end = node;
        queue->size += 1;
    }
    return 1;
}

sched schq_dequeue(schq_ptr queue){
    if (queue->size <= 0) return (sched){0};
    sch_ptr ret_sch_ptr = queue->front;
    sched ret_sch = *ret_sch_ptr;
    
    queue->front = queue->front->next;
    free(ret_sch_ptr);
    queue->size -= 1;
    
    return ret_sch;
}