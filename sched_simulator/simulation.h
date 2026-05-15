#include "test_gen.h"
#include "data_struct.h"
#include "sched.h"

// if returns NULL, running -> waiting due to io request
tq_ptr update_r_io_queue(rq_Qptr rq, it_ptr tcase, tq_ptr running, ioq_ptr *io_qs, unsigned *tcnt_ptr, unsigned curr_time);

// sjf, priority algorithm tick
tpq_ptr update_r_io_pqueue(rq_PQptr rpq, it_ptr tcase, tpq_ptr running, ioq_ptr *io_qs, unsigned *tcnt_ptr, unsigned curr_time);

// update running per clock
unsigned tick(void* q, it_ptr tcase, void** running, void *prev_running, ioq_ptr *io_qs, unsigned curr_time, unsigned *tcnt_ptr, sched_alg alg);

// update running & make gant list
schq_ptr sched_simulation(it_ptr tcase, sched_alg alg);