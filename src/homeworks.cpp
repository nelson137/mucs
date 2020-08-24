#include "config.hpp"


bool Hw::compare::operator()(const Hw& a, const Hw& b) const {
    return a.duedate < b.duedate;
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
        row.push_back(format_datetime(hw.duedate, HW_FMT));
        // Append row
        table.push_back(move(row));
    }
    return table;
}


void from_json(const json& j, Hw& hw) {
    if (j.is_object() == false)
        throw "Homework objects must be of type object";

    parse_key(j, "name", "string", hw.name,
        "Homework objects require key 'name' of type 'string'");
    // Normalize name (lowercase)
    stl_transform(hw.name, ::tolower);

    parse_key(j, "duedate", "string", [&] (const json& value) {
        hw.duedate = parse_datetime(value.get<string>(), HW_FMT);
    }, "Homework objects require key 'duedate' of type 'string'");
}


void from_json(const json& j, Homeworks& homeworks) {
    for (const json& hw : j)
        homeworks.insert(hw.get<Hw>());
}


void to_json(json& j, const Hw& hw) {
    j = {
        {"name", hw.name},
        {"duedate", format_datetime(hw.duedate, HW_FMT)}
    };
}


void to_json(json& j, const Homeworks& homeworks) {
    j = json::array();
    for (const Hw& hw : homeworks)
        j.push_back(hw);
}
