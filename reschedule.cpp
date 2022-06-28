#include <cassert>
#include <iostream>
#include <tuple>
#include <utility>

#include <benchmark/benchmark.h>

#include <yaclib/algo/wait.hpp>
#include <yaclib/coroutine/await.hpp>
#include <yaclib/coroutine/future_traits.hpp>
#include <yaclib/coroutine/on.hpp>
#include <yaclib/executor/inline.hpp>
#include <yaclib/executor/thread_pool.hpp>

#include <folly/Portability.h>

#include <folly/executors/InlineExecutor.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/CurrentExecutor.h>
#include <folly/experimental/coro/Task.h>
#include <folly/experimental/coro/ViaIfAsync.h>

#include <cppcoro/inline_scheduler.hpp>
#include <cppcoro/resume_on.hpp>
#include <cppcoro/schedule_on.hpp>
#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>

#include "benchmark_decls.hpp"

namespace {
constexpr int N = 50;
}

namespace bench {

yaclib::Future<void> yaclib_reschedule(yaclib::IExecutor &tp) {
  for (int i = 0; i < N; ++i) {
    co_await On(tp);
  }
}

folly::coro::Task<void> folly_reschedule() {
  //  co_await folly::coro::co_reschedule_on_current_executor; // TODO
  co_return;
}

cppcoro::task<void> cppcoro_reschedule(cppcoro::static_thread_pool &tp) {
  for (int i = 0; i < N; ++i) {
    co_await tp.schedule();
  }
}

void BM_yaclib_reschedule(benchmark::State &state) {
  // Perform setup here
  auto tp = yaclib::MakeThreadPool(1);
  for (auto _ : state) {
    // This code gets timed
    std::ignore = yaclib_reschedule(*tp).Get();
  }
  tp->HardStop();
  tp->Wait();
}

void BM_folly_reschedule(benchmark::State &state) {
  // Perform setup here
  for (auto _ : state) {
    // This code gets timed
    // folly_reschedule().scheduleOn(&folly::InlineExecutor::instance()).start().get();
  }
}

void BM_cppcoro_reschedule(benchmark::State &state) {
  // Perform setup here
  cppcoro::static_thread_pool tp(1);
  for (auto _ : state) {
    // This code gets timed
    cppcoro::sync_wait(cppcoro_reschedule(tp));
  }
}

} // namespace bench
