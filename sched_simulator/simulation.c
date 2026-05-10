#include "simulation.h"

// fcfs, rr algorithm tick
// if returns NULL, running -> waiting due to io request
tq_ptr update_r_io_queue(rq_Qptr rq, it_ptr tcase, tq_ptr running, ioq_ptr *io_qs, unsigned curr_time){
    // io_queue updating
    for(unsigned i = 0; i < DEVICES; i++){
        if(io_qs[i]->size > 0){
            tq_ptr io_front = (tq_ptr)(io_qs[i]->rbuf_q[io_qs[i]->front]);
            io_front->info.current_io_remaining -= 1;
            // io finish -> dequeue from io queue & enqueue in ready queue
            if( io_front->info.current_io_remaining == 0 ){
                io_front->info.current_io_idx += 1;
                rq_enqueue(rq, (tq_ptr)io_dequeue(io_qs[i]));
            }
        }
    }

    // arrival task enqueue in ready queue
    unsigned *next_arrival_ptr = &(tcase->next_arrival_idx);
    while((*next_arrival_ptr) < tcase->size){
        if(tcase->task_list[*next_arrival_ptr].arrival == curr_time){
            tq_ptr new_node = (tq_ptr)malloc(sizeof(task_q));
            new_node->info = tcase->task_list[*next_arrival_ptr];
            new_node->next = NULL;
            rq_enqueue(rq, new_node);
            *next_arrival_ptr += 1;
        }
        else break;
    }

    // for busy cpu time, running task remaining time decrease & termination, io request check
    if(running){
        if(--running->info.remaining == 0) return NULL;
        unsigned idx, extime;
        idx = running->info.current_io_idx;
        extime =  running->info.burst - running->info.remaining;
        // boundary check if current_io_idx is less than io_num(no more ios left exception)
        if(idx < running->info.io_num && running->info.io_list[idx].io_time == extime){
            running->info.current_io_idx = idx;
            running->info.current_io_remaining = running->info.io_list[idx].duration;
            io_enqueue(io_qs[running->info.io_list[idx].device_num], running);
            return NULL;
        }
    }
    return running;
}

// sjf, priority algorithm tick
tpq_ptr update_r_io_pqueue(rq_PQptr rpq, it_ptr tcase, tpq_ptr running, ioq_ptr *io_qs, unsigned curr_time){
    // io_queue updating
    for(unsigned i = 0; i < DEVICES; i++){
        if(io_qs[i]->size > 0){
            tpq_ptr io_front = (tpq_ptr)(io_qs[i]->rbuf_q[io_qs[i]->front]);
            io_front->info.current_io_remaining -= 1;
            // io finish -> dequeue from io queue & enqueue in ready queue
            if( io_front->info.current_io_remaining == 0 ){
                io_front->info.current_io_idx += 1;
                insert(rpq, (tpq_ptr)io_dequeue(io_qs[i]));
            }
        }
    }

    // arrival task enqueue in ready queue
    unsigned *next_arrival_ptr = &(tcase->next_arrival_idx);
    while((*next_arrival_ptr) < tcase->size){
        if(tcase->task_list[*next_arrival_ptr].arrival == curr_time){
            tpq_ptr new_node = (tpq_ptr)malloc(sizeof(task_pq));
            new_node->info = tcase->task_list[*next_arrival_ptr];
            insert(rpq, new_node);
            *next_arrival_ptr += 1;
        }
        else break;
    }

    // for busy cpu time, running task io request check
    if(running){
        if(--running->info.remaining == 0) return NULL;
        unsigned idx, extime;
        idx = running->info.current_io_idx;
        extime =  running->info.burst - running->info.remaining;
        // boundary check if current_io_idx is less than io_num(no more ios left exception)
        if(idx < running->info.io_num && running->info.io_list[idx].io_time == extime){
            running->info.current_io_idx = idx;
            running->info.current_io_remaining = running->info.io_list[idx].duration;
            io_enqueue(io_qs[running->info.io_list[idx].device_num], running);
            return NULL;
        }
    }
    return running;
}

void tick(void* q, it_ptr tcase, void** running, ioq_ptr *io_qs, unsigned curr_time, sched_alg alg){
    if(alg < sjf){
        tq_ptr u_running_q = update_r_io_queue((rq_Qptr)q, tcase, (tq_ptr)*running, io_qs, curr_time);
        switch(alg){
            case fcfs : // running termination | io request waiting
            case rr : // volatile로 time quanta 계산, time quanta expired | running termination | io waiting
            default :
        }
    }
    else{
        tpq_ptr u_running_pq = update_r_io_pqueue((rq_PQptr)q, tcase, (tpq_ptr)*running, io_qs, curr_time);
        switch(alg){
            case sjf : // running termination | io request waiting
            case pre_sjf : // running termination | io request waiting | lookup_min < running -> tick : priority = remaining update needed
            case prior : // running termination | io request waiting
            case pre_prior: // running termination | io request waiting | lookup_min < running
            default :
        }
    }
}

schq_ptr sched_simulation(it_ptr tcase, sched_alg alg){
    unsigned curr_time = -1;
    // io device queues * DEVICES 
    ioq_ptr *io_qs = malloc(sizeof(ioq_ptr) * DEVICES);
    for(int i = 0; i < DEVICES; i++){
        io_qs[i] = init_io_queue(MAX_DEGREE_MULTI);
    }
    // fcfs, rr
    if(alg < sjf){
        rq_Qptr rq = malloc(sizeof(rq_Q));
        tq_ptr prev_running, running;
        prev_running = running = NULL;
        while( !rq_queue_empty(rq) && tcase->size ){
            tick(rq, tcase, &running, io_qs, ++curr_time, alg);
            if(prev_running != running){
                if(prev_running == NULL){

                }
                else{
                    
                }
                if(running == NULL){

                }
                else{

                }

            }
        }

    }
    // sjf ~ pre priority
    else{

    }
}