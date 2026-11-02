#include "domain_types.h"
#include <cassert>
#include <stdexcept>
#include <type_traits>

int main() {
    static_assert(!std::is_constructible_v<EncounterMath>);
    static_assert(!std::is_convertible_v<Coin, int>);
    assert((Coin(10) + Coin(14)).amount() == 24);
    assert((Coin(10) - Coin(3)).amount() == 7);
    assert((Damage(3) + Damage(4)).points() == 7);
    assert(Vec2(2, 3) + Vec2(4, 5) == Vec2(6, 8));
    bool threw = false;
    try { (void)Coin(-1); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { (void)Dice(0, 6); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    Purse purse(Coin(20));
    purse.deposit(Coin(4));
    assert(purse.total().amount() == 24);
    purse.spend(Coin(4));
    assert(purse.total().amount() == 20);
    threw = false;
    try { purse.spend(Coin(21)); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    assert(purse.total().amount() == 20);

    HeroPtr ash = make_hero("Ash", 100, 10, 0);
    MonsterPtr kobold = make_monster("Kobold", "kobold", 30, 5, 0, 24, ABIL_NONE);
    Damage fixed = EncounterMath::resolve(*ash, *kobold, Damage(11));
    assert(fixed.points() == 11 && ash->hp == 89);
    dd_seed(1234);
    Damage rolled = EncounterMath::resolve(*ash, *kobold, Dice(3, 6));
    assert(rolled.points() >= 3 && rolled.points() <= 18);
    Damage out;
    EncounterMath::resolve(*ash, *kobold, Dice(1, 1), out);
    assert(out.points() == 1);
}
