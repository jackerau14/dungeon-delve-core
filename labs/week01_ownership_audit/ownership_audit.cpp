#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

struct Rune {
    char name[24];
    int charge;
};

std::unique_ptr<Rune> make_rune(const char* name, int charge)
{
    auto r = std::make_unique<Rune>();

    std::strncpy(r->name, name, 23);
    r->name[23] = '\0';
    r->charge = charge;

    return r;
}

/* Returns the total charge of every rune. */
int total_charge(const std::vector<std::unique_ptr<Rune>>& satchel)
{
    int total = 0;

    for (size_t i = 0; i < satchel.size(); i++)
        total += satchel[i]->charge;

    return total;
}

/* Spends a rune's charge. */
int spend_rune(Rune* r) // Non-owning pointer
{
    int spent = r->charge;
    r->charge = 0;
    return spent;
}

/* Builds the starting satchel. */
std::vector<std::unique_ptr<Rune>> build_satchel()
{
    std::vector<std::unique_ptr<Rune>> satchel;

    satchel.push_back(make_rune("Ember", 12));
    satchel.push_back(make_rune("Frost", 8));
    satchel.push_back(make_rune("Gale", 15));

    auto spare = make_rune("Spare", 99);
    (void)spare;

    return satchel;
}

/* Finds the strongest rune. */
const Rune& strongest_rune(
    const std::vector<std::unique_ptr<Rune>>& satchel)
{
    size_t best = 0;

    for (size_t i = 1; i < satchel.size(); i++) {
        if (satchel[i]->charge > satchel[best]->charge)
            best = i;
    }

    return *satchel[best]; // Non-owning reference
}

int main()
{
    std::vector<std::unique_ptr<Rune>> satchel = build_satchel();

    std::printf("runes in satchel ... %d\n", (int)satchel.size());
    std::printf("total charge ....... %d\n", total_charge(satchel));

    const Rune& strongest = strongest_rune(satchel); // Non-owning reference

    std::printf("strongest rune ..... %s (%d)\n",
                strongest.name, strongest.charge);

    std::printf("spent Ember ........ %d\n",
                spend_rune(satchel[0].get()));

    std::printf("total charge ....... %d\n", total_charge(satchel));

    // Smart pointers automatically delete the runes.
    satchel.clear();

    std::printf("satchel emptied.\n");

    return 0;
}
