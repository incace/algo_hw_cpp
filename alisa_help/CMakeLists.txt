cmake_minimum_required(VERSION 3.14)
project(AlisaBot CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Static library (parser + dungeon logic) ───────────────────────────────────
add_library(alisa_lib STATIC
    src/dungeon.cpp
    src/parse.cpp
)
target_include_directories(alisa_lib PUBLIC include)

# ── Main executable ───────────────────────────────────────────────────────────
add_executable(alisa_bot src/main.cpp)
target_link_libraries(alisa_bot PRIVATE alisa_lib)

# ── Tests ─────────────────────────────────────────────────────────────────────
enable_testing()
add_subdirectory(tests)
