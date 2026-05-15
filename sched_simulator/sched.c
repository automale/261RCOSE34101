#include "sched.h"

// all scheduler just change running task
// case (*) if *running == NULL passed, 
// 1) prev_running terminates
// 2) prev_running requested io
// 3) prev_running was idle
// case (**) time slice expired && rq has at least one task
// case (***) rpq has at least one task && preemption happens

// case (*)
void fcfs_alg(rq_Qptr rq, tq_ptr *running){
    if(!rq_queue_empty(rq) && *running == NULL){
        *running = rq_dequeue(rq);
    }
}

// case (*), (**)
void rr_alg(rq_Qptr rq, tq_ptr *running, tq_ptr prev_running){
    static unsigned quanta = -1;
    quanta++;
    // idle -> idle 
    // ... time quanta init -1
    if(prev_running == NULL && rq_queue_empty(rq)){
        quanta = -1;
    }
    // running termination || io waiting && rq non-empty
    // ... new busy task & time quanta renew
    else if(!rq_queue_empty(rq) && *running == NULL){
        quanta = 0;
        *running = rq_dequeue(rq);
    }
    // time quanta expired
    // ... time quanta renew
    else if(quanta == QUANTA){
        quanta = 0;
        // ... no context change (implicit)
        // ... new busy task
        if(!rq_queue_empty(rq)){
            rq_enqueue(rq, *running);
            *running = rq_dequeue(rq);
        }
    }
    // busy(task A) -> busy(task A)
    // ... no time quanta update
}

// case (*)
void sjf_alg(rq_PQptr rpq, tpq_ptr *running){
    // idle -> busy || busy -> idle || busy -> busy
    if(!pq_empty(rpq) && *running == NULL){
        *running = extract_min(rpq);
    }
}

// case (*), (***)
void pre_sjf_alg(rq_PQptr rpq, tpq_ptr *running){
    // empty ready queue -> ignore
    if(pq_empty(rpq)) return ;

    tpq_ptr cand = look_min(rpq);
    // in the case of termination, waiting queue, idle
    if(*running == NULL){
        *running = extract_min(rpq);
    }
    // preemption
    else if(cand->info.priority < (*running)->info.priority){
        (*running)->info.priority = (*running)->info.remaining;
        insert(rpq, *running);
        *running = extract_min(rpq);
    }
}

// case (*)
void prior_alg(rq_PQptr rpq, tpq_ptr *running){
    if(!pq_empty(rpq) && *running == NULL){
        *running = extract_min(rpq);
    }
}

// case (*), (***)
void pre_prior_alg(rq_PQptr rpq, tpq_ptr *running){
    static unsigned quanta = -1;
    // empty ready queue -> ignore
    if(pq_empty(rpq)) {return ;}

    tpq_ptr cand = look_min(rpq);
    // in the case of termination, waiting queue, idle
    if(*running == NULL){
        quanta = 0;
        *running = extract_min(rpq);
    }
    // preemption
    else if(cand->info.priority < (*running)->info.priority){
        quanta = 0;
        insert(rpq, *running);
        *running = extract_min(rpq);
    }
    else if(cand->info.priority == (*running)->info.priority \
            && ++quanta == QUANTA ){
        quanta = 0;
        tpq_ptr temp = *running;
        *running = extract_min(rpq);
        insert(rpq, temp);
    }
}