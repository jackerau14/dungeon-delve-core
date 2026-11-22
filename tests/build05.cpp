#include "data_layer.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

int main() {
    auto bestiary = build_bestiary();
    assert(bestiary.size() == 9);
    assert(find_in_bestiary(bestiary, "kobold") != nullptr);
    assert(find_in_bestiary(bestiary, "kobold")->hp == 12);
    assert(find_in_bestiary(bestiary, "not_a_monster") == nullptr);
    assert(count_species_in_bestiary(bestiary, "kobold") == 1);
    assert(count_species_in_bestiary(bestiary, "not_a_monster") == 0);

    dd::PartyRoster roster;
    roster.add({"Bree",84,12}); roster.add({"Cato",61,9}); roster.add({"Ash",100,15});
    // Function object comparator.
    auto hp = roster.sorted_by(dd::ByHP{});
    assert(hp[0].name == "Ash" && hp[1].name == "Bree" && hp[2].name == "Cato");
    // Function pointer comparator.
    bool (*name_cmp)(const dd::RosterEntry&, const dd::RosterEntry&) = &dd::by_name;
    auto by_name = roster.sorted_by(name_cmp);
    assert(by_name[0].name == "Ash" && by_name[1].name == "Bree");
    // Lambda comparator.
    auto strength = roster.sorted_by([](const auto& a, const auto& b) { return a.strength < b.strength; });
    assert(strength[0].name == "Cato" && roster.members()[0].name == "Bree");

    dd::SpeciesSeen seen;
    for (const char* s : {"kobold", "goblin", "bat", "adept", "warg", "priest", "ogre", "kobold", "bat", "goblin", "warg"}) seen.encounter(s);
    assert(seen.size() == 7 && seen.contains("kobold") && !seen.contains("dragon"));

    dd::Registry<int> scores;
    assert(scores.add("ash", 100) && !scores.add("ash", 0));
    assert(scores.find("ash") && *scores.find("ash") == 100);
    dd::Registry<std::string> titles;
    assert(titles.add("ash", "hero") && *titles.find("ash") == "hero");
    dd::Pool<int> nums;
    nums.add(1); nums.add(2); nums.add(3); assert(nums.remove_at(1) && nums.at(1) == 3);
    dd::Pool<std::string> names;
    names.add("copper"); names.add("silver"); assert(names.remove_at(0) && names.at(0) == "silver");

    const char* legacy[3] = {"Rusted Dagger", "Tin Whistle", "Glass Eye"};
    auto imported = dd::LegacyLootBridge::import(legacy);
    auto exported = dd::LegacyLootBridge::export_pointers<3>(imported);
    for (std::size_t i = 0; i < 3; ++i) {
        assert(std::strcmp(exported[i], legacy[i]) == 0);
        assert(exported[i] == imported.at(i).c_str()); // no second text copy
        assert(legacy[i] != nullptr); // original C array unchanged
    }
    auto engine_loot = dd::LegacyLootBridge::import_engine_table();
    assert(engine_loot.size() == static_cast<std::size_t>(loot_count()));
    auto view = dd::LegacyLootBridge::export_pointers<12>(engine_loot);
    for (int i = 0; i < loot_count(); ++i) assert(std::strcmp(view[i], loot_at(i)) == 0);
    std::puts("Build 05 data layer checks passed");
}
