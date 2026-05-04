#include "data_struct.h"

// #define DEGREE_MULTI 20
// #define MAX_IO_PER_TASK 10
// #define MAX_PRIOR 20

// // ========================================
// // 0. IO timing and device number specificating struct
// struct io_config{
//     unsigned time_list;
//     unsigned device_num;
// };
// typedef io_config io_config;
// typedef io_config *io_ptr;

// // ========================================
// // 1. ready queue data structure
// struct common_task{
//     pid_t pid;
//     unsigned arrival;
//     unsigned remaining;
//     io_config io_list[MAX_IO_PER_TASK];
// };
// typedef common_task common_task;
// typedef common_task *c_task_ptr;

// struct input_tasks{
//     common_task task_list[DEGREE_MULTI]; // sorted in non-decreasing order by arrival time
//     unsigned size;
// };

// input_task gen random
