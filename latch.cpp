#include <cppcoro/async_latch.hpp>
#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>
#include <cppcoro/when_all.hpp>

#include <yaclib/algo/wait.hpp>
#include <yaclib/coroutine/await_group.hpp>
#include <yaclib/coroutine/future_traits.hpp>
#include <yaclib/coroutine/on.hpp>
#include <yaclib/executor/inline.hpp>
#include <yaclib/executor/thread_pool.hpp>

#include <benchmark/benchmark.h>

#include <array>
#include <cassert>
#include <iostream>
#include <tuple>
#include <utility>

namespace bench {
namespace {

cppcoro::task<void> cppcoro_done(cppcoro::static_thread_pool &tp,
                                 cppcoro::async_latch &wg) {
  co_await tp.schedule();
  wg.count_down();
}

cppcoro::task<void> cppcoro_await(cppcoro::static_thread_pool &tp,
                                  cppcoro::async_latch &wg) {
  co_await tp.schedule();
  co_await wg;
}

yaclib::Future<void> yaclib_done(yaclib::IExecutor &tp,
                                 yaclib::AwaitGroup &wg) {
  co_await On(tp);
  wg.Done();
}

yaclib::Future<void> yaclib_await(yaclib::IExecutor &tp,
                                  yaclib::AwaitGroup &wg) {
  co_await On(tp);
  co_await wg;
}

} // namespace

void BM_cppcoro_latch(benchmark::State &state) {
  const std::size_t tasks_count = state.range(0);
  cppcoro::static_thread_pool tp{std::thread::hardware_concurrency()};
  for (auto _ : state) {
    cppcoro::async_latch wg{static_cast<ptrdiff_t>(tasks_count)};
    auto all = [&]() -> cppcoro::task<void> {
      std::vector<cppcoro::task<void>> tasks;
      tasks.reserve(tasks_count * 2);
      for (std::size_t i = 0; i < tasks_count * 2; ++i) {
        if (i % 2 == 0) {
          tasks.push_back(cppcoro_await(tp, wg));
        } else {
          tasks.push_back(cppcoro_done(tp, wg));
        }
      }
      co_await cppcoro::when_all_ready(std::move(tasks));
    };
    cppcoro::sync_wait(all());
  }
}

void BM_yaclib_latch(benchmark::State &state) {
  const std::size_t tasks_count = state.range(0);
  auto tp = yaclib::MakeThreadPool(std::thread::hardware_concurrency());
  for (auto _ : state) {
    yaclib::AwaitGroup wg;
    wg.Add(tasks_count);
    auto all = [&]() -> yaclib::Future<void> {
      for (std::size_t i = 0; i < tasks_count * 2; ++i) {
        if (i % 2 == 0) {
          yaclib_await(*tp, wg).Detach();
        } else {
          yaclib_done(*tp, wg).Detach();
        }
      }
      co_await wg;
    };
    std::ignore = all().Get();
  }
  tp->HardStop();
  tp->Wait();
}

/*
void BM_yaclib_latch2(benchmark::State &state) {
  const std::size_t tasks_count = state.range(0);
  auto tp = yaclib::MakeThreadPool(std::thread::hardware_concurrency());
  for (auto _ : state) {
    yaclib::AwaitGroup wg;
    wg.Add(tasks_count);
    auto all = [&]() -> yaclib::Future<void> {
      std::vector<yaclib::Future<void>> tasks;
      tasks.reserve(tasks_count * 2);
      for (std::size_t i = 0; i < tasks_count * 2; ++i) {
        if (i % 2 == 0) {
          tasks.push_back(yaclib_await(*tp, wg));
        } else {
          tasks.push_back(yaclib_done(*tp, wg));
        }
      }
      co_await Await(tasks.begin(), tasks.end());
    };
    std::ignore = all().Get();
  }
  tp->HardStop();
  tp->Wait();
}
*/

} // namespace bench
