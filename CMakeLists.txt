cmake_minimum_required(VERSION 3.7)
project(Bench)

find_package(Boost REQUIRED)
find_package(folly REQUIRED)
find_package(Threads REQUIRED)
find_package(gflags REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_VISIBILITY_PRESET hidden)

set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS O)

# add cppcoro
include(FetchContent)
FetchContent_Declare(cppcoro
  GIT_REPOSITORY https://github.com/mkornaukhov03/cppcoro.git 
  GIT_TAG master
  )
FetchContent_MakeAvailable(cppcoro)



# add yaclib 
set(YACLIB_FLAGS CORO)
include(FetchContent)
FetchContent_Declare(yaclib
  GIT_REPOSITORY https://github.com/YACLib/YACLib.git
  GIT_TAG main
  )
FetchContent_MakeAvailable(yaclib)
link_libraries(yaclib)

link_libraries(cppcoro)

#add folly

if (NOT TARGET Folly::folly)
  include("${FOLLY_CMAKE_DIR}/folly-targets.cmake")
endif()

set(FOLLY_LIBRARIES Folly::folly)

if (NOT folly_FIND_QUIETLY)
  message(STATUS "Found folly: ${PACKAGE_PREFIX_DIR}")
endif()



# add google benchmark
find_package(benchmark REQUIRED)

# sources
set (SOURCES main.cpp
  collatz.cpp
  reschedule.cpp
  mutex.cpp
)
set (HEADERS benchmark_decls.hpp)

add_executable(Bench ${SOURCES} ${HEADERS})
target_include_directories(
  Bench
  PUBLIC
  /Users/mkornaukhov/Projects/folly/installed/include
)


target_link_libraries(Bench ${Boost_LIBRARIES} yaclib cppcoro
  ${FOLLY_LIBRARIES}
  benchmark::benchmark benchmark::benchmark_main
)
#target_link_libraries(Bench "/Users/mkornaukhov/Projects/folly/installed/lib/libfolly.a")

