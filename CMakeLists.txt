cmake_minimum_required(VERSION 3.2)
project(search_engine VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0079 NEW)

add_definitions(-DENGINE_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
                -DENGINE_VERSION_MINOR=${PROJECT_VERSION_MINOR}
                -DPROJECT_ROOT="${CMAKE_SOURCE_DIR}/")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

set(JSON_BuildTests OFF CACHE INTERNAL "")

set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(PROJECT_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_SOURCES ${PROJECT_SRC_DIR}/jsonConverter.cpp
                    ${PROJECT_SRC_DIR}/invertedIndex.cpp
                    ${PROJECT_SRC_DIR}/searchServer.cpp
                    ${PROJECT_SRC_DIR}/threadPool.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SRC_DIR}/main.cpp ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDE_DIR})

add_subdirectory(Tests)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/ec25eea8f8237cf86c30703f59747e42f34b6f75.zip)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
set(gtest_disable_pthreads on)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME}_test PRIVATE nlohmann_json::nlohmann_json GTest::gtest_main)
target_compile_definitions(${PROJECT_NAME}_test PUBLIC TEST_MODE)
