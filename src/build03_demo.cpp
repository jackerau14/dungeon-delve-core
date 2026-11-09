#include "dungeon_delve.h"

int main() {
    auto ash = make_hero("Ash", 100, 10, 3);
    auto kobold = make_monster("Kobold", "kobold", 12, 5, 1, 24, ABIL_BITE);
    auto yarrow = make_merchant("Old Yarrow", 100, 10);
    const std::vector<const Entity*> roster{ash.get(), kobold.get(), yarrow.get()};
    describe_roster(roster);
}
