#include "domain_types.h"
#include <limits>

namespace {
int checked_add(int a, int b) {
    if (b > std::numeric_limits<int>::max() - a)
        throw std::overflow_error("Domain value overflow");
    return a + b;
}
}

Coin Coin::operator+(const Coin& rhs) const { return Coin(checked_add(amount_, rhs.amount_)); }
Coin Coin::operator-(const Coin& rhs) const {
    if (rhs.amount_ > amount_) throw std::invalid_argument("Not enough coins");
    return Coin(amount_ - rhs.amount_);
}
Damage Damage::operator+(const Damage& rhs) const { return Damage(checked_add(points_, rhs.points_)); }

Damage EncounterMath::resolve(Hero& hero, const Monster& enemy, Damage incoming) {
    (void)enemy; // Reserved for enemy-specific behavior in a later build.
    // Reuse the existing engine rule; damage is reduced by armor exactly once.
    return Damage(apply_damage_to_hero(hero, incoming.points()));
}

Damage EncounterMath::resolve(Hero& hero, const Monster& enemy, const Dice& dice) {
    Damage rolled(roll_dice(dice.count(), dice.sides()));
    return resolve(hero, enemy, rolled);
}

void EncounterMath::resolve(Hero& hero, const Monster& enemy, const Dice& dice, Damage& dealt) {
    dealt = resolve(hero, enemy, dice);
}
