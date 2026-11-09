# Build 3 — The Entity Hierarchy

## Class diagram

```text
                       Entity
                (virtual destructor,
                virtual describe())
                 /      |       \
                /       |        \
        Combatant       |       Lootable
      (virtual Entity)  |     (virtual Entity)
          |  \          |         /  |
          |   \         |        /   |
          |    +------- Monster --+   |
          |             /  |          |
         Hero          /   |       Merchant
          |           /    |          |
      Combatant    Combatant      Lootable

   Concrete classes all also declare `virtual Entity` directly:
     Hero     : virtual Entity, Combatant
     Monster  : virtual Entity, Combatant, Lootable
     Merchant : virtual Entity, Lootable
```

The key *diamond* is `Monster -> Combatant -> Entity` and
`Monster -> Lootable -> Entity` (with a direct virtual `Monster -> Entity`
edge as well). Both mixins use `virtual Entity`, and the concrete types use
`virtual Entity` too, so a `Monster` contains **one** `Entity` subobject,
not two or three. `tests/build03.cpp` checks that the `Entity*` reached through
both mixins and directly has the same address. All concrete classes override
`describe()`; the compiler chooses the override through `Entity&` at runtime.

## Why I chose this design

My most debatable choice is having `Monster` inherit both `Combatant` and
`Lootable`. A monster needs both capabilities, so two small interfaces describe
it better than one large parent class with unrelated methods. The cost is extra
complexity: virtual inheritance takes more care, and it can add indirect calls
and some object-layout overhead. Keeping the old public fields also means this
build does not yet fully protect game state, but changing them now could break
Build 1 tests and the save-file format. I would ship this hierarchy for Build 3
because it gives Build 4 a shared interface without losing existing behavior.

## What works through each inheritance edge

- `Hero -> Entity`: `describe_entity(const Entity&)` calls `Hero::describe()`.
- `Monster -> Entity`: `describe_entity(const Entity&)` calls `Monster::describe()`.
- `Merchant -> Entity`: `describe_entity(const Entity&)` calls `Merchant::describe()`.
- `Combatant -> Entity`: `describe_entity(const Entity&)` works with a `Combatant&` that refers to a `Hero` or `Monster`.
- `Lootable -> Entity`: `describe_entity(const Entity&)` works with a `Lootable&` that refers to a `Monster` or `Merchant`.
- `Hero -> Combatant`: `combatant_hp(const Combatant&)` reads the hero's HP.
- `Monster -> Combatant`: `combatant_hp(const Combatant&)` reads the monster's HP.
- `Monster -> Lootable`: `lootable_gold(const Lootable&)` reads the monster's drop gold.
- `Merchant -> Lootable`: `lootable_gold(const Lootable&)` reads the merchant's gold.

`describe_roster(const std::vector<const Entity*>&)` demonstrates dispatch for
all three concrete types. There is no `dynamic_cast`, no type tag, and no
polymorphic ability system added yet.

## Verification

- Build all CMake targets, then run `ctest --test-dir build/course-debug-msvc -C Debug --output-on-failure` in a Visual Studio 2022 developer terminal (or use the matching preset).
- Run `build03_demo` to see Ash, Kobold, and Old Yarrow described through an `Entity` interface.
- The earlier three regression tests and the Build 2 value-type test are preserved.
