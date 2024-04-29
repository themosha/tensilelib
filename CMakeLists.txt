cmake_minimum_required(VERSION 3.20)
project(tensile)

set(CMAKE_CXX_STANDARD 17)

# Configure tensile library
add_subdirectory(argh)
set(TENSILE_SOURCES
    features.cpp
    tensile.cpp)
add_library(tensilelib ${TENSILE_SOURCES})

# Tests - require defining TENSILE_ENABLE_TESTS (in order not to conflict with popular googletest)
if (TENSILE_ENABLE_TESTS)
  add_subdirectory(googletest)
  add_executable(tensile_test features_test.cpp ${TENSILE_SOURCES} tensile_test.cpp)
  target_link_libraries(tensile_test gtest gmock)
endif()
