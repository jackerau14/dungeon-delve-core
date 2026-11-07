# Adding Concept Lab 3 to Dungeon Delve in Visual Studio 2022

1. Extract this ZIP.
2. Copy the entire `labs/week03_same_problem_twice` folder into the `labs` folder of your existing `dungeon-delve-core` Git checkout.
3. Open your **existing Git checkout** in Visual Studio 2022 (not a standalone copy of the ZIP).
4. Check your root `CMakeLists.txt`: it must add the lab folder as a subdirectory. If the root already discovers labs automatically, do not change it. Otherwise add `add_subdirectory(labs/week03_same_problem_twice)` after defining the `dd_sanitize_flags` target, and ensure `CMAKE_RUNTIME_OUTPUT_DIRECTORY` is `${CMAKE_BINARY_DIR}` if you want executables in the build root.
5. Select the course debug CMake preset and run Build > Build All.
6. Run both `same_problem_twice_inherit_tests` and `same_problem_twice_compose_tests` from the build folder. Both should report `ALL TESTS PASSED (90 checks)`.
7. Run both demo executables and compare their output. They should be identical.
8. Review `labs/week03_same_problem_twice/ANALYSIS.md` and submit that file as directed by your instructor. The measured results come from a Linux GCC/CMake build; verify on Visual Studio as well.

Do not replace your root CMakeLists.txt with a template from the lab handout: that could overwrite the Build 2 configuration in your current repository. Do not modify the supplied contract, tests, driver, or support files.
