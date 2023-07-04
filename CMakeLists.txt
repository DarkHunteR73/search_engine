cmake_minimum_required(VERSION 3.2)
project(search_engine VERSION 0.1)

if (WIN32)
    set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.12.12/5.12.12/msvc2017_64/lib/cmake")
endif ()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0079 NEW)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

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
                    ${PROJECT_SRC_DIR}/threadPool.cpp
)

set(PROJECT_HEADERS ${PROJECT_INCLUDE_DIR}/jsonConverter.h
                    ${PROJECT_INCLUDE_DIR}/invertedIndex.h
                    ${PROJECT_INCLUDE_DIR}/searchServer.h
                    ${PROJECT_INCLUDE_DIR}/threadPool.h
                    ${PROJECT_INCLUDE_DIR}/mainwindow.h)

add_executable(${PROJECT_NAME}_cli ${PROJECT_SRC_DIR}/main.cpp
                ${PROJECT_SOURCES}
)

add_executable(${PROJECT_NAME}_gui ${PROJECT_SRC_DIR}/main_gui.cpp
                ${PROJECT_SOURCES}
                ${PROJECT_HEADERS}
                ${PROJECT_SRC_DIR}/mainwindow.ui
                ${PROJECT_SRC_DIR}/mainwindow.cpp)

target_include_directories(${PROJECT_NAME}_cli PUBLIC ${PROJECT_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME}_gui PUBLIC ${PROJECT_INCLUDE_DIR})

add_subdirectory(Tests)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/ec25eea8f8237cf86c30703f59747e42f34b6f75.zip)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
set(gtest_disable_pthreads on)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(${PROJECT_NAME}_cli PRIVATE nlohmann_json::nlohmann_json)

target_link_libraries(${PROJECT_NAME}_gui PRIVATE
    nlohmann_json::nlohmann_json
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
)

target_link_libraries(${PROJECT_NAME}_test PRIVATE nlohmann_json::nlohmann_json GTest::gtest_main)
target_compile_definitions(${PROJECT_NAME}_test PUBLIC TEST_MODE)

if (WIN32)
    set(DEBUG_SUFFIX)
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
                set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME}_gui POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}_gui>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME}_gui POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
            "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}_gui>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME}_gui POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}_gui>")
    endforeach (QT_LIB)
endif ()
