#include "domain_types.h"
#include <cstdio>

int main() {
    HeroPtr hero = make_hero("Ash", 100, 10, 0);
    MonsterPtr kobold = make_monster("Kobold", "kobold", 25, 4, 0, 24, ABIL_NONE);
    const Dice dice(3, 6);
    // An example roll of 11 is used here so the reference output is repeatable.
    const Damage example_roll(11);
    Damage landed = EncounterMath::resolve(*hero, *kobold, example_roll);
    Purse purse;
    purse.deposit(Coin(kobold->gold_drop));
    std::printf("> resolve(hero, kobold)\n");
    std::printf("Kobold rolls %dd%d .......... %d damage\n", dice.count(), dice.sides(), landed.points());
    std::printf("Ash takes %d .............. %d / %d HP\n", landed.points(), hero->hp, hero->max_hp);
    std::printf("Loot ...................... %d gold\n\n", purse.total().amount());
    std::printf("> purse.total()\nCoin{ %d }\n", purse.total().amount());
}
