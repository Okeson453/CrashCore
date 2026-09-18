cmake_minimum_required(VERSION 3.20)
project(predictions-engine-cpp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(PE_ENABLE_ML "Build the optional ML scoring path" OFF)

set(PE_SOURCES src/state/incremental_state_engine.cpp)
if(PE_ENABLE_ML)
  list(APPEND PE_SOURCES
    src/ml/ml_preprocessor.cpp src/ml/ml_normalizer.cpp src/ml/ml_inference.cpp
    src/ml/ml_calibrator.cpp src/ml/ml_fusion.cpp src/ml/ml_health.cpp
    src/ml/ml_metrics.cpp src/ml/ml_model_loader.cpp src/ml/ml_runtime.cpp
    src/ml/ml_predictor.cpp src/ml/ml_sha256.cpp
    src/models/ml_predictive_model.cpp)
endif()

add_library(pe STATIC ${PE_SOURCES})
target_include_directories(pe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(PE_ENABLE_ML)
  target_compile_definitions(pe PUBLIC PE_ENABLE_ML)
endif()

add_executable(pe_demo examples/demo.cpp)
target_link_libraries(pe_demo PRIVATE pe)

enable_testing()

# Core regression tests (no external test framework)
foreach(t pe_smoke test_incremental_state_engine test_baseline_model
          test_gap_features test_prediction_engine)
  if(t STREQUAL "pe_smoke")
    add_executable(${t} tests/smoke.cpp)
  else()
    add_executable(${t} tests/${t}.cpp)
  endif()
  target_link_libraries(${t} PRIVATE pe)
  add_test(NAME ${t} COMMAND ${t})
endforeach()

if(PE_ENABLE_ML)
  foreach(t ml_preprocessor_test ml_inference_test ml_model_loader_test
            ml_predictor_test ml_calibration_test ml_fusion_test ml_latency_test)
    add_executable(${t} tests/${t}.cpp)
    target_link_libraries(${t} PRIVATE pe)
    add_test(NAME ${t} COMMAND ${t})
  endforeach()
  add_executable(pe_ml_tests tests/ml_tests.cpp)
  target_link_libraries(pe_ml_tests PRIVATE pe)
  add_test(NAME pe_ml_tests COMMAND pe_ml_tests)
endif()
