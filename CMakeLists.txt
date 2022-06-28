cmake_minimum_required(VERSION 3.7)
project(Bench)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_VISIBILITY_PRESET hidden)

set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
find_package(folly REQUIRED)
find_package(gflags REQUIRED)
find_package(benchmark QUIET)

if (NOT BENCHMARK_FOUND)
  if (NOT BENCHMARK_FOUND)
    set(BENCHMARK_ENABLE_TESTING NO)
    FetchContent_Declare(
      benchmark
      GIT_REPOSITORY https://github.com/google/benchmark.git
      GIT_TAG main
    )
    FetchContent_MakeAvailable(benchmark)
  endif ()
endif ()

# add cppcoro
include(FetchContent)
FetchContent_Declare(cppcoro
  GIT_REPOSITORY https://github.com/andreasbuhr/cppcoro.git
  GIT_TAG main
  )
FetchContent_MakeAvailable(cppcoro)

# add yaclib
set(YACLIB_FLAGS CORO)
FetchContent_Declare(yaclib
  GIT_REPOSITORY https://github.com/YACLib/YACLib.git
  GIT_TAG add-task2-rebase
  )
FetchContent_MakeAvailable(yaclib)

# sources
set(SOURCES main.cpp
  collatz.cpp
  reschedule.cpp
  mutex.cpp
  latch.cpp
  )
set(HEADERS benchmark_decls.hpp)

add_executable(Bench ${SOURCES} ${HEADERS})

target_link_libraries(Bench ${Boost_LIBRARIES} yaclib cppcoro
  benchmark::benchmark benchmark::benchmark_main
  yaclib cppcoro
  )

