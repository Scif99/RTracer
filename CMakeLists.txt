cmake_minimum_required(VERSION 3.15)

project(
  WhittedRayTracer
  VERSION 1.0
  LANGUAGES CXX
)

# ============================================================================
# compile using C++20
# ============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

