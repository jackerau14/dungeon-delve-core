#ifndef DD_DOMAIN_TYPES_H
#define DD_DOMAIN_TYPES_H

#include "dungeon_delve.h"
#include <stdexcept>

// Domain values are intentionally different types: gold cannot be used as HP.
class Coin {
    int amount_;
public:
    explicit Coin(int amount = 0) : amount_(amount) {
        if (amount < 0) throw std::invalid_argument("Coin cannot be negative");
    }
    int amount() const noexcept { return amount_; }
    Coin operator+(const Coin& rhs) const;
    Coin operator-(const Coin& rhs) const;
    bool operator==(const Coin& rhs) const noexcept { return amount_ == rhs.amount_; }
};

class Damage {
    int points_;
public:
    explicit Damage(int points = 0) : points_(points) {
        if (points < 0) throw std::invalid_argument("Damage cannot be negative");
    }
    int points() const noexcept { return points_; }
    Damage operator+(const Damage& rhs) const;
};

class Dice {
    int count_;
    int sides_;
public:
    Dice(int count, int sides) : count_(count), sides_(sides) {
        if (count <= 0 || sides <= 0) throw std::invalid_argument("Dice need positive count and sides");
    }
    int count() const noexcept { return count_; }
    int sides() const noexcept { return sides_; }
};

class Vec2 {
    int x_;
    int y_;
public:
    Vec2(int x, int y) noexcept : x_(x), y_(y) {}
    int x() const noexcept { return x_; }
    int y() const noexcept { return y_; }
    Vec2 operator+(const Vec2& rhs) const noexcept { return Vec2(x_ + rhs.x_, y_ + rhs.y_); }
    bool operator==(const Vec2& rhs) const noexcept { return x_ == rhs.x_ && y_ == rhs.y_; }
};

// An instance has its own balance; it is NOT a static-only helper.
class Purse {
    Coin balance_;
public:
    Purse() = default;
    explicit Purse(Coin starting) : balance_(starting) {}
    Coin total() const noexcept { return balance_; }
    void deposit(const Coin& coins) { balance_ = balance_ + coins; }
    void spend(const Coin& coins) { balance_ = balance_ - coins; }
};

// All functionality is static: creating an EncounterMath object makes no sense.
class EncounterMath {
    EncounterMath() = delete;
public:
    // Overload 1: predetermined damage; caller gets a typed result by value.
    static Damage resolve(Hero& hero, const Monster& enemy, Damage incoming);
    // Overload 2: roll dice and return a typed result by value.
    static Damage resolve(Hero& hero, const Monster& enemy, const Dice& dice);
    // Overload 3: roll dice and write the result into an out parameter.
    static void resolve(Hero& hero, const Monster& enemy, const Dice& dice, Damage& dealt);
};

#endif
