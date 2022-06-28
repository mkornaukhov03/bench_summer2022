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

#include <cppcoro/inline_scheduler.hpp>
#include <cppcoro/schedule_on.hpp>
#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>

#include "benchmark_decls.hpp"

namespace bench {

yaclib::Future<void> yaclib_reschedule(yaclib::IExecutor &tp) {
  for (int i = 0; i < 100; ++i) {
    co_await On(tp);
  }
}

cppcoro::task<void> cppcoro_reschedule(cppcoro::static_thread_pool &tp) {
  for (int i = 0; i < 100; ++i) {
    co_await tp.schedule();
  }
}

void BM_yaclib_reschedule(benchmark::State &state) {
  auto tp = yaclib::MakeThreadPool(1);
  for (auto _ : state) {
    std::ignore = yaclib_reschedule(*tp).Get();
  }
  tp->HardStop();
  tp->Wait();
}

void BM_cppcoro_reschedule(benchmark::State &state) {
  cppcoro::static_thread_pool tp{1};
  for (auto _ : state) {
    cppcoro::sync_wait(cppcoro_reschedule(tp));
  }
}

} // namespace bench
