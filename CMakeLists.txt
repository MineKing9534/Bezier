cmake_minimum_required(VERSION 3.27)
project(Bezier
        LANGUAGES CXX
        VERSION 1.0.0
        DESCRIPTION "TUI-Renderer for bezier curves"
)

set(CMAKE_CXX_STANDARD 17)

if (DEFINED ENV{WINDOWS_BUILD})
    message("Windows build detected")
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif ()

include(FetchContent)

FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/MineKing9534/ftxui
        GIT_TAG 5c82aa986fa3de563993028a991653963f6a96a8
)

FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)

FetchContent_GetProperties(ftxui)
if (NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_library(bezier src/bezier.cc src/bezier.h)
target_link_libraries(bezier)

add_library(display src/display.cc src/display.h)
target_link_libraries(display
        PUBLIC bezier

        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PUBLIC ftxui::component
)

add_library(storage src/storage.cc src/storage.h)
target_link_libraries(storage
        PUBLIC display
        PUBLIC bezier

        PUBLIC nlohmann_json::nlohmann_json
)

add_library(components src/components.cc src/components.h src/scroller.cc src/scroller.h)
target_link_libraries(components
        PUBLIC display

        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)

add_executable(main src/main.cc)
target_link_libraries(main
        PRIVATE components
        PRIVATE storage

        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)