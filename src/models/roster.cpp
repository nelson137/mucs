#include "config.hpp"


list<vector<string>> Roster::to_table() const {
    list<vector<string>> table;
    for (auto it=this->begin(); it!=this->end(); it++) {
        // Create row
        vector<string> row;
        row.reserve(2);
        // Column 1
        row.push_back(it->first);
        // Column 2
        row.push_back(stl_join(it->second));
        // Append row
        table.push_back(move(row));
    }
    return table;
}


void from_json(const json& j, Roster& roster) {
    string user_orig, user, lab_ids;
    for (auto& entry : j.items()) {
        if (entry.value().type() != json::value_t::string)
            throw Config::error(
                "Roster entries must be of type string",
                {"roster", entry.key()});

        // Normalize user (lowercase)
        user = user_orig = entry.key();
        stl_transform(user, ::tolower);

        lab_ids = entry.value().get<string>();

        for (string id : string_split(lab_ids, ",")) {
            // Normalize lab ids (uppercase)
            stl_transform(id, ::toupper);
            roster[user].push_back(id);
        }
    }
}


void to_json(json& j, const Roster& roster) {
    j = json::object();
    for (auto& entry : roster)
        j[entry.first] = stl_join(entry.second);
}
