#include "roster.hpp"


Roster::Roster() {
}


Roster::Roster(ICourseConfig& config) {
    this->parse(config);
}


string Roster::parse_path(const string& child_key) const {
    return this->filename + "[\"" + this->key + "\"][\"" + child_key + "\"]";
}


string Roster::parse_path(const string&& child_key) const {
    return this->parse_path(child_key);
}


void Roster::parse(ICourseConfig& config) {
    this->filename = config.filename;

    json roster = config[this->key];
    string user, lab_id;

    for (auto& entry : roster.items()) {
        if (entry.value().type() != json::value_t::string)
            throw mucs_exception(
                "Roster entries must be of type string: " +
                this->parse_path(entry.key()));

        // Normalize user (lowercase) and lab ids (uppercase)
        user = entry.key();
        transform(user.begin(), user.end(), user.begin(), ::tolower);
        lab_id = entry.value().get<string>();
        transform(lab_id.begin(), lab_id.end(), lab_id.begin(), ::toupper);

        (*this)[user] = lab_id;
    }
}
