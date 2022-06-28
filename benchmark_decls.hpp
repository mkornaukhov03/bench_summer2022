#pragma once

#include <benchmark/benchmark.h>

namespace bench {
void BM_yaclib_collatz(benchmark::State &state);
void BM_folly_collatz(benchmark::State &state);
void BM_cppcoro_collatz(benchmark::State &state);

void BM_yaclib_reschedule(benchmark::State &state);
void BM_folly_reschedule(benchmark::State &state);
void BM_cppcoro_reschedule(benchmark::State &state);

void BM_yaclib_mutex(benchmark::State &state);
void BM_folly_mutex(benchmark::State &state);
void BM_cppcoro_mutex(benchmark::State &state);

void BM_yaclib_latch(benchmark::State &state);
void BM_folly_latch(benchmark::State &state);
void BM_cppcoro_latch(benchmark::State &state);
} // namespace bench
