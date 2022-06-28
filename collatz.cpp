#include <cassert>
#include <iostream>
#include <tuple>
#include <utility>

#include <benchmark/benchmark.h>

#include <yaclib/algo/wait.hpp>
#include <yaclib/coroutine/await.hpp>
#include <yaclib/coroutine/future_traits.hpp>

#include <folly/Portability.h>

#include <folly/executors/InlineExecutor.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Task.h>

#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>

#include "benchmark_decls.hpp"

#include <iostream> // remove later

namespace {
constexpr int N = 10;
}

namespace bench {

yaclib::Future<void> yaclib_collatz(int x) {
  if (x <= 1)
    co_return;
  if (x % 2 == 0) {
    auto foo = yaclib_collatz(x / 2);
    co_await Await(foo);
    co_return;
  }
  auto foo = yaclib_collatz(3 * x + 1);
  co_await Await(foo);
  co_return;
}

folly::coro::Task<void> folly_collatz(int x) {
  if (x <= 1)
    co_return;
  if (x % 2 == 0) {
    co_await folly_collatz(x / 2);
    co_return;
  }
  co_await folly_collatz(3 * x + 1);
  co_return;
}

cppcoro::task<void> cppcoro_collatz(int x) {
  if (x <= 1)
    co_return;
  if (x % 2 == 0) {
    co_await cppcoro_collatz(x / 2);
    co_return;
  }
  co_await cppcoro_collatz(3 * x + 1);
  co_return;
}

void BM_yaclib_collatz(benchmark::State &state) {
  // Perform setup here
  for (auto _ : state) {
    // This code gets timed
    std::ignore = yaclib_collatz(N).Get();
  }
}

void BM_folly_collatz(benchmark::State &state) {
  // Perform setup here
  for (auto _ : state) {
    // This code gets timed
    // folly_collatz(N).scheduleOn(&folly::InlineExecutor::instance()).start().get();
  }
}

void BM_cppcoro_collatz(benchmark::State &state) {
  // Perform setup here
  for (auto _ : state) {
    // This code gets timed
    cppcoro::sync_wait(cppcoro_collatz(N));
  }
}

} // namespace bench
