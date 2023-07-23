cmake_minimum_required(VERSION 3.5)
project(search_engine VERSION 0.1)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#cmake_policy(SET CMP0079 NEW)

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
                    ${PROJECT_INCLUDE_DIR}/mainwindow.h
                    ${PROJECT_INCLUDE_DIR}/requesteditor.h)

set(UI_SOURCES ${PROJECT_SRC_DIR}/mainwindow.ui
               ${PROJECT_SRC_DIR}/mainwindow.cpp
               ${PROJECT_SRC_DIR}/requesteditor.ui
               ${PROJECT_SRC_DIR}/requesteditor.cpp)

add_executable(${PROJECT_NAME}_cli ${PROJECT_SRC_DIR}/main.cpp
                                   ${PROJECT_SOURCES}
)

add_executable(${PROJECT_NAME}_gui WIN32 ${PROJECT_SRC_DIR}/main_gui.cpp
                                         ${PROJECT_HEADERS}
                                         ${PROJECT_SOURCES}
                                         ${UI_SOURCES}
)

target_include_directories(${PROJECT_NAME}_cli PUBLIC ${PROJECT_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME}_gui PUBLIC ${PROJECT_INCLUDE_DIR})

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_subdirectory(Tests)
endif()

target_link_libraries(${PROJECT_NAME}_cli PRIVATE nlohmann_json::nlohmann_json)

target_link_libraries(${PROJECT_NAME}_gui PRIVATE
    nlohmann_json::nlohmann_json
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
)

if (WIN32)
    set(DEBUG_SUFFIX)

    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()

    set(QT_INSTALL_PATH "${Qt5_DIR}/..")
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
