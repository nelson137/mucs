#include "config.hpp"


Hw::Hw() {
}


Hw::Hw(string n) {
    // Normalize name (lowercase)
    stl_transform(n, ::tolower);
    this->name = n;
}


Hw::Hw(string n, sys_seconds dd) : Hw(n) {
    this->duedate = dd;
}


/**
 * Using name as a secondary sort key makes sure that emplace calls succeed when
 * no duedate is passed. If sorting was done only by duedate, then multiple
 * successive emplace calls, passed only a name, would consider the constructed
 * objects to be equivalent to the first because they would all have a duedate
 * of 0, despite having different names. Also sorting by name fixes this.
 */
bool Hw::compare::operator()(const Hw& a, const Hw& b) const {
    return a.duedate != b.duedate
        ? a.duedate < b.duedate
        : a.name < b.name;
}


void Hw::override() const {
    this->is_overridden = true;
}


bool Hw::is_active() const {
    return this->is_overridden || NOW < this->duedate;
}


const Hw *Homeworks::find_name(const string& name) const {
    auto it = stl_find_if(*this, [&] (const Hw& hw) {
        return hw.name == name;
    });
    /**
     * Why use `&*`: A const_iterator cannot be cast to a pointer, so
     *   dereference the iterator (*) to get a const reference then get the
     *   object's address (&).
     */
    return it == this->end() ? nullptr : &*it;
}


/**
 * Row format: {NAME, DUEDATE}
 */
list<vector<string>> Homeworks::to_table() const {
    list<vector<string>> table;
    for (const Hw& hw : *this) {
        // Create row
        vector<string> row;
        row.reserve(2);
        // Column 1
        row.push_back(hw.name);
        // Column 2
        row.push_back(format(HW_FMT, hw.duedate));
        // Append row
        table.push_back(move(row));
    }
    return table;
}


void from_json(const json& j, Hw& hw) {
    string name = j["name"];
    hw.name = name;
    // Normalize name (lowercase)
    stl_transform(hw.name, ::tolower);

    istringstream iss(j["duedate"].get<string>());
    iss >> date::parse(HW_FMT, hw.duedate);
    if (not iss.good())
        throw Config::error("Invalid duedate", {"homeworks", name});
}


void from_json(const json& j, Homeworks& homeworks) {
    for (const json& hw : j)
        homeworks.insert(hw.get<Hw>());
}


void to_json(json& j, const Hw& hw) {
    j = {
        {"name", hw.name},
        {"duedate", format(HW_FMT, hw.duedate)}
    };
}


void to_json(json& j, const Homeworks& homeworks) {
    j = json::array();
    for (const Hw& hw : homeworks)
        j.push_back(hw);
}
