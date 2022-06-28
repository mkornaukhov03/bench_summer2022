#include <cppcoro/async_mutex.hpp>
#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/task.hpp>
#include <cppcoro/when_all.hpp>

#include <yaclib/algo/wait.hpp>
#include <yaclib/coroutine/async_mutex.hpp>
#include <yaclib/coroutine/await.hpp>
#include <yaclib/coroutine/future_traits.hpp>
#include <yaclib/coroutine/on.hpp>
#include <yaclib/executor/inline.hpp>
#include <yaclib/executor/thread_pool.hpp>

#include <benchmark/benchmark.h>

#include <array>
#include <cassert>
#include <tuple>

namespace bench {
namespace {

constexpr std::size_t kClients = 128;
constexpr std::size_t kQperClient = 100;
constexpr std::size_t kStateSize = 6400;
constexpr std::size_t kMod = 1'000'007;

struct State {
  static std::size_t get_next(std::size_t x) noexcept {
    return (x * x + 1) % kMod;
  }

  void accept(std::size_t local) noexcept {
    for (std::size_t &i : state) {
      i ^= local;
      local = get_next(local);
    }
  }

  std::array<std::size_t, kStateSize> state;
};

yaclib::Future<void> yaclib_mutex(yaclib::IExecutor &tp,
                                  yaclib::AsyncMutex<false> &m,
                                  std::size_t &shared_id) {
  co_await On(tp);
  State state{};
  for (std::size_t i = 0; i < kQperClient; ++i) {
    co_await m.Lock();
    const auto old_id = shared_id;
    shared_id = State::get_next(shared_id);
    co_await m.Unlock();
    state.accept(old_id);
  }
}

cppcoro::task<void> cppcoro_mutex(cppcoro::static_thread_pool &tp,
                                  cppcoro::async_mutex &m,
                                  std::size_t &shared_id) {
  co_await tp.schedule();
  State state{};
  for (std::size_t i = 0; i < kQperClient; ++i) {
    co_await m.lock_async();
    const auto old_id = shared_id;
    shared_id = State::get_next(shared_id);
    m.unlock();
    state.accept(old_id);
  }
}

} // namespace

void BM_cppcoro_mutex(benchmark::State &state) {
  cppcoro::static_thread_pool tp{std::thread::hardware_concurrency()};
  cppcoro::async_mutex m;
  std::size_t shared_id = 0;
  for (auto _ : state) {
    auto all = [&]() -> cppcoro::task<void> {
      std::vector<cppcoro::task<void>> tasks;
      tasks.reserve(kClients);
      for (std::size_t i = 0; i < kClients; ++i) {
        tasks.push_back(cppcoro_mutex(tp, m, shared_id));
      }
      co_await cppcoro::when_all_ready(std::move(tasks));
    };
    cppcoro::sync_wait(all());
  }
}

void BM_yaclib_mutex(benchmark::State &state) {
  auto tp = yaclib::MakeThreadPool(std::thread::hardware_concurrency());
  yaclib::AsyncMutex<false> m;
  std::size_t shared_id = 0;
  for (auto _ : state) {
    auto all = [&]() -> yaclib::Future<void> {
      std::vector<yaclib::Future<void>> tasks;
      tasks.reserve(kClients);
      for (std::size_t i = 0; i < kClients; ++i) {
        tasks.push_back(yaclib_mutex(*tp, m, shared_id));
      }
      co_await Await(tasks.begin(), tasks.end());
    };
    std::ignore = all().Get();
  }
  tp->HardStop();
  tp->Wait();
}

} // namespace bench
