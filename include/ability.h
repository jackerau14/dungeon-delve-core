#ifndef DD_ABILITY_H
#define DD_ABILITY_H
#include "dungeon_delve.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

// Failure layers: invalid input is rejected at registration/lookup;
// execution errors retain their context at the encounter boundary.
class AbilityError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct AbilityContext {
    Monster& caster;
    Hero& hero;
    Wave& wave;
    int verbose;
};

class Ability {
public:
    virtual ~Ability() = default;
    virtual std::string describe() const = 0;
    virtual int apply(AbilityContext& context) const = 0;
};

// Second override level: Ability -> AttackAbility -> concrete attack.
class AttackAbility : public Ability {
public:
    std::string describe() const override { return "Basic attack"; }
protected:
    int strike(AbilityContext& context) const;
};

using AbilityPtr = std::unique_ptr<Ability>;
using AbilityFactory = AbilityPtr (*)();

// Register named abilities independently from the encounter and dispatcher.
// Registration must use a unique, nonempty name and valid factory.
void register_ability(std::string name, AbilityFactory factory);
AbilityPtr create_ability(std::string_view name);
std::vector<std::string> available_abilities();

// Compatibility adapters preserve the old Build 1-3 API / save format.
AbilityPtr create_legacy_ability(int tag);
const char* legacy_ability_name(int tag);
int apply_ability(const Ability& ability, AbilityContext& context);
int resolve_abilities(const std::vector<const Ability*>& abilities,
                      AbilityContext& context);
#endif
