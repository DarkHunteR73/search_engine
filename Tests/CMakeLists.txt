cmake_minimum_required(VERSION 3.21)

enable_testing()

add_executable(${PROJECT_NAME}_test test.cpp)
target_include_directories(${PROJECT_NAME}_test PUBLIC ${CMAKE_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)