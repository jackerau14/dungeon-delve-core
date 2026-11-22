# Build 05 — Visual Studio 2022 and GitHub

1. Copy the changed or new files from this ZIP into your **existing local Git clone**, not into a new GitHub repository. The project is a CMake folder project; use **File > Open > Folder**.
2. Select **course-debug-msvc** in the CMake configuration dropdown, then **Build > Build All**.
3. Open the Visual Studio terminal and run `ctest --test-dir build/course-debug-msvc -C Debug --output-on-failure` if your preset uses that output location. Alternatively use `ctest --preset course-debug-msvc` from the repo root. All seven CTest tests should pass.
4. Run `build05_demo` and confirm the displayed kobold, HP-sorted roster, and 7 species.
5. Open `CONTAINERS.md`, add **your actual Discussion 5 timings** (they were not supplied with the repository), and review the explanation.
6. Once the current main branch has the changes and tests pass, commit and push, tag `build-05`, and push the tag. If you uploaded files via GitHub first, pull them into your clean local clone before tagging.
7. Verify `build-05` in GitHub Tags and submit that exact tag name in Canvas.

Do not push or tag until Visual Studio builds and the tests pass. The provided ZIP has no `.git` history, so it is not a substitute for your existing Git clone.
