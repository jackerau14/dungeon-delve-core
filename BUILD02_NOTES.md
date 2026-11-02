# Build 2 — Value Types and the Domain Vocabulary

## What changed

- `include/domain_types.h` defines `Coin`, `Damage`, `Dice`, and `Vec2` as distinct types with private data, getters, and validation where appropriate. A negative `Coin` or `Damage` is rejected; `Dice` requires a positive count and side count.
- `Coin + Coin`, `Coin - Coin`, `Damage + Damage`, and `Vec2 + Vec2` describe meaningful operations. No arithmetic between unrelated value types is permitted. Spending more than a purse holds raises an error rather than allowing a negative balance.
- `EncounterMath` has only static methods and a deleted constructor: it does not own state, so instances would have no purpose.
- `Purse` is a true instance-based class: each purse owns its own balance.
- Three overloads of `resolve` handle predetermined damage, rolled damage, and rolled damage written to an out parameter.

## Parameter choices and their costs

- `Hero&` is a non-const reference because resolving an attack changes HP. It avoids copying the whole Hero, and the changed HP remains visible to the caller.
- `const Monster&` is a read-only borrowed reference because the enemy is only inspected; it avoids a full Monster copy.
- `const Dice&` is read-only and avoids copying even though Dice is small; for a tiny two-integer value, passing Dice by value would also be reasonable and could be cheaper. This choice is mainly consistent with the interface.
- `Damage` by value is cheap (one integer) and keeps a separate, validated typed argument; it does not need the caller's object to change.
- Returning `Damage` by value avoids an extra out-parameter and is easy to use. Compilers commonly elide copies or move a small value efficiently.
- The `Damage& dealt` out-parameter writes directly to an existing caller object, avoiding a separately named return variable, but adds a mutable reference and makes the call harder to read. It is provided because the assignment asks for both paths.

## How to demonstrate

Build and run `build02_demo`. Its example uses a fixed, documented 11-damage roll to show the requested layout deterministically. `build02_tests` checks actual dice rolling and all three resolve overloads. Run `ctest` to check the original Build 1 regression tests too.

## Scope

The existing legacy campaign structures continue to use their original integer fields to preserve Build 1 behavior. New code can use the typed Build 2 API, which calls the existing armor/damage rule instead of duplicating it. A complete engine-wide conversion of every int field is not included here.

## Windows / Visual Studio 2022

Open the extracted project folder in Visual Studio 2022 (File > Open > Folder). Choose `course-debug-msvc` when configuring CMake, then Build > Build All and Test > Run All Tests. The Build 2 demo is a separate CMake startup target. Do not tag/push until the tests pass on your computer.
