# Build 4 — The Ability System

## Old switch resolver vs. new polymorphic resolver

I started with the Build 3 version of `src/dungeon_delve.cpp`. Its old `resolve_ability()` was **101 physical lines** (counting the function declaration through its closing brace, excluding surrounding comments). It used one long `switch` with cases for Flail, Cleave, Bite, Mend, Hex, Summon, Guard and Rally. The refactored `resolve_ability()` is **6 physical lines**. It creates an ability object from the legacy tag, creates an `AbilityContext`, and sends the work to the ability's virtual `apply()` method. The central function is shorter, but the code has not simply vanished; it now lives in derived classes.

| File | Before (physical lines) | After (physical lines) | Difference |
|---|---:|---:|---:|
| `src/dungeon_delve.cpp` | 811 | 706 | -105 |
| `include/ability.h` | 0 (did not exist) | 54 | +54 |
| `src/ability_system.cpp` | 0 (did not exist) | 174 | +174 |
| `src/abilities/sunder.cpp` | 0 (did not exist) | 20 | +20 |

*Method used:* count newline-separated physical lines in the Build 3 ZIP and in the completed source, including blank lines and comments. These numbers focus on the files that contain the ability resolver and its replacement; the new demo, tests, and build settings are additional work. The original monster-round dispatch now calls through `Ability::apply()`, not a tag-based `switch`, `if` on types, or `dynamic_cast`.

## Five abilities and two levels of virtual override

`Ability` is abstract and declares pure virtual `describe()` and `apply()` with a virtual destructor. There are eight migrated concrete abilities, so more than the required five. They do different work: damage, bonus damage, a bite bonus, healing, strength reduction, summoning, armor buffs, and group buffs. For two levels of overriding, `AttackAbility` overrides `describe()` from `Ability`, and `Cleave` (also `Bite` and `Flail`) overrides it again from `AttackAbility`.

## What got worse

This design makes the code more spread out. Instead of opening one switch statement to see all of the actions, I have to look at several classes and the ability registration code. It also creates and manages polymorphic ability objects and introduces a registry that can reject invalid names or duplicate registrations. That's more complexity and can mean more allocations than the old integer switch. The tradeoff is worth it for adding abilities without changing the resolver, but the old version was simpler to read for a small fixed set of attacks.

## Open/closed test: adding a sixth ability

I added **Sunder** in `src/abilities/sunder.cpp`. It deals attack damage and gives the caster +1 armor. The CMake build already discovers sources under `src/abilities/` automatically, and the extension registers itself by name. **Existing implementation files edited after adding Sunder: 0. New files added: 1.** The new test calls `create_ability("Sunder")` and checks the armor effect. This proves that a new ability can be added without editing the central combat dispatcher, though new tests or game content would naturally still need changes to use it in an actual encounter.

## Error handling

The ability factory throws `std::invalid_argument` for unknown ability names, invalid registration, and missing abilities in a sequence. `apply_ability` lets validation errors pass through while wrapping other execution failures in `AbilityError` with the ability name. The Build 4 test checks both an invalid ability and an ability that throws while running.

## Compatibility and verification

I kept the old integer tags at the **legacy data/API boundary** so earlier save data and the previous tests still work. The resolution loop itself never checks an ability type. Using GCC/CMake on Linux, I compiled the updated repository and ran CTest: **6/6 test suites passed**, including the Build 1, Build 2, and Build 3 tests. The Build 4 demo runs and recognizes Sunder. It has not yet been tested on my Windows installation of Visual Studio 2022.
