#include "roster.hpp"


Roster::Roster() : map<string,vector<string>>() {
}


Roster::Roster(
    const string& fn,
    const vector<string>& lab_ids
) : Roster() {
    this->filename = fn;
    this->all_lab_ids = lab_ids;
}


void from_json(const json& j, Roster& roster) {
    string user_orig, user, lab_ids, id;
    for (auto& entry : j.items()) {
        if (entry.value().type() != json::value_t::string)
            throw mucs_exception(error_config(
                "Roster entries must be of type string",
                roster.filename,
                "roster",
                entry.key()));

        // Normalize user (lowercase)
        user = user_orig = entry.key();
        stl_transform(user, ::tolower);

        lab_ids = entry.value().get<string>();

        for (auto&& id_orig : string_split(lab_ids)) {
            id = id_orig;
            // Normalize lab ids (uppercase)
            stl_transform(id, ::toupper);
            if (not stl_contains(roster.all_lab_ids, id))
                throw mucs_exception(error_config(
                    "Lab id not recognized",
                    roster.filename,
                    "roster",
                    user_orig));
            roster[user].push_back(id);
        }
    }
}


void to_json(json& j, const Roster& roster) {
    j = json::object();
    for (auto& entry : roster)
        j[entry.first] = stl_join(entry.second);
}
