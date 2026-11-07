# Concept Lab 3 — Same Problem, Twice

**Name:** Jack Rau    **Date:** 11/06/2026

---

## Step 1 — Get both versions passing

| | Output |
|:--|:--|
| `same_problem_twice_inherit_tests` final line | `ALL TESTS PASSED (90 checks)` |
| `same_problem_twice_compose_tests` final line | `ALL TESTS PASSED (90 checks)` |
| `diff a.txt b.txt` of the two drivers | `empty` (no differences) |

**My answer:** The two drivers gave the same output, so I did not need to change their behavior to match. Both versions passed all 90 checks using the supplied tests unchanged.

---

## Step 2 — The measurement table

### Line count

| | Version A — inheritance | Version B — composition |
|:--|:--:|:--:|
| Files in the folder | 1 | 1 |
| Raw lines (`wc -l`, total) | 94 | 65 |
| Code lines only (neither blank nor `//`) | 89 | 61 |

These counts are for the final versions, including Hex. The code-line counts exclude blank lines and lines starting with `//`.

### Files touched to add Hex

| | Version A — inheritance | Version B — composition |
|:--|:--:|:--:|
| Existing files I **edited** | 1 | 1 |
| New files I **added** | 0 | 0 |
| Separate places inside those files I had to change | 2 | 2 |

In inheritance, I added a `Hex` subclass and added its instance to the spell registry. In composition, I added a `hex` effect function and added its entry to the spell table.

The `diff -rq` comparison of the before-Hex snapshot and the finished lab printed:

```text
Files week03_before/impl_compose/spells.cpp and labs/week03_same_problem_twice/impl_compose/spells.cpp differ
Files week03_before/impl_inherit/spells.cpp and labs/week03_same_problem_twice/impl_inherit/spells.cpp differ
```

### Depth of the structure

| | Version A — inheritance | Version B — composition |
|:--|:--:|:--:|
| Number reported | 2 | 1 |
| What I counted to get it | 2 classes in the `Spell` → derived spell inheritance chain | 1 function-pointer indirection from the spell record to its effect; inheritance depth is 0 |

### Where the shared mana rule ended up

| | Where the mana rule lives |
|:--|:--|
| Version A — inheritance | Once in `Spell::cast()` in `impl_inherit/spells.cpp` |
| Version B — composition | Once in `cast_spell()` in `impl_compose/spells.cpp` |

Neither version duplicates the shared mana check across individual spells.

---

## Step 3 — What the numbers do not show

The inheritance version made it easier to keep the mana check inside a shared base class, because every spell uses the same `cast()` method. The composition version was easier to extend with another effect function, but the registry still needs to be updated when a new spell is added.

---

## Step 4 — The recommendation

**Which one would you ship as the ability system for Build 4?**

**I would ship:** [ ] Version A, inheritance   [x] Version B, composition

**Why — in terms of the numbers above:**

I would use composition for Build 4 because it uses 65 raw lines compared with 94 for inheritance, and 61 code lines compared with 89. Both designs needed one existing file and two separate edits to add Hex, so composition did not make that change more expensive in this lab. It also avoids the inheritance chain while keeping the same spell behavior and passing the same 90 checks.

**Name something the design you rejected does better:**

Inheritance gives the shared mana rule a natural place in `Spell::cast()`. This helps make sure that every subclass checks mana before applying its own effect, rather than relying on a separate dispatch function to enforce that rule.

**One change that would flip your recommendation:**

I would ship the inheritance design instead if the requirements changed so that spells needed several shared operations with different behavior in each subclass, such as their own targeting and cooldown rules. In that case, a common base class with virtual hooks could keep those rules together more clearly.

---

## Verification notes

Compiled both versions using CMake with C++20. Both supplied test binaries reported `ALL TESTS PASSED (90 checks)`. The driver output comparison produced no differences. Line counts were measured on the final source files, and file-change counts were measured by comparing the completed source against a before-Hex snapshot.
