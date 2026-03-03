cmake_minimum_required(VERSION 3.20)
project(CultivationFortress VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define sources
set(SOURCES
    src/Main.cpp
    src/SimEngine.cpp
    src/Database.cpp
    src/WorldMap.cpp
    src/WorldGen.cpp
    src/HistoryGenerator.cpp
    src/EventManager.cpp
    src/NPCGenerator.cpp
)

# Dependencies
include(FetchContent)

# SFML
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(SFML)

# Create Alias Targets for ImGui-SFML
if(NOT TARGET SFML::Graphics)
    add_library(SFML::Graphics ALIAS sfml-graphics)
    add_library(SFML::Window ALIAS sfml-window)
    add_library(SFML::System ALIAS sfml-system)
endif()

# ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)

# ImGui-SFML
set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)
set(IMGUI_DIR ${imgui_SOURCE_DIR} CACHE PATH "" FORCE)
FetchContent_Declare(
  ImGui-SFML
  GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(ImGui-SFML)
target_compile_definitions(ImGui-SFML PUBLIC "ImDrawIdx=unsigned int")
target_link_libraries(ImGui-SFML PUBLIC sfml-graphics sfml-window sfml-system)

# SQLite3 Amalgamation
FetchContent_Declare(
  sqlite3
  URL https://www.sqlite.org/2023/sqlite-amalgamation-3440200.zip
)
FetchContent_GetProperties(sqlite3)
if(NOT sqlite3_POPULATED)
  FetchContent_Populate(sqlite3)
  add_library(sqlite3 STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c)
  target_include_directories(sqlite3 PUBLIC ${sqlite3_SOURCE_DIR})
endif()

# Define executable
add_executable(CultivationFortress ${SOURCES})

# Link Libraries
target_link_libraries(CultivationFortress PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system 
    ImGui-SFML
    sqlite3
    nlohmann_json::nlohmann_json
)

# JSON
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(json)

target_include_directories(CultivationFortress PRIVATE include)

if(MSVC)
    target_compile_options(CultivationFortress PRIVATE /W4)
else()
    target_compile_options(CultivationFortress PRIVATE -Wall -Wextra -Werror)
endif()
