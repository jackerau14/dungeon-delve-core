#include "data_layer.h"
#include <cstdio>

int main() {
    auto bestiary = build_bestiary();
    auto* kobold = find_in_bestiary(bestiary, "kobold");
    if (!kobold) return 1;
    std::printf("> bestiary.find(\"kobold\")\n  %s   %d HP   3d6   lootable   [O(log n)]\n\n", kobold->name, kobold->hp);

    dd::PartyRoster roster;
    roster.add({"Bree", 84, 12});
    roster.add({"Cato", 61, 9});
    roster.add({"Ash", 100, 15});
    auto sorted = roster.sorted_by(dd::ByHP{});
    std::printf("> roster.sorted_by(ByHP{})\n  %s %d | %s %d | %s %d\n\n",
        sorted[0].name.c_str(), sorted[0].hp, sorted[1].name.c_str(), sorted[1].hp,
        sorted[2].name.c_str(), sorted[2].hp);

    dd::SpeciesSeen seen;
    for (const auto* name : {"kobold", "goblin", "bat", "adept", "warg", "priest", "ogre", "kobold", "bat", "goblin", "warg"})
        seen.encounter(name);
    std::printf("> species_seen.size()\n  %zu   (11 encounters, %zu duplicates absorbed)\n", seen.size(), 11 - seen.size());
}
