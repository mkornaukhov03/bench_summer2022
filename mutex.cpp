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
#include <folly/experimental/coro/Mutex.h>
#include <folly/experimental/coro/Task.h>
#include <folly/experimental/coro/ViaIfAsync.h>

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
constexpr int kClients = 16;
constexpr int kQperClient = 5000;
constexpr int kStateSize = 64;
constexpr int kMod = 1'000'007;

static std::size_t id = 0;

struct State {
  int state[kStateSize]{};
  void reset() {
    for (int i = 0; i < kStateSize; ++i) {
      state[i] = 0;
    }
  }
  void accept(int local) {
    for (int i = 0; i < kStateSize; ++i) {
      state[i] ^= local;
      local = get_next(local);
    }
  }
  static int get_next(int x) { return (x * x + 1) % kMod; }
};

State s;

} // namespace

namespace bench {

yaclib::Future<void> yaclib_mutex(yaclib::IExecutor &tp,
                                  yaclib::AsyncMutex<false> &m) {
  co_await On(tp);
  State s;
  for (int i = 0; i < kQperClient; ++i) {
    co_await m.Lock();
    std::size_t local_id = id;
    id = State::get_next(id);
    co_await m.UnlockOn(tp);
    s.accept(local_id);
  }
}

folly::coro::Task<void> folly_mutex(folly::coro::Mutex &m) {
#if 0 
  for (int i = 0; i < kQperClient; ++i) {
    co_await m.co_lock();
    s.accept();
    m.unlock();
  }
#endif
  co_return;
}

cppcoro::task<void> cppcoro_mutex(cppcoro::static_thread_pool &tp,
                                  cppcoro::async_mutex &m) {
  co_await tp.schedule();
  State s;
  for (int i = 0; i < kQperClient; ++i) {
    co_await m.lock_async();
    std::size_t local_id = id;
    id = State::get_next(id);
    m.unlock();
    s.accept(local_id);
  }
}

void BM_yaclib_mutex(benchmark::State &state) {
  // Perform setup here
  auto tp = yaclib::MakeThreadPool();
  yaclib::AsyncMutex<false> m;
  s.reset();
  for (auto _ : state) {
    auto foo = [&]() -> yaclib::Future<void> {
      std::vector<yaclib::Future<void>> arr(kClients);
      for (int i = 0; i < kClients; ++i) {
        arr[i] = yaclib_mutex(*tp, m);
      }
      co_await Await(arr.begin(), arr.end());
    };
    std::ignore = foo().Get();
  }
  tp->HardStop();
  tp->Wait();
}

void BM_folly_mutex(benchmark::State &state) {
  // Perform setup here

  for (auto _ : state) {
    // This code gets timed
#if 0
  auto foo = [&]()-> folly::coro::Task<void>  {
      std::vector<folly::coro::Task<void>> arr(kClients);
      for (int i = 0; i < kClients; ++i) {
        arr[i] = co_await co_ViaIfAsync(tp, folly_mutex(m));
      }
      co_await Wait(arr.begin(), arr.end());
  };
#endif
  }
}

void BM_cppcoro_mutex(benchmark::State &state) {
  // Perform setup here
  cppcoro::static_thread_pool tp{};
  cppcoro::async_mutex m;
  s.reset();
  for (auto _ : state) {
    auto foo = [&]() -> cppcoro::task<void> {
      std::vector<cppcoro::task<void>> arr(kClients);
      for (int i = 0; i < kClients; ++i) {
        arr[i] = cppcoro_mutex(tp, m);
      }
      co_await cppcoro::when_all(std::move(arr));
    };
    cppcoro::sync_wait(foo());
  }
}

} // namespace bench
