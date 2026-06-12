cmake_minimum_required(VERSION 3.20)
project(CultivationFortress VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies: only nlohmann/json (serialization). ---
# The simulation core is fully decoupled from any UI/rendering library.
include(FetchContent)

# Reuse the already-downloaded copy if present (offline-friendly)
if(EXISTS "${CMAKE_SOURCE_DIR}/build2/_deps/json-src/CMakeLists.txt")
  set(FETCHCONTENT_SOURCE_DIR_JSON "${CMAKE_SOURCE_DIR}/build2/_deps/json-src" CACHE PATH "" FORCE)
endif()
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(json)

# --- Core simulation library (no UI dependencies whatsoever) ---
add_library(cfs_core STATIC
    src/SimEngine.cpp
    src/Body.cpp
    src/Combat.cpp
    src/GameData.cpp
    src/GameDataLoader.cpp
    src/GameMap.cpp
    src/HistoryGenerator.cpp
)
target_include_directories(cfs_core PUBLIC include)
target_link_libraries(cfs_core PUBLIC nlohmann_json::nlohmann_json)

# --- Text-mode front-end ---
add_executable(CultivationFortress src/Main.cpp)
target_link_libraries(CultivationFortress PRIVATE cfs_core)

# Run from the project root so the .md databases and SaveData/ resolve
set_target_properties(CultivationFortress PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

if(MSVC)
    target_compile_options(cfs_core PRIVATE /W4)
    target_compile_options(CultivationFortress PRIVATE /W4)
else()
    target_compile_options(cfs_core PRIVATE -Wall -Wextra)
    target_compile_options(CultivationFortress PRIVATE -Wall -Wextra)
endif()

# --- Graphical front-end (SFML 3 + Dear ImGui) ---
option(BUILD_GUI "Build the SFML/ImGui graphical front-end" ON)
if(BUILD_GUI)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        SFML
        URL https://github.com/SFML/SFML/archive/refs/tags/3.0.0.zip
    )
    FetchContent_MakeAvailable(SFML)

    FetchContent_Declare(
        imgui
        URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.9.zip
    )
    FetchContent_MakeAvailable(imgui)

    set(IMGUI_DIR ${imgui_SOURCE_DIR} CACHE PATH "" FORCE)
    set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        imgui-sfml
        URL https://github.com/SFML/imgui-sfml/archive/refs/tags/v3.0.zip
    )
    FetchContent_MakeAvailable(imgui-sfml)

    add_executable(CultivationFortressGUI src/MainGui.cpp)
    target_link_libraries(CultivationFortressGUI PRIVATE cfs_core ImGui-SFML::ImGui-SFML)
    set_target_properties(CultivationFortressGUI PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()
