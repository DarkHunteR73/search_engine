cmake_minimum_required(VERSION 3.21)

add_executable(${PROJECT_NAME} main.cpp ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDE_DIR})
