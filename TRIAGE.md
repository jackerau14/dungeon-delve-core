# TRIAGE.md — Build 1: Baseline and Triage

## 1. The three defects

### Defect 1 — Armor was subtracted twice (`encounter_resolves`)
**How I found it:** I ran the course-debug regression test and compared the damage reported by an attack with the HP actually removed. The armored monster lost too little HP.

**What was actually wrong:** `resolve_attack_on_monster()` (and the matching hero attack function) called `apply_armor()` before calling `apply_damage_to_monster()` or `apply_damage_to_hero()`. Those damage functions already apply armor. Armor was therefore subtracted twice, and the returned number did not match the actual HP loss.

**What I changed:** In `src/dungeon_delve.cpp` lines 235–245, each attack now passes raw damage directly to the damage function and returns the amount it reports. Armor is handled in one place.

### Defect 2 — Loot index went past the array (`loot_table_intact`)
**How I found it:** The debug test crashed with an AddressSanitizer global-buffer-overflow in `roll_loot()`.

**What was actually wrong:** `dd_rand_range()` includes both endpoints, but `roll_loot()` selected an index between 0 and `LOOT_COUNT` inclusive. The last possible index is invalid because an array of 12 entries has indices 0 through 11.

**What I changed:** At `src/dungeon_delve.cpp` line 184, the upper bound is now `LOOT_COUNT - 1`. Every roll now selects a valid array element.

### Defect 3 — Summoning invalidated the wave iterator (`roster_survives_reallocation`)
**How I found it:** AddressSanitizer reported heap-use-after-free from `run_monster_round()`. Its trace showed `wave.push_back()` in the summon ability triggered vector reallocation.

**What was actually wrong:** The loop held a `std::vector` iterator while the monster's summon ability added another monster to that same vector. Adding an element can move the vector's storage and invalidate its iterators. The next iteration then used an invalid iterator.

**What I changed:** At `src/dungeon_delve.cpp` lines 432–441, the loop uses an integer index and obtains a borrowed `Monster*` for the current monster. The monster object is separately heap-allocated, so moving its `unique_ptr` inside the vector does not move the monster itself. The loop can safely handle a newly added monster.

## 2. Ownership table

All heap allocations below were created using raw `new` in the original source. The original code did not consistently release them.

| Original heap allocation | Owner before | Converted to | Why |
|:--|:--|:--|:--|
| `new Hero` in `make_hero()` (original line 119) | Caller of `make_hero()`, including the campaign or test | `HeroPtr` = `std::unique_ptr<Hero>` | Only one owner is needed; automatic cleanup when it goes out of scope. |
| `new Monster` in `make_monster()` (original line 137) | Caller; usually `Bestiary` or `Wave`, including summoned monsters | `MonsterPtr` = `std::unique_ptr<Monster>` | Each monster has one owning handle, and the vectors clean up their monsters. |
| `new Merchant` in `make_merchant()` (original line 155) | Campaign's merchant owner | `MerchantPtr` = `std::unique_ptr<Merchant>` | One owner and automatic cleanup. |
| `new Hero` in `load_hero()` (original line 615) | Caller receiving the loaded hero; the error path formerly used `delete` | `HeroPtr` = `std::unique_ptr<Hero>` | Cleans up on both success and early return without manual `delete`. |

`spawn_from_bestiary()` creates monsters through `make_monster()`, not a separate allocation mechanism. `build_bestiary()` owns its prototypes, while each `Wave` owns its independent spawned copies. `ABIL_SUMMON` adds an independently owned monster to the wave. The existing `Wave` and `Bestiary` vectors are kept; no new container was introduced.

**Non-owning views:** `find_in_bestiary()` returns `Monster*` borrowed from `Bestiary`; its caller's `proto` is borrowed too (line 396). The `raw(...)` helper explicitly returns a non-owning `Monster*`. The `m` local in `run_monster_round()` (line 433) borrows from `wave`. `Hero&` and `Merchant&` aliases in `run_campaign()` borrow from their `unique_ptr` owners. Function arguments passed as entity references (`Hero&`, `Monster&`, `Merchant&`) also do not take ownership. `acted_names` is an optional borrowed pointer to a vector supplied by the caller. Loot-name `const char*` values point into the static loot table and must not be deleted. Borrowed pointers/references must not outlive their owner.

## 3. The three worst things about this codebase

1. **All abilities are handled in a large integer-tag switch (`src/dungeon_delve.cpp`, lines 254–350).** This makes new abilities difficult to add and test. In a later build I would separate ability behaviors as allowed by that week's architecture rules. I did not add any new classes or inheritance in Build 1.
2. **Save loading does not check that all numeric fields were read (`src/dungeon_delve.cpp`, lines 618–625).** A damaged or incomplete save can leave fields uninitialized. In a later build I would check every `fscanf()` result, reject bad files safely, and add regression tests.
3. **Entity names use fixed-size character arrays (`include/dungeon_delve.h`, lines 34–59).** A long name must be truncated and copied carefully, making mistakes easier. In a later build I would consider safer string handling after the course permits architecture changes.

## Verification

Commands run successfully with CMake's `course-debug` preset:

```text
cmake --preset course-debug
cmake --build --preset course-debug
ctest --preset course-debug --output-on-failure

encounter_resolves ............... Passed
loot_table_intact ................ Passed
roster_survives_reallocation ..... Passed
100% tests passed, 0 tests failed out of 3
```

The sanitized `dungeon_delve` executable also cleared all six stages with `ASAN_OPTIONS=detect_leaks=1` and produced no sanitizer error or leak report.
