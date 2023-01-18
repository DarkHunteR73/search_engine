cmake_minimum_required(VERSION 3.21)
project(search_engine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -DENGINE_VERSION_MAJOR=${PROJECT_VERSION_MAJOR} -DENGINE_VERSION_MINOR=${PROJECT_VERSION_MINOR}")

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(src)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
