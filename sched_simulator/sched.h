#include "data_struct.h"

// all scheduler just change running task
// case (*) if *running == NULL passed, 
// 1) prev_running terminates
// 2) prev_running requested io
// 3) prev_running was idle
// case (**) time slice expired && rq has at least one task
// case (***) rpq has at least one task && preemption happens

// case (*)
void fcfs_alg(rq_Qptr rq, tq_ptr *running);

// case (*), (**)
void rr_alg(rq_Qptr rq, tq_ptr *running, tq_ptr prev_running);

// case (*)
void sjf_alg(rq_PQptr rpq, tpq_ptr *running);

// case (*), (***)
void pre_sjf_alg(rq_PQptr rpq, tpq_ptr *running);

// case (*)
void prior_alg(rq_PQptr rpq, tpq_ptr *running);

// case (*), (***)
void pre_prior_alg(rq_PQptr rpq, tpq_ptr *running);