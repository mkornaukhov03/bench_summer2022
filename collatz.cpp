#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>

#include <yaclib/algo/wait.hpp>
#include <yaclib/coroutine/await.hpp>
#include <yaclib/coroutine/future_traits.hpp>

#include <benchmark/benchmark.h>

namespace bench {
namespace {

cppcoro::task<void> cppcoro_collatz(size_t x) {
  if (x <= 1) {
    co_return;
  }
  if (x % 2 == 0) {
    co_await cppcoro_collatz(x / 2);
    co_return;
  }
  co_await cppcoro_collatz(3 * x + 1);
  co_return;
}

yaclib::Future<void> yaclib_collatz(size_t x) {
  if (x <= 1) {
    co_return;
  }
  if (x % 2 == 0) {
    auto foo = yaclib_collatz(x / 2);
    co_await Await(foo);
    co_return;
  }
  auto foo = yaclib_collatz(3 * x + 1);
  co_await Await(foo);
  co_return;
}

} // namespace

void BM_cppcoro_collatz(benchmark::State &state) {
  const size_t n = state.range(0);
  for (auto _ : state) {
    cppcoro::sync_wait(cppcoro_collatz(n));
  }
}

void BM_yaclib_collatz(benchmark::State &state) {
  const size_t n = state.range(0);
  for (auto _ : state) {
    auto task = yaclib_collatz(n);
    Wait(task);
  }
}

} // namespace bench
