#include <cassert>
#include <iostream>
#include <tuple>
#include <utility>

#include <benchmark/benchmark.h>

#include <yaclib/coroutine/future_traits.hpp>

#include <folly/Portability.h>

#include <folly/executors/InlineExecutor.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Task.h>

#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>

#include "benchmark_decls.hpp"

namespace {
const int RESCHEDULE_ITERATIONS = 1'000;
const int MUTEX_ITERATIONS = 20;
const int LATCH_ITERATIONS = 20;
} // namespace

// Register the function as a benchmark

BENCHMARK(bench::BM_yaclib_collatz);
BENCHMARK(bench::BM_folly_collatz);
BENCHMARK(bench::BM_cppcoro_collatz);

BENCHMARK(bench::BM_yaclib_reschedule)
    ->UseRealTime();
BENCHMARK(bench::BM_folly_reschedule)
    ->UseRealTime();
BENCHMARK(bench::BM_cppcoro_reschedule)
    ->UseRealTime();

BENCHMARK(bench::BM_yaclib_mutex)->UseRealTime();
BENCHMARK(bench::BM_folly_mutex)->UseRealTime();
BENCHMARK(bench::BM_cppcoro_mutex)->UseRealTime();
BENCHMARK(bench::BM_yaclib_latch)->UseRealTime();
BENCHMARK(bench::BM_folly_latch)->UseRealTime();
BENCHMARK(bench::BM_cppcoro_latch)->UseRealTime();

// Run the benchmark
BENCHMARK_MAIN();
