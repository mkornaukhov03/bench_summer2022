#include <array>
#include <cassert>
#include <iostream>
#include <tuple>
#include <utility>

#include <benchmark/benchmark.h>

#include <yaclib/algo/wait.hpp>
#include <yaclib/coroutine/async_mutex.hpp>
#include <yaclib/coroutine/await.hpp>
#include <yaclib/coroutine/await_group.hpp>
#include <yaclib/coroutine/future_traits.hpp>
#include <yaclib/coroutine/on.hpp>
#include <yaclib/coroutine/oneshot_event.hpp>
#include <yaclib/executor/inline.hpp>
#include <yaclib/executor/thread_pool.hpp>

#include <folly/Portability.h>

#include <folly/executors/InlineExecutor.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/CurrentExecutor.h>
#include <folly/experimental/coro/Task.h>
#include <folly/experimental/coro/ViaIfAsync.h>

#include <cppcoro/async_latch.hpp>
#include <cppcoro/async_mutex.hpp>
#include <cppcoro/inline_scheduler.hpp>
#include <cppcoro/resume_on.hpp>
#include <cppcoro/schedule_on.hpp>
#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>
#include <cppcoro/when_all.hpp>

#include "benchmark_decls.hpp"
namespace {
constexpr int kFutures = 20;
}
namespace bench {

yaclib::Future<void> yaclib_latch(yaclib::IExecutor &tp,
                                  yaclib::AwaitGroup &ag) {
  co_await On(tp);
  ag.Done();
  co_await ag;
}

folly::coro::Task<void> folly_latch() {
  //  co_await folly::coro::co_mutex_on_current_executor; // TODO
  co_return;
}

cppcoro::task<void> cppcoro_latch(cppcoro::static_thread_pool &tp,
                                  cppcoro::async_latch &latch) {
  co_await tp.schedule();
  latch.count_down();
  co_await latch;
}

void BM_yaclib_latch(benchmark::State &state) {
  // Perform setup here
  auto tp = yaclib::MakeThreadPool();
  yaclib::AwaitGroup ag;
  for (auto _ : state) {
    ag.Reset();
    auto foo = [&]() -> yaclib::Future<void> {
      std::vector<yaclib::Future<void>> arr(kFutures);
      ag.Add(kFutures);
      for (int i = 0; i < kFutures; ++i) {
        arr[i] = yaclib_latch(*tp, ag);
      }
      co_await ag;
    };
    std::ignore = foo().Get();
  }
  tp->HardStop();
  tp->Wait();
}

void BM_folly_latch(benchmark::State &state) {
  // Perform setup here
  for (auto _ : state) {
    // This code gets timed
    // folly_mutex().scheduleOn(&folly::InlineExecutor::instance()).start().get();
  }
}

void BM_cppcoro_latch(benchmark::State &state) {
  // Perform setup here
  cppcoro::static_thread_pool tp{};
  cppcoro::async_latch m{kFutures};
  std::array<cppcoro::task<void>, kFutures> arr;

  for (auto _ : state) {
    auto foo = [&]() -> cppcoro::task<void> {
      std::vector<cppcoro::task<void>> arr(kFutures);
      for (int i = 0; i < kFutures; ++i) {
        arr[i] = cppcoro_latch(tp, m);
      }
      co_await cppcoro::when_all(std::move(arr));
    };
    cppcoro::sync_wait(foo());
  }
}

} // namespace bench
