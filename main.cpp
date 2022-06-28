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
const int RESCHEDULE_ITERATIONS = 40'000;
const int MUTEX_ITERATIONS = 20;
}

// Register the function as a benchmark
BENCHMARK(bench::BM_yaclib_collatz);
BENCHMARK(bench::BM_folly_collatz);
BENCHMARK(bench::BM_cppcoro_collatz);

BENCHMARK(bench::BM_yaclib_reschedule)->Iterations(RESCHEDULE_ITERATIONS);
BENCHMARK(bench::BM_folly_reschedule)->Iterations(RESCHEDULE_ITERATIONS);
BENCHMARK(bench::BM_cppcoro_reschedule)->Iterations(RESCHEDULE_ITERATIONS);

BENCHMARK(bench::BM_yaclib_mutex)->Iterations(MUTEX_ITERATIONS);
BENCHMARK(bench::BM_folly_mutex)->Iterations(MUTEX_ITERATIONS);
BENCHMARK(bench::BM_cppcoro_mutex)->Iterations(MUTEX_ITERATIONS);

// Run the benchmark
BENCHMARK_MAIN();
