#ifndef DD_DATA_LAYER_H
#define DD_DATA_LAYER_H

#include "dungeon_delve.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace dd {

// Keyed registry: O(log n) lookup and insertion, unique keys.
template<class T>
class Registry {
public:
    bool add(std::string key, T value) {
        return entries_.emplace(std::move(key), std::move(value)).second;
    }
    const T* find(const std::string& key) const {
        auto it = entries_.find(key);
        return it == entries_.end() ? nullptr : &it->second;
    }
    std::size_t size() const noexcept { return entries_.size(); }
private:
    std::map<std::string, T> entries_;
};

// Ordered, contiguous storage with value semantics.
template<class T>
class Pool {
public:
    void add(T item) { items_.push_back(std::move(item)); }
    bool remove_at(std::size_t index) {
        if (index >= items_.size()) return false;
        items_.erase(items_.begin() + static_cast<std::ptrdiff_t>(index));
        return true;
    }
    std::size_t size() const noexcept { return items_.size(); }
    const T& at(std::size_t index) const { return items_.at(index); }
    const std::vector<T>& values() const noexcept { return items_; }
private:
    std::vector<T> items_;
};

struct RosterEntry {
    std::string name;
    int hp;
    int strength;
};

inline bool by_name(const RosterEntry& a, const RosterEntry& b) {
    return a.name < b.name;
}
struct ByHP {
    bool operator()(const RosterEntry& a, const RosterEntry& b) const {
        if (a.hp != b.hp) return a.hp > b.hp;
        return a.name < b.name;
    }
};

class PartyRoster {
public:
    void add(RosterEntry entry) { entries_.push_back(std::move(entry)); }
    std::size_t size() const noexcept { return entries_.size(); }
    const std::vector<RosterEntry>& members() const noexcept { return entries_; }

    template<class Comparator>
    std::vector<RosterEntry> sorted_by(Comparator compare) const {
        auto view = entries_; // sort a snapshot without changing canonical roster
        std::sort(view.begin(), view.end(), compare);
        return view;
    }
private:
    std::vector<RosterEntry> entries_;
};

class SpeciesSeen {
public:
    bool encounter(const std::string& species) { return species_.insert(species).second; }
    std::size_t size() const noexcept { return species_.size(); }
    bool contains(const std::string& species) const { return species_.contains(species); }
private:
    std::set<std::string> species_;
};

// Keep the source C array unchanged. One copy of each name into an owning Pool
// is necessary; the reverse view borrows pointers from Pool's strings.
// IMPORTANT: do not add/remove items in Pool while a borrowed view is in use.
class LegacyLootBridge {
public:
    template<std::size_t N>
    static Pool<std::string> import(const char* const (&source)[N]) {
        Pool<std::string> result;
        for (const char* name : source) result.add(name ? name : "");
        return result;
    }

    static Pool<std::string> import_engine_table() {
        Pool<std::string> result;
        for (int i = 0; i < loot_count(); ++i) result.add(loot_at(i));
        return result;
    }

    template<std::size_t N>
    static std::array<const char*, N> export_pointers(const Pool<std::string>& pool) {
        if (pool.size() != N) throw std::invalid_argument("loot size mismatch");
        std::array<const char*, N> out{};
        for (std::size_t i = 0; i < N; ++i) out[i] = pool.at(i).c_str();
        return out;
    }
};

} // namespace dd
#endif
