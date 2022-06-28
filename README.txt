# bench_summer2022

Apple M1, 16 RAM
TODO: generate a chart
-------------------------------------------------------------------------------------------------
Benchmark                                                       Time             CPU   Iterations
-------------------------------------------------------------------------------------------------
bench::BM_cppcoro_reschedule/iterations:1000/real_time      54439 ns         6789 ns         1000
bench::BM_yaclib_reschedule/iterations:1000/real_time       12377 ns         2601 ns         1000
bench::BM_cppcoro_collatz/27/real_time                      11704 ns        11704 ns        58773
bench::BM_yaclib_collatz/27/real_time                       59541 ns        59530 ns        11806
bench::BM_cppcoro_mutex/real_time                       555413458 ns       200000 ns            1
bench::BM_yaclib_mutex/real_time                        181482604 ns       129750 ns            4
bench::BM_cppcoro_latch/600/real_time                      687294 ns       302060 ns         1000
bench::BM_yaclib_latch/600/real_time                      1021829 ns       848656 ns          622
