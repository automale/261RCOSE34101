test_gen.c : generate 1000 test case (cpu burst * N, IO burst * M)
simulator.c : call test_gen to generate test cases and for each test case, simulate sched algorithm & draw gant chart
draw_gant.c : draw gant chart
sched.h : define each algorithm's task struct and data structure