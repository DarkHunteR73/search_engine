cmake_minimum_required(VERSION 3.21)

set(PROJECT_SOURCES main.cpp ../include/converterJSON.h converterJSON.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
