#include <array>
#include <cassert>
#include <iostream>
#include <tuple>
#include <utility>

#include <benchmark/benchmark.h>

#include <yaclib/algo/wait.hpp>
#include <yaclib/coroutine/async_mutex.hpp>
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

#include <cppcoro/async_mutex.hpp>
#include <cppcoro/inline_scheduler.hpp>
#include <cppcoro/resume_on.hpp>
#include <cppcoro/schedule_on.hpp>
#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>

#include "benchmark_decls.hpp"

namespace {
constexpr int kClients = 1000;
constexpr int kQperClient = 10;
constexpr int kStateSize = 64;
constexpr int kMod = 1'000'007;

struct State {
  int state[kStateSize]{};
  std::size_t id{};
  void reset() {
    id = 0;
    for (int i = 0; i < kStateSize; ++i) {
      state[i] = 0;
    }
  }
  void accept() {
    int local = id++;
    for (int i = 0; i < kStateSize; ++i) {
      state[i] ^= local;
      local = get_next(local);
    }
  }
  int get_next(int x) { return (x * x + 1) % kMod; }
};

State s;

} // namespace

namespace bench {

yaclib::Future<void> yaclib_mutex(yaclib::IExecutor &tp,
                                  yaclib::AsyncMutex<false> &m) {
  co_await On(tp);
  for (int i = 0; i < kQperClient; ++i) {
    co_await m.Lock();
    s.accept();
    co_await m.UnlockOn(tp);
  }
}

folly::coro::Task<void> folly_mutex() {
  //  co_await folly::coro::co_mutex_on_current_executor; // TODO
  co_return;
}

cppcoro::task<void> cppcoro_mutex(cppcoro::static_thread_pool &tp,
                                  cppcoro::async_mutex &m) {
  co_await tp.schedule();
  for (int i = 0; i < kQperClient; ++i) {
    co_await m.lock_async();
    s.accept();
    m.unlock();
  }
}

void BM_yaclib_mutex(benchmark::State &state) {
  // Perform setup here
  auto tp = yaclib::MakeThreadPool();
  yaclib::AsyncMutex<false> m;
  for (auto _ : state) {
    s.reset();
    std::array<yaclib::Future<void>, kClients> arr;
    for (int i = 0; i < kClients; ++i) {
      arr[i] = yaclib_mutex(*tp, m);
    }
    Wait(arr.begin(), arr.end());
  }
  tp->HardStop();
  tp->Wait();
}

void BM_folly_mutex(benchmark::State &state) {
  // Perform setup here
  for (auto _ : state) {
    // This code gets timed
    // folly_mutex().scheduleOn(&folly::InlineExecutor::instance()).start().get();
  }
}

void BM_cppcoro_mutex(benchmark::State &state) {
  // Perform setup here
  cppcoro::static_thread_pool tp{};
  cppcoro::async_mutex m;
  std::array<cppcoro::task<void>, kClients> arr;
  for (auto _ : state) {
    for (int i = 0; i < kClients; ++i) {
      arr[i] = cppcoro_mutex(tp, m);
    }
    for (int i = 0; i < kClients; ++i) {
      cppcoro::sync_wait(arr[i]);
    }
  }
}

} // namespace bench
