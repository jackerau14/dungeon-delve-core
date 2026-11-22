# Build 5 — The Data Layer

## Why these containers fit the actual access patterns

- **`std::vector` for the party roster (`PartyRoster` in `include/data_layer.h`).** The program walks every party member to display the roster and sorts the *same* members in multiple ways. Contiguous storage gives efficient iteration and allows `std::sort` on a snapshot. Keeping the underlying roster unchanged means HP sorting, name sorting, and strength sorting do not interfere with one another. Building each sorted view takes O(n log n) time and copies n lightweight entries.
- **`std::map<std::string, MonsterPtr>` for `Bestiary` (`include/dungeon_delve.h`; `src/dungeon_delve.cpp`).** Spawn and lookup operations ask for a monster by its species key, not by its position. `find_in_bestiary` and `count_species_in_bestiary` use the ordered map's O(log n) search instead of scanning the whole bestiary, while unique keys prevent duplicate species definitions. The existing monster prototypes remain owned by `unique_ptr` and spawning still makes a separate monster.
- **`std::set<std::string>` for species encounters (`SpeciesSeen`).** The game only needs to know whether each species has been seen and how many distinct species exist. Insert both records a species and absorbs duplicates automatically. In the test, 11 encounters produce 7 unique species and 4 duplicates.
- **`Registry<T>` uses a map** because items are retrieved by stable string keys; it works for `int` and `std::string`. **`Pool<T>` uses a vector** because entries are stored in order and visited or removed by position; it works for `int` and `std::string`.
- **`LegacyLootBridge`** keeps the original `LOOT_TABLE` C array untouched. Importing into an owning `Pool<std::string>` copies each text value once because the array's lifetime is not owned by the pool. Exporting a `std::array<const char*, N>` borrows pointers from the pool's existing strings without copying their text again. The pool must stay alive and unchanged while those borrowed pointers are used. `tests/build05.cpp` checks exact name equality and pointer identity on export.

## Three ways of supplying a comparator

`tests/build05.cpp` tests (1) `by_name` through a function pointer, (2) `ByHP` as a callable function object, and (3) a lambda sorting by strength. Every sorted view is created from a `std::vector` snapshot; the original `PartyRoster` order is preserved.

## Measured timings and Discussion 5

**Discussion 5 timing results (measured on my computer):** I tested insertion of 100,000 unique integer keys, 100,000 successful lookups, and iteration over all elements. The experiment used Visual Studio 2022, MSVC 19.44 (x64), a Release build, and an HP Pavilion Gaming Desktop with an Intel Core i5-9400F at 2.90 GHz and 24 GB RAM on Windows. Vector and list lookups used `std::find`; set and map lookups used their member `find`. Measurements are in milliseconds from this run.

| Container | Insert (ms) | Find (ms) | Iterate (ms) |
|---|---:|---:|---:|
| `std::vector` | 0.507 | 324.213 | 0.034 |
| `std::list` | 4.226 | 22,906.878 | 0.303 |
| `std::set` | 33.950 | 24.953 | 3.519 |
| `std::map` | 27.538 | 28.613 | 1.873 |

**Did the results change my choices?** No. The vector was fastest at insertion and iteration, supporting its use for a small roster that is displayed and sorted. The map and set had much faster key lookup than a linear vector scan in this 100,000-element experiment, supporting the bestiary and unique-species use cases. List search was especially slow despite sharing O(n) search complexity with vector; pointer-based node traversal and cache locality are plausible explanations. These timings do not prove a map is always faster: my separate nine-species benchmark below actually favors vector linear search. I would repeat the measurements under matching workloads before choosing based on speed alone.

**Separate, reproducible Build 5 benchmark (performed here):** A Release C++20 executable (`build05_benchmark`) compared 1,000,000 repeated lookups against nine bestiary species, including missing keys. On this Linux environment in one run, vector linear search averaged **17.25 ns/query**, while `std::map` averaged **20.53 ns/query**. These results are implementation- and machine-specific; a tiny fixed set can favor a vector because of lower overhead and cache locality. The timings did **not** change the map choice for the assignment: the index is useful for a scalable species-key lookup contract, but the result shows it is not automatically faster for nine entries. To reproduce locally, use a Release preset and run `build05_benchmark` (never benchmark a Debug build).

## What I would revisit if the roster were 1,000 times larger

I would revisit building a full copied and sorted roster for every view request. If roster sorting were frequent, I would cache sorted *indices* into the vector and update/invalidate the indices when the roster changes, avoiding repeated copies of large entries. This adds bookkeeping and invalidation complexity, so it is not worthwhile for a three-member roster. I would keep the canonical vector unless measurement showed its iteration or update pattern becoming a problem.

## Validation

`tests/build05.cpp` checks keyed lookup and missing keys; the HP, name, and strength sorts; deduplication; two instantiations of each template; raw-array import/export and unchanged source content. The demo prints the assignment's kobold, Ash/Bree/Cato, and 7-of-11 output. All earlier CTest tests remain included.
