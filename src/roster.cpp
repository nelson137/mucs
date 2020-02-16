#include "config.hpp"


void from_json(const json& j, Roster& roster) {
    auto& config = Config::get();
    string user_orig, user, lab_ids, id;
    for (auto& entry : j.items()) {
        if (entry.value().type() != json::value_t::string)
            throw mucs_exception::config(
                "Roster entries must be of type string",
                config.filename,
                "roster",
                entry.key());

        // Normalize user (lowercase)
        user = user_orig = entry.key();
        stl_transform(user, ::tolower);

        lab_ids = entry.value().get<string>();

        for (auto&& id_orig : string_split(lab_ids, ",")) {
            id = id_orig;
            // Normalize lab ids (uppercase)
            stl_transform(id, ::toupper);
            if (not stl_contains(config.lab_ids, id))
                throw mucs_exception::config(
                    "Lab id not recognized",
                    config.filename,
                    "roster",
                    user_orig);
            roster[user].push_back(id);
        }
    }
}


void to_json(json& j, const Roster& roster) {
    j = json::object();
    for (auto& entry : roster)
        j[entry.first] = stl_join(entry.second);
}
