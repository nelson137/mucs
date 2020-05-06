#include "config.hpp"


bool Hw::compare::operator()(
    const pair<string,Hw>& a,
    const pair<string,Hw>& b
) const {
    return a.second.duedate < b.second.duedate;
}


list<vector<string>> Homeworks::to_table() const {
    list<vector<string>> table;
    for (const pair<string,Hw>& hw : *this) {
        // Create row
        vector<string> row;
        row.reserve(2);
        // Column 1
        row.push_back(hw.first);
        // Column 2
        row.push_back(format_datetime(hw.second.duedate, HW_FMT));
        // Append row
        table.push_back(move(row));
    }
    return table;
}


void from_json(const json& j, Hw& hw) {
    hw.duedate = parse_datetime(j.get<string>(), HW_FMT);
}


void from_json(const json& j, Homeworks& homeworks) {
    string id;
    for (auto it=j.begin(); it!=j.end(); it++) {
        if (it.value().type() != json::value_t::string)
            throw Config::error(
                "Homework entries must be of type string",
                {"homeworks", it.key()});

        id = it.key();
        // Normalize id (lowercase)
        stl_transform(id, ::tolower);

        homeworks.insert({ id, it.value().get<Hw>() });
    }
}


void to_json(json& j, const Hw& hw) {
    j = json(format_datetime(hw.duedate, HW_FMT));
}


void to_json(json& j, const Homeworks& homeworks) {
    j = json::object();
    for (auto& hw : homeworks)
        j[hw.first] = json(hw.second);
}
