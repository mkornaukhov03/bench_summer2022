#include "benchmark_decls.hpp"

BENCHMARK(bench::BM_cppcoro_reschedule)->Iterations(1000)->UseRealTime();
BENCHMARK(bench::BM_yaclib_reschedule)->Iterations(1000)->UseRealTime();

BENCHMARK(bench::BM_cppcoro_collatz)
    ->UseRealTime()
    ->ArgsProduct({{27}});
BENCHMARK(bench::BM_yaclib_collatz)
    ->UseRealTime()
    ->ArgsProduct({{27}});

BENCHMARK(bench::BM_cppcoro_mutex)->UseRealTime();
BENCHMARK(bench::BM_yaclib_mutex)->UseRealTime();

BENCHMARK(bench::BM_cppcoro_latch)
    ->UseRealTime()
    ->ArgsProduct({{600}});
BENCHMARK(bench::BM_yaclib_latch)
    ->UseRealTime()
    ->ArgsProduct({{600}});
// Run the benchmark
BENCHMARK_MAIN();
