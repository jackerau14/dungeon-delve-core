#include "ability.h"
#include <cstdio>
#include <vector>
int main() {
    auto hero = make_hero("Ash", 100, 8, 2);
    auto kobold = make_monster("Kobold", "kobold", 12, 4, 1, 6, ABIL_BITE);
    Wave wave;
    wave.push_back(std::move(kobold));
    dd_seed(1337);
    std::printf("> encounter(ash, kobold)\n");
    for (const char* name : {"Cleave", "Bite", "Mend"}) {
        auto ability = create_ability(name);
        AbilityContext context{*wave.front(), *hero, wave, 1};
        apply_ability(*ability, context);
    }
    std::printf("  resolution loop: 4 lines, 0 type checks, 0 casts\n");
    auto extra = create_ability("Sunder");
    std::printf("  registered extension: %s\n", extra->describe().c_str());
}
