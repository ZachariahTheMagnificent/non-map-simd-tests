cmake_minimum_required(VERSION 3.11)
project(vector-length-squared VERSION 1.0.0 DESCRIPTION "A benchmark on an simd implementation of an operation that calculates the lengths of an array of vectors squared." LANGUAGES CXX)

add_executable(vector-length-squared)
target_compile_features(vector-length-squared PRIVATE cxx_std_17)
target_include_directories(vector-length-squared PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
#target_compile_definitions(vector-length-squared PRIVATE SIMD)
target_sources(vector-length-squared PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp")

add_executable(vector-length-squared-simd)
target_compile_features(vector-length-squared-simd PRIVATE cxx_std_17)
target_include_directories(vector-length-squared-simd PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_definitions(vector-length-squared-simd PRIVATE SIMD)
target_sources(vector-length-squared-simd PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp")
