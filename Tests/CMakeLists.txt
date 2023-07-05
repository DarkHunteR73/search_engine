cmake_minimum_required(VERSION 3.2)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/ec25eea8f8237cf86c30703f59747e42f34b6f75.zip)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
set(gtest_disable_pthreads on)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()

add_executable(${PROJECT_NAME}_test tests.cpp ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME}_test PUBLIC ${PROJECT_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}_test PRIVATE nlohmann_json::nlohmann_json GTest::gtest_main)
target_compile_definitions(${PROJECT_NAME}_test PUBLIC TEST_MODE)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)