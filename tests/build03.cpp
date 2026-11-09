#include "dungeon_delve.h"

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

int main() {
    static_assert(std::has_virtual_destructor_v<Entity>);
    static_assert(std::is_base_of_v<Entity, Hero>);
    static_assert(std::is_base_of_v<Entity, Monster>);
    static_assert(std::is_base_of_v<Entity, Merchant>);
    static_assert(std::is_base_of_v<Combatant, Hero>);
    static_assert(std::is_base_of_v<Combatant, Monster>);
    static_assert(std::is_base_of_v<Lootable, Monster>);
    static_assert(std::is_base_of_v<Lootable, Merchant>);

    HeroPtr hero = make_hero("Ash", 100, 10, 3);
    MonsterPtr monster = make_monster("Kobold", "kobold", 12, 5, 1, 24, ABIL_BITE);
    MerchantPtr merchant = make_merchant("Old Yarrow", 100, 10);

    assert(hero->describe().find("Ash") != std::string::npos);
    assert(monster->describe().find("combatant, lootable") != std::string::npos);
    assert(merchant->describe().find("lootable") != std::string::npos);
    assert(combatant_hp(*hero) == 100);
    assert(static_cast<Combatant&>(*monster).maximum_hp() == 12);
    assert(lootable_gold(*monster) == 24);

    // Diamond test: both paths yield exactly the SAME virtual Entity base.
    Entity* via_combat = static_cast<Combatant*>(monster.get());
    Entity* via_loot = static_cast<Lootable*>(monster.get());
    Entity* direct = static_cast<Entity*>(monster.get());
    assert(via_combat == via_loot && via_loot == direct);

    // Runtime dispatch also works through the base class.
    std::vector<std::unique_ptr<Entity>> owned;
    owned.push_back(make_hero("Second hero", 50, 3, 0));
    owned.push_back(make_monster("Second monster", "test", 20, 4, 0, 5, ABIL_NONE));
    owned.push_back(make_merchant("Second merchant", 60, 10));
    assert(lootable_gold(*merchant) == 100);
    assert(owned[0]->describe().find("[Hero]") != std::string::npos);
    assert(owned[1]->describe().find("[Monster]") != std::string::npos);
    assert(owned[2]->describe().find("[Merchant]") != std::string::npos);
}
