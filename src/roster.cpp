#include "config.hpp"


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
