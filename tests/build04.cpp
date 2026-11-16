#include "ability.h"
#include <cassert>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

int main() {
    static_assert(std::is_abstract_v<Ability>);
    static_assert(std::is_base_of_v<Ability, AttackAbility>);
    auto hero = make_hero("Ash", 100, 10, 1);
    auto monster = make_monster("Kobold", "kobold", 25, 4, 1, 6, ABIL_BITE);
    Wave wave;
    Monster& caster = *monster;
    wave.push_back(std::move(monster));
    AbilityContext ctx{caster, *hero, wave, 0};

    // All eight legacy behaviors use virtual dispatch and names stay stable.
    const char* names[] = {"Flail", "Cleave", "Bite", "Mend", "Hex", "Summon", "Guard", "Rally"};
    for (int i = 0; i < 8; ++i) {
        auto a = create_legacy_ability(i);
        assert(a->describe() == names[i]);
        assert(ability_name(i) == std::string(names[i]));
    }
    auto first = create_ability("Flail");
    auto cleave = create_ability("Cleave");
    auto bite = create_ability("Bite");
    auto mend = create_ability("Mend");
    auto hex = create_ability("Hex");
    auto summon = create_ability("Summon");
    auto guard = create_ability("Guard");
    auto rally = create_ability("Rally");
    dd_seed(123);
    assert(apply_ability(*first, ctx) > 0);
    assert(apply_ability(*cleave, ctx) > 0);
    assert(apply_ability(*bite, ctx) > 0);
    caster.hp = 10;
    assert(apply_ability(*mend, ctx) > 0 && caster.hp > 10);
    assert(apply_ability(*hex, ctx) > 0 && hero->strength < 10);
    caster.summon_budget = 1;
    auto previous = wave.size();
    assert(apply_ability(*summon, ctx) == 1 && wave.size() == previous + 1);
    int armor = caster.armor;
    assert(apply_ability(*guard, ctx) == 2 && caster.armor == armor + 2);
    assert(apply_ability(*rally, ctx) == 1 && wave.back()->strength == 3);

    // New variant loaded from its own translation unit, without dispatch edits.
    auto sunder = create_ability("Sunder");
    assert(sunder->describe() == "Sunder");
    armor = caster.armor;
    assert(apply_ability(*sunder, ctx) > 0 && caster.armor == armor + 1);

    std::vector<const Ability*> sequence {first.get(), guard.get()};
    assert(resolve_abilities(sequence, ctx) > 0);
    bool invalid = false;
    try { create_ability("Unknown"); } catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
    invalid = false;
    try { register_ability("Sunder", nullptr); } catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
    invalid = false;
    try { sequence.push_back(nullptr); resolve_abilities(sequence, ctx); }
    catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
    // Unanticipated execution failures are translated into contextual errors.
    struct Broken final : Ability {
        std::string describe() const override { return "Broken"; }
        int apply(AbilityContext&) const override { throw std::runtime_error("out of charges"); }
    } broken;
    bool wrapped = false;
    try { apply_ability(broken, ctx); }
    catch (const AbilityError& e) { wrapped = std::string(e.what()).find("Broken") != std::string::npos; }
    assert(wrapped);
}
