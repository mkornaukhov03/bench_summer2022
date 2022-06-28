#include "benchmark_decls.hpp"

BENCHMARK(bench::BM_cppcoro_reschedule)->Iterations(1000)->UseRealTime();
BENCHMARK(bench::BM_yaclib_reschedule)->Iterations(1000)->UseRealTime();

BENCHMARK(bench::BM_cppcoro_collatz)
    ->UseRealTime()
    ->ArgsProduct({{1, 2, 3, 6, 7, 9, 18, 25, 27}});
BENCHMARK(bench::BM_yaclib_collatz)
    ->UseRealTime()
    ->ArgsProduct({{1, 2, 3, 6, 7, 9, 18, 25, 27}});

BENCHMARK(bench::BM_cppcoro_mutex)->UseRealTime();
BENCHMARK(bench::BM_yaclib_mutex)->UseRealTime();

BENCHMARK(bench::BM_cppcoro_latch)
    ->UseRealTime()
    ->ArgsProduct({{1, 5, 10, 50, 100, 500}});
BENCHMARK(bench::BM_yaclib_latch)
    ->UseRealTime()
    ->ArgsProduct({{1, 5, 10, 50, 100, 500}});
/*
BENCHMARK(bench::BM_yaclib_latch2)
    ->UseRealTime()
    ->ArgsProduct({{1, 5, 10, 50, 100, 500}});
*/

// Run the benchmark
BENCHMARK_MAIN();
