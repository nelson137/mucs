#include "roster.hpp"


Roster::Roster() {
}


Roster::Roster(ICourseConfig& config, vector<string> ids) : lab_ids(ids) {
    this->parse(config);
}


string Roster::parse_path(const string& child_key) const {
    return this->filename + "[\"" + this->key + "\"][\"" + child_key + "\"]";
}


void Roster::parse(ICourseConfig& config) {
    this->filename = config.filename;

    json roster = config[this->key];
    string user_orig, user, lab_ids, id_orig, id;

    for (auto& entry : roster.items()) {
        if (entry.value().type() != json::value_t::string)
            throw mucs_exception(
                "Roster entries must be of type string: " +
                this->parse_path(entry.key()));

        // Normalize user (lowercase)
        user = user_orig = entry.key();
        transform(user.begin(), user.end(), user.begin(), ::tolower);

        lab_ids = entry.value().get<string>();

        (*this)[user] = json::array();

        for (auto& j_id : json_string_split(lab_ids)) {
            id = id_orig = j_id.get<string>();
            // Normalize lab ids (uppercase)
            transform(id.begin(), id.end(), id.begin(), ::toupper);
            if (not this->is_valid_lab_id(id))
                throw mucs_exception(
                    "Lab id '" + id_orig + "' not recognized: " +
                    this->parse_path(user_orig));
            (*this)[user].push_back(id);
        }
    }
}


bool Roster::is_valid_lab_id(const string& id) {
    auto ll_begin = this->lab_ids.begin();
    auto ll_end = this->lab_ids.end();
    return ::find(ll_begin, ll_end, id) != ll_end;
}
