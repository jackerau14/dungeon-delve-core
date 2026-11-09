#include "dungeon_delve.h"

#include <iostream>
#include <string>

std::string Hero::describe() const {
    return std::string("[Hero]     ") + name + "  " +
           std::to_string(hp) + "/" + std::to_string(max_hp) + " HP  combatant";
}

std::string Monster::describe() const {
    return std::string("[Monster]  ") + name + "  " +
           std::to_string(hp) + "/" + std::to_string(max_hp) +
           " HP  combatant, lootable";
}

std::string Merchant::describe() const {
    return std::string("[Merchant] ") + name + "  --  lootable";
}

void describe_entity(const Entity& entity) {
    std::cout << entity.describe() << '\n';
}

void describe_roster(const std::vector<const Entity*>& entities) {
    for (const Entity* entity : entities) {
        if (entity) describe_entity(*entity);
    }
}

int combatant_hp(const Combatant& combatant) {
    return combatant.current_hp();
}

int lootable_gold(const Lootable& lootable) {
    return lootable.loot_gold();
}
