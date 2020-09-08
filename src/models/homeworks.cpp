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


bool Hw::compare::operator()(const Hw& a, const Hw& b) const {
    return a.duedate < b.duedate;
}


bool Hw::is_active() const {
    return NOW < this->duedate;
}


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
    string name = j.value("name", "");
    hw.name = name;
    // Normalize name (lowercase)
    stl_transform(hw.name, ::tolower);

    istringstream iss(j.value("duedate", ""));
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
