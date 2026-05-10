#include "test_gen.h"
#include "data_struct.h"
#include "sched.h"

// fcfs, rr algorithm
tq_ptr update_r_io_queue(rq_Qptr rq, it_ptr tcase, tq_ptr running, ioq_ptr *io_qs, unsigned curr_time);
// sjf, priority algorithm
tpq_ptr update_r_io_pqueue(rq_PQptr rpq, it_ptr tcase, tpq_ptr running, ioq_ptr *io_qs, unsigned curr_time);