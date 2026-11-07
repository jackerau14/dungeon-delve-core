#include "spellbook.h"
#include <algorithm>

namespace {
class Spell {
public:
    virtual ~Spell() = default;
    virtual const char* name() const = 0;
    virtual int cost() const = 0;
    CastResult cast(Caster& caster, Target& target) const {
        if (caster.mana < cost()) return {false, 0, 0, 0, 0, 0};
        caster.mana -= cost();
        CastResult result{true, cost(), 0, 0, 0, 0};
        effect(caster, target, result);
        return result;
    }
protected:
    virtual void effect(Caster&, Target&, CastResult&) const = 0;
};

void deal_damage(Target& target, int raw, CastResult& result) {
    const int amount = std::max(1, raw - target.ward);
    target.hp = std::max(0, target.hp - amount);
    result.damage_dealt = amount;
}

class Fireball final : public Spell {
public:
    const char* name() const override { return "Fireball"; }
    int cost() const override { return 6; }
private:
    void effect(Caster&, Target& target, CastResult& result) const override {
        deal_damage(target, 9, result);
    }
};
class Mend final : public Spell {
public:
    const char* name() const override { return "Mend"; }
    int cost() const override { return 4; }
private:
    void effect(Caster& caster, Target&, CastResult& result) const override {
        result.healing_done = std::max(0, std::min(7, caster.max_hp - caster.hp));
        caster.hp += result.healing_done;
    }
};
class Summon final : public Spell {
public:
    const char* name() const override { return "Summon"; }
    int cost() const override { return 8; }
private:
    void effect(Caster& caster, Target& target, CastResult& result) const override {
        if (caster.summon_budget > 0) {
            --caster.summon_budget;
            ++caster.minions;
            result.minions_added = 1;
        } else {
            deal_damage(target, 3, result);
        }
    }
};
class Hex final : public Spell {
public:
    const char* name() const override { return "Hex"; }
    int cost() const override { return 3; }
private:
    void effect(Caster&, Target& target, CastResult& result) const override {
        const int new_strength = std::max(1, target.strength - 2);
        result.strength_drained = target.strength - new_strength;
        target.strength = new_strength;
    }
};

const Fireball fireball;
const Mend mend;
const Summon summon;
const Hex hex;
const Spell* const spells[] = {&fireball, &mend, &summon, &hex};
constexpr int count = static_cast<int>(sizeof(spells) / sizeof(spells[0]));
const Spell* lookup(int index) { return index >= 0 && index < count ? spells[index] : nullptr; }
} // namespace

int spell_count() { return count; }
const char* spell_name(int index) {
    const Spell* spell = lookup(index);
    return spell ? spell->name() : nullptr;
}
int spell_mana_cost(int index) {
    const Spell* spell = lookup(index);
    return spell ? spell->cost() : 0;
}
CastResult cast_spell(int index, Caster& caster, Target& target) {
    const Spell* spell = lookup(index);
    return spell ? spell->cast(caster, target) : CastResult{false, 0, 0, 0, 0, 0};
}
