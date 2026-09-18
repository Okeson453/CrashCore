cmake_minimum_required(VERSION 3.20)
project(CrashCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CRASHCORE_BUILD_TESTS "Build tests" ON)
option(CRASHCORE_BUILD_BENCH "Build benchmarks" ON)
option(CRASHCORE_BUILD_TOOLS "Build tools" ON)

include(cmake/Dependencies.cmake)
include(cmake/CompilerOptions.cmake)

file(GLOB_RECURSE CRASHCORE_SOURCES CONFIGURE_DEPENDS
  src/application/*.cpp
  src/orchestration/*.cpp
  src/ingestion/*.cpp
  src/ingestion/*/*.cpp
  src/prediction_interface/*.cpp
  src/validation/*.cpp
  src/delivery/*.cpp
  src/delivery/*/*.cpp
  src/persistence/*.cpp
  src/persistence/*/*.cpp
  src/concurrency/*.cpp
  src/timing/*.cpp
  src/telemetry/*.cpp
  src/logging/*.cpp
  src/security/*.cpp
  src/api/*.cpp
  src/config/*.cpp
  src/live/*.cpp
  src/feedback/*.cpp
)


# Predictions Engine (in-process N+1)
option(CRASHCORE_ENABLE_PE_ML "Enable PE optional ML path" OFF)
set(PE_ENABLE_ML ${CRASHCORE_ENABLE_PE_ML} CACHE BOOL "" FORCE)
add_subdirectory(third_party/predictions-engine)

add_library(crashcore_lib STATIC ${CRASHCORE_SOURCES})
target_include_directories(crashcore_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/predictions-engine/include
  ${Boost_INCLUDE_DIRS}
  ${PostgreSQL_INCLUDE_DIRS}
  ${CURL_INCLUDE_DIRS}
)
target_link_libraries(crashcore_lib PUBLIC
  OpenSSL::SSL OpenSSL::Crypto
  Threads::Threads
  Boost::system
  ${PostgreSQL_LIBRARIES}
  ${CURL_LIBRARIES}
  pe
)
target_compile_definitions(crashcore_lib PUBLIC CRASHCORE_HAS_PE=1)
target_compile_definitions(crashcore_lib PUBLIC CRASHCORE_HAS_BEAST=1 CRASHCORE_HAS_LIBPQ=1 CRASHCORE_HAS_CURL=1)

add_executable(CrashCore src/main.cpp)
target_link_libraries(CrashCore PRIVATE crashcore_lib)

if(CRASHCORE_BUILD_TESTS)
  enable_testing()
  # Only sources without their own main() — standalone tests are separate targets
  set(CRASHCORE_TEST_SOURCES
    tests/test_main.cpp
    tests/phase0_wiring_test.cpp
    tests/phase12_durable_acie_test.cpp
    tests/phase34_feedback_obs_test.cpp
    tests/phase_reverify_test.cpp
  )
  # Optional standalone test binaries (each has its own main)
  set(CRASHCORE_STANDALONE_TESTS
    tests/integration/end_to_end.cpp
    tests/integration/pipeline_engine_test.cpp
    tests/validation/validator_tests.cpp
  )
  # Only add sources that exist
  set(CRASHCORE_TEST_SOURCES_EXISTING "")
  foreach(src ${CRASHCORE_TEST_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
      list(APPEND CRASHCORE_TEST_SOURCES_EXISTING ${src})
    endif()
  endforeach()
  add_executable(crashcore_tests ${CRASHCORE_TEST_SOURCES_EXISTING})
  target_link_libraries(crashcore_tests PRIVATE crashcore_lib)
  add_test(NAME crashcore_tests COMMAND crashcore_tests)
  foreach(src ${CRASHCORE_STANDALONE_TESTS})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
      get_filename_component(tname "${src}" NAME_WE)
      add_executable(crashcore_${tname} ${src})
      target_link_libraries(crashcore_${tname} PRIVATE crashcore_lib)
      add_test(NAME crashcore_${tname} COMMAND crashcore_${tname})
    endif()
  endforeach()
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_production_wiring.cpp")
    add_executable(crashcore_production_wiring tests/test_production_wiring.cpp)
    target_link_libraries(crashcore_production_wiring PRIVATE crashcore_lib)
    add_test(NAME crashcore_production_wiring COMMAND crashcore_production_wiring)
  endif()
endif()

if(CRASHCORE_BUILD_BENCH)
  add_executable(crashcore_bench benchmarks/end_to_end_latency.cpp)
  target_link_libraries(crashcore_bench PRIVATE crashcore_lib)
endif()

if(CRASHCORE_BUILD_TOOLS)
  add_executable(crashcore_replay tools/replay/replay.cpp)
  target_link_libraries(crashcore_replay PRIVATE crashcore_lib)
  add_executable(crashcore_latency_probe tools/diagnostics/latency_probe.cpp)
  target_link_libraries(crashcore_latency_probe PRIVATE crashcore_lib)
endif()
