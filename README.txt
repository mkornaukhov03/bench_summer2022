# bench_summer2022

Apple M1, 16 RAM
TODO: generate a chart
---------------------------------------------------------------------------------
Benchmark                                       Time             CPU   Iterations
---------------------------------------------------------------------------------
bench::BM_yaclib_collatz                     3612 ns         3612 ns       192330
bench::BM_folly_collatz                      2.15 ns         2.15 ns    325776861
bench::BM_cppcoro_collatz                     925 ns          925 ns       751847
bench::BM_yaclib_reschedule/real_time        5986 ns         1856 ns       115544
bench::BM_folly_reschedule/real_time         2.16 ns         2.16 ns    328135768
bench::BM_cppcoro_reschedule/real_time      42519 ns         2828 ns        16555
bench::BM_yaclib_mutex/real_time         46066305 ns        15062 ns           16
bench::BM_folly_mutex/real_time              2.15 ns         2.15 ns    328148841
bench::BM_cppcoro_mutex/real_time        31810346 ns        30550 ns           20
bench::BM_yaclib_latch/real_time            29471 ns        18061 ns        23591
bench::BM_folly_latch/real_time              2.12 ns         2.12 ns    327936948
bench::BM_cppcoro_latch/real_time           49603 ns        18077 ns        10788
