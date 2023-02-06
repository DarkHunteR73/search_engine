cmake_minimum_required(VERSION 3.21)

enable_testing()

add_executable(${PROJECT_NAME}_test tests.cpp ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME}_test PUBLIC ${PROJECT_INCLUDE_DIR})

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)