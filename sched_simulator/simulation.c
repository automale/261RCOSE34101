#include "simulation.h"

// fcfs, rr algorithm tick
tq_ptr update_r_io_queue(rq_Qptr rq, it_ptr tcase, tq_ptr running, ioq_ptr *io_qs, unsigned *tcnt_ptr, unsigned curr_time){
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
        if(--running->info.remaining == 0) {
            free(running);
            *tcnt_ptr += 1; 
            return NULL;
        }
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
tpq_ptr update_r_io_pqueue(rq_PQptr rpq, it_ptr tcase, tpq_ptr running, ioq_ptr *io_qs, unsigned *tcnt_ptr, unsigned curr_time){
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
        if(--running->info.remaining == 0) {
            free(running);
            *tcnt_ptr += 1;
            return NULL;
        }
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

unsigned tick(void* q, it_ptr tcase, void** running, void *prev_running, ioq_ptr *io_qs, unsigned curr_time, unsigned *tcnt_ptr, sched_alg alg){
    if(alg < sjf){
        *running = update_r_io_queue((rq_Qptr)q, tcase, (tq_ptr)*running, io_qs, tcnt_ptr,  curr_time);
        switch(alg){
            case fcfs : // running termination | io request waiting
                fcfs_alg((rq_Qptr)q, (tq_ptr*)running);
                break;
            case rr : // volatile로 time quanta 계산, time quanta expired | running termination | io waiting
                rr_alg((rq_Qptr)q, (tq_ptr*)running, prev_running);
                break;
            default :
        }
        if(running != NULL) return (*(tq_ptr*)running)->info.pid;
        else return (unsigned)(-1);
    }
    else{
        *running = update_r_io_pqueue((rq_PQptr)q, tcase, (tpq_ptr)*running, io_qs, tcnt_ptr, curr_time);
        switch(alg){
            case sjf : // running termination | io request waiting
                sjf_alg((rq_PQptr)q, (tpq_ptr*)running);
                break;
            case pre_sjf : // running termination | io request waiting | lookup_min < running -> tick : priority = remaining update needed
                pre_sjf_alg((rq_PQptr)q, (tpq_ptr*)running);
                break;
            case prior : // running termination | io request waiting
                prior_alg((rq_PQptr)q, (tpq_ptr*)running);
                break;
            case pre_prior: // running termination | io request waiting | lookup_min < running
                pre_prior_alg((rq_PQptr)q, (tpq_ptr*)running);
                break;
            default :
        }
        if(running != NULL) return (*(tpq_ptr*)running)->info.pid;
        else return (unsigned)(-1);
    }
}

schq_ptr sched_simulation(it_ptr tcase, sched_alg alg){
    schq_ptr result_gant = sched_queue_init();
    sch_ptr sch_node;
    unsigned terminate_cnt = 0, curr_time = -1; // clock(time)
    // io device queues * DEVICES 
    ioq_ptr *io_qs = malloc(sizeof(ioq_ptr) * DEVICES);
    for(int i = 0; i < DEVICES; i++){
        io_qs[i] = init_io_queue(MAX_DEGREE_MULTI);
    }

    // ready queue init
    void * rq;
    if(alg < sjf) { rq = rq_queue_init(); }
    else { rq = rpq_init(); }

    // prev_running, running tracking
    void *prev_running = NULL, *running = NULL;
    unsigned prev_pid = (unsigned)(-1), curr_pid = (unsigned)(-1);

    while( terminate_cnt < tcase->size ){
        // running update, io waiting queue update + scheduling (running update)
        curr_pid = tick(rq, tcase, &running, prev_running, io_qs, ++curr_time, &terminate_cnt ,alg);

        // node generation and enqueue in result node
        if(prev_pid != curr_pid || result_gant->size == 0 ){
            if( result_gant->size != 0 ) result_gant->end->end = curr_time; 
            // if idle time, then node's pid will be 0xffff_ffff
            sch_node =  sched_init(curr_pid, curr_time, (unsigned)(-1));
            schq_enqueue(result_gant, sch_node);
        }
        // update prev_running
        prev_running = running;
        prev_pid = curr_pid;
    }
    // end of the last gant node
    if(result_gant->size != 0){ result_gant->end->end = curr_time; }
    
    return result_gant;
}