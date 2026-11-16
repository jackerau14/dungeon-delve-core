#include "ability.h"
#include <cstdio>
#include <memory>

// Open/closed demonstration: this sixth ability lives in one new file.
// Existing combat or registry sources do not need modification.
namespace {
class Sunder final : public AttackAbility {
public:
    std::string describe() const override { return "Sunder"; }
    int apply(AbilityContext& c) const override {
        const int damage = strike(c);
        c.caster.armor += 1; // difference from Cleave: improves the caster's defense
        if (c.verbose) std::printf("  %s sunders ................. %d damage, +1 armor\n", c.caster.name, damage);
        return damage;
    }
};
AbilityPtr make_sunder() { return std::make_unique<Sunder>(); }
const bool registered = [] { register_ability("Sunder", &make_sunder); return true; }();
}
