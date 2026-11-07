#include "spellbook.h"
#include <algorithm>

namespace {
using Effect = void (*)(Caster&, Target&, CastResult&);
struct Spell {
    const char* name;
    int cost;
    Effect effect;
};

void deal_damage(Target& target, int raw, CastResult& result) {
    const int amount = std::max(1, raw - target.ward);
    target.hp = std::max(0, target.hp - amount);
    result.damage_dealt = amount;
}
void fireball(Caster&, Target& target, CastResult& result) {
    deal_damage(target, 9, result);
}
void mend(Caster& caster, Target&, CastResult& result) {
    result.healing_done = std::max(0, std::min(7, caster.max_hp - caster.hp));
    caster.hp += result.healing_done;
}
void summon(Caster& caster, Target& target, CastResult& result) {
    if (caster.summon_budget > 0) {
        --caster.summon_budget;
        ++caster.minions;
        result.minions_added = 1;
    } else {
        deal_damage(target, 3, result);
    }
}
void hex(Caster&, Target& target, CastResult& result) {
    const int new_strength = std::max(1, target.strength - 2);
    result.strength_drained = target.strength - new_strength;
    target.strength = new_strength;
}

const Spell spells[] = {
    {"Fireball", 6, &fireball},
    {"Mend", 4, &mend},
    {"Summon", 8, &summon},
    {"Hex", 3, &hex},
};
constexpr int count = static_cast<int>(sizeof(spells) / sizeof(spells[0]));
const Spell* lookup(int index) { return index >= 0 && index < count ? &spells[index] : nullptr; }
} // namespace

int spell_count() { return count; }
const char* spell_name(int index) {
    const Spell* spell = lookup(index);
    return spell ? spell->name : nullptr;
}
int spell_mana_cost(int index) {
    const Spell* spell = lookup(index);
    return spell ? spell->cost : 0;
}
CastResult cast_spell(int index, Caster& caster, Target& target) {
    const Spell* spell = lookup(index);
    if (!spell || caster.mana < spell->cost) return {false, 0, 0, 0, 0, 0};
    caster.mana -= spell->cost;
    CastResult result{true, spell->cost, 0, 0, 0, 0};
    spell->effect(caster, target, result);
    return result;
}
