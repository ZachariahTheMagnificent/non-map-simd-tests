cmake_minimum_required(VERSION 3.11)
project(non-map-simd-tests VERSION 1.0.0 DESCRIPTION "A collection of tests that benchmarks the effectiveness of simd with non-map access patterns" LANGUAGES CXX)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vector-length-squared")
