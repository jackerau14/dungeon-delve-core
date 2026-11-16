#include "ability.h"
#include <array>
#include <cstdio>
#include <map>
#include <utility>

namespace {
std::map<std::string, AbilityFactory>& registry() {
    static std::map<std::string, AbilityFactory> types;
    return types;
}

struct Flail final : AttackAbility {
    std::string describe() const override { return "Flail"; }
    int apply(AbilityContext& c) const override {
        const int n = strike(c);
        if (c.verbose) std::printf("  %s flails .................. %d damage\n", c.caster.name, n);
        return n;
    }
};
struct Cleave final : AttackAbility {
    std::string describe() const override { return "Cleave"; }
    int apply(AbilityContext& c) const override {
        int amount = strike(c);
        amount += amount / 2;
        apply_damage_to_hero(c.hero, amount / 2);
        if (c.verbose) std::printf("  %s cleaves ................. %d damage\n", c.caster.name, amount);
        return amount;
    }
};
struct Bite final : AttackAbility {
    std::string describe() const override { return "Bite"; }
    int apply(AbilityContext& c) const override {
        int amount = strike(c);
        if (dd_rand_range(1, 100) > 70) {
            apply_damage_to_hero(c.hero, 2);
            amount += 2;
            if (c.verbose) std::printf("  %s bites deep ............. %d damage\n", c.caster.name, amount);
        } else if (c.verbose) {
            std::printf("  %s bites ................... %d damage\n", c.caster.name, amount);
        }
        return amount;
    }
};
struct Mend final : Ability {
    std::string describe() const override { return "Mend"; }
    int apply(AbilityContext& c) const override {
        const int amount = roll_dice(2, 4);
        c.caster.hp += amount;
        if (c.caster.hp > c.caster.max_hp) c.caster.hp = c.caster.max_hp;
        if (c.verbose) std::printf("  %s mends ................... +%d HP\n", c.caster.name, amount);
        return amount;
    }
};
struct Hex final : Ability {
    std::string describe() const override { return "Hex"; }
    int apply(AbilityContext& c) const override {
        const int amount = roll_dice(1, 4);
        c.hero.strength -= amount;
        if (c.hero.strength < 1) c.hero.strength = 1;
        if (c.verbose) std::printf("  %s hexes ................... -%d STR\n", c.caster.name, amount);
        return amount;
    }
};
struct Summon final : Ability {
    std::string describe() const override { return "Summon"; }
    int apply(AbilityContext& c) const override {
        if (c.caster.summon_budget > 0) {
            --c.caster.summon_budget;
            c.wave.push_back(make_monster("Whelp", "whelp", 6, 2, 0, 3, ABIL_NONE));
            if (c.verbose) std::printf("  %s summons a Whelp!\n", c.caster.name);
            return 1;
        }
        const int amount = resolve_attack_on_hero(c.caster, c.hero);
        if (c.verbose) std::printf("  %s is out of whelps ....... %d damage\n", c.caster.name, amount);
        return amount;
    }
};
struct Guard final : Ability {
    std::string describe() const override { return "Guard"; }
    int apply(AbilityContext& c) const override {
        c.caster.armor += 2;
        if (c.verbose) std::printf("  %s guards .................. +2 armor\n", c.caster.name);
        return 2;
    }
};
struct Rally final : Ability {
    std::string describe() const override { return "Rally"; }
    int apply(AbilityContext& c) const override {
        for (const MonsterPtr& member : c.wave) {
            if (!member->is_dead && member.get() != &c.caster) member->strength += 1;
        }
        if (c.verbose) std::printf("  %s rallies the pack ........ +1 STR each\n", c.caster.name);
        return 1;
    }
};

template<class T> AbilityPtr construct() { return std::make_unique<T>(); }

void register_builtin_abilities() {
    static const bool registered = [] {
        register_ability("Flail", &construct<Flail>);
        register_ability("Cleave", &construct<Cleave>);
        register_ability("Bite", &construct<Bite>);
        register_ability("Mend", &construct<Mend>);
        register_ability("Hex", &construct<Hex>);
        register_ability("Summon", &construct<Summon>);
        register_ability("Guard", &construct<Guard>);
        register_ability("Rally", &construct<Rally>);
        return true;
    }();
    (void)registered;
}

constexpr std::array<const char*, 8> legacy_names {
    "Flail", "Cleave", "Bite", "Mend", "Hex", "Summon", "Guard", "Rally"
};
}

int AttackAbility::strike(AbilityContext& c) const {
    return resolve_attack_on_hero(c.caster, c.hero);
}

void register_ability(std::string name, AbilityFactory factory) {
    if (name.empty() || factory == nullptr) throw std::invalid_argument("Ability requires a name and factory");
    if (!registry().emplace(std::move(name), factory).second)
        throw std::invalid_argument("Duplicate ability registration");
}

AbilityPtr create_ability(std::string_view name) {
    register_builtin_abilities();
    auto it = registry().find(std::string(name));
    if (it == registry().end()) throw std::invalid_argument("Unknown ability: " + std::string(name));
    auto result = it->second();
    if (!result) throw AbilityError("Ability factory returned null");
    return result;
}

std::vector<std::string> available_abilities() {
    register_builtin_abilities();
    std::vector<std::string> names;
    for (const auto& entry : registry()) names.push_back(entry.first);
    return names;
}

const char* legacy_ability_name(int tag) {
    if (tag < 0 || static_cast<std::size_t>(tag) >= legacy_names.size()) return "???";
    return legacy_names[static_cast<std::size_t>(tag)];
}

AbilityPtr create_legacy_ability(int tag) {
    // Out-of-range legacy tags used to fall back to a basic attack.
    const char* name = legacy_ability_name(tag);
    return create_ability(name[0] == '?' ? "Flail" : name);
}

int apply_ability(const Ability& ability, AbilityContext& context) {
    try {
        return ability.apply(context);
    } catch (const std::invalid_argument&) {
        throw; // validation failures retain their specific type
    } catch (const std::exception& e) {
        throw AbilityError(std::string("Could not apply ") + ability.describe() + ": " + e.what());
    }
}

int resolve_abilities(const std::vector<const Ability*>& abilities, AbilityContext& context) {
    int total = 0;
    for (const Ability* ability : abilities) {
        if (!ability) throw std::invalid_argument("Null ability in encounter");
        total += apply_ability(*ability, context);
    }
    return total;
}
