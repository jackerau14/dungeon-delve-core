#include "dungeon_delve.h"
#include <chrono>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

// Optional reproducible lookup microbenchmark, NOT the student's Discussion 5 measurements.
int main() {
    auto indexed = build_bestiary();
    std::vector<const Monster*> linear;
    for (const auto& entry : indexed) linear.push_back(entry.second.get());
    const char* keys[] = {"bat", "kobold", "priest", "ogre", "warg", "goblin", "brood", "packleader", "adept", "unknown"};
    constexpr int n = 1000000;
    std::size_t found = 0;
    using clock = std::chrono::steady_clock;
    const auto start_linear = clock::now();
    for (int i = 0; i < n; ++i) {
        const char* key = keys[i % 10];
        for (const auto* monster : linear) {
            if (std::strcmp(monster->species, key) == 0) { ++found; break; }
        }
    }
    const auto end_linear = clock::now();
    for (int i = 0; i < n; ++i) {
        const char* key = keys[i % 10];
        if (indexed.find(key) != indexed.end()) ++found;
    }
    const auto end_map = clock::now();
    const auto linear_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_linear - start_linear).count();
    const auto map_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_map - end_linear).count();
    std::printf("Release lookup benchmark (9 species, 1,000,000 queries each):\n");
    std::printf("vector linear search: %.2f ns/query\n", static_cast<double>(linear_ns) / n);
    std::printf("std::map lookup: %.2f ns/query\n", static_cast<double>(map_ns) / n);
    std::printf("matching lookups (both rounds): %zu\n", found);
}
