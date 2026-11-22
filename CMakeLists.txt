cmake_minimum_required(VERSION 3.21)

project(dungeon_delve_core
        VERSION 0.1.0
        DESCRIPTION "Dungeon Delve encounter engine"
        LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Language standard.
#
# The course targets C++20 as the floor. Your textbook is written to C++23;
# where the book uses something your compiler does not have yet, the course
# materials say so. Do not lower this.
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# ---------------------------------------------------------------------------
# Warnings. These are on from Week 1 and they stay on.
# ---------------------------------------------------------------------------
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------------------------------------------
# Sanitizers.
#
# DD_SANITIZE is ON in the course-debug preset. The address sanitizer is what
# finds the memory problems in this codebase; it reports a leak, an
# out-of-bounds read, or a use-after-free at the moment it happens, with a
# stack trace, instead of as a mystery crash later.
#
# Not supported under MSVC in this configuration -- Windows students should
# use the course-debug-msvc preset and read the note in README.md.
# ---------------------------------------------------------------------------
option(DD_SANITIZE "Build with address and undefined-behavior sanitizers" OFF)

add_library(dd_sanitize_flags INTERFACE)

if(DD_SANITIZE AND NOT MSVC)
    target_compile_options(dd_sanitize_flags INTERFACE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -g)

    # Clang can always detect reads of a dead stack frame. GCC does it at
    # runtime instead, via ASAN_OPTIONS=detect_stack_use_after_return=1 --
    # see README.md. Only add the flag if this compiler understands it.
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-fsanitize-address-use-after-return=always"
                            DD_HAS_UAR_FLAG)
    if(DD_HAS_UAR_FLAG)
        target_compile_options(dd_sanitize_flags INTERFACE
            -fsanitize-address-use-after-return=always)
    endif()
    target_link_options(dd_sanitize_flags INTERFACE
        -fsanitize=address,undefined)
endif()

# ---------------------------------------------------------------------------
# The engine, built twice.
#
# dd_engine is the engine compiled with DD_NO_MAIN so tests can link it.
# The Build 4 object library ensures self-registering extension code is linked.
# dungeon_delve is the playable program.
#
# Yes, compiling the same source file two different ways is an odd thing to
# find in a codebase. Make a note of it for TRIAGE.md.
# ---------------------------------------------------------------------------
# CONFIGURE_DEPENDS discovers future abilities without edits to existing files.
file(GLOB DD_EXTENSION_ABILITIES CONFIGURE_DEPENDS src/abilities/*.cpp)
set(DD_ABILITY_SOURCES src/ability_system.cpp ${DD_EXTENSION_ABILITIES})
add_library(dd_ability_plugins OBJECT ${DD_EXTENSION_ABILITIES})
target_include_directories(dd_ability_plugins PRIVATE include)
add_library(dd_engine OBJECT src/dungeon_delve.cpp src/domain_types.cpp src/entity_hierarchy.cpp src/ability_system.cpp)
target_sources(dd_engine PUBLIC $<TARGET_OBJECTS:dd_ability_plugins>)
target_include_directories(dd_engine PUBLIC include)
target_compile_definitions(dd_engine PRIVATE DD_NO_MAIN)
target_link_libraries(dd_engine PUBLIC dd_sanitize_flags dd_ability_plugins)

add_executable(dungeon_delve src/dungeon_delve.cpp src/entity_hierarchy.cpp src/ability_system.cpp $<TARGET_OBJECTS:dd_ability_plugins>)
target_include_directories(dungeon_delve PRIVATE include)
target_link_libraries(dungeon_delve PRIVATE dd_sanitize_flags)

# ---------------------------------------------------------------------------
# Concept Lab programs.
#
# These are small standalone programs, one per week, that practice the week's
# idea outside the engine. They are not part of the game and nothing in the
# engine depends on them.
# ---------------------------------------------------------------------------
add_executable(ownership_audit labs/week01_ownership_audit/ownership_audit.cpp)
target_link_libraries(ownership_audit PRIVATE dd_sanitize_flags)

# ---------------------------------------------------------------------------
# Tests.
# ---------------------------------------------------------------------------
enable_testing()

add_executable(regression tests/regression.cpp)
target_link_libraries(regression PRIVATE dd_engine dd_sanitize_flags)

add_test(NAME encounter_resolves
         COMMAND regression encounter_resolves)

add_test(NAME loot_table_intact
         COMMAND regression loot_table_intact)

add_test(NAME roster_survives_reallocation
         COMMAND regression roster_survives_reallocation)

# Leak detection is deliberately OFF for these three tests.
#
# Each test targets exactly one of the three seeded defects. This codebase
# leaks almost everything it allocates, and if leak reports failed these tests
# too you could not tell which problem you were looking at.
#
# The ownership half of Build 1 is checked separately, by running the game
# itself under the sanitizer:
#
#     ./build/course-debug/dungeon_delve --quiet
#
# That run must report no leaks and no invalid accesses before you submit.
set_tests_properties(encounter_resolves loot_table_intact roster_survives_reallocation
    PROPERTIES
        ENVIRONMENT "ASAN_OPTIONS=detect_leaks=0:abort_on_error=0"
        TIMEOUT 60)

# Build 2: typed domain vocabulary, demo and focused tests.
add_executable(build02_demo src/build02_demo.cpp)
target_link_libraries(build02_demo PRIVATE dd_engine)

add_executable(build02_tests tests/build02.cpp)
target_link_libraries(build02_tests PRIVATE dd_engine)
add_test(NAME build02_value_types COMMAND build02_tests)

# Build 3: polymorphic entity hierarchy and diamond inheritance checks.
add_executable(build03_demo src/build03_demo.cpp)
target_link_libraries(build03_demo PRIVATE dd_engine)

add_executable(build03_tests tests/build03.cpp)
target_link_libraries(build03_tests PRIVATE dd_engine)
add_test(NAME build03_entity_hierarchy COMMAND build03_tests)

# Build 4: dispatch, extensions, inheritance and failure-handling tests.
add_executable(build04_demo src/build04_demo.cpp)
target_link_libraries(build04_demo PRIVATE dd_engine)
add_executable(build04_tests tests/build04.cpp)
target_link_libraries(build04_tests PRIVATE dd_engine)
add_test(NAME build04_polymorphic_abilities COMMAND build04_tests)

# Build 5: ordered bestiary, roster, species and template/adapter tests.
add_executable(build05_demo src/build05_demo.cpp)
target_link_libraries(build05_demo PRIVATE dd_engine)
add_executable(build05_tests tests/build05.cpp)
target_link_libraries(build05_tests PRIVATE dd_engine)
add_test(NAME build05_data_layer COMMAND build05_tests)

# Optional release benchmark; not part of the graded tests.
add_executable(build05_benchmark tests/build05_benchmark.cpp)
target_link_libraries(build05_benchmark PRIVATE dd_engine)
