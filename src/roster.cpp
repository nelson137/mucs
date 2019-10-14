#include "roster.hpp"


Roster::Roster() {
}


Roster::Roster(ICourseConfig& config, vector<string> letters)
: lab_letters(letters) {
    this->parse(config);
}


string Roster::parse_path(const string& child_key) const {
    return this->filename + "[\"" + this->key + "\"][\"" + child_key + "\"]";
}


void Roster::parse(ICourseConfig& config) {
    this->filename = config.filename;

    json roster = config[this->key];
    string user_orig, user, lab_id_orig, lab_id;

    for (auto& entry : roster.items()) {
        if (entry.value().type() != json::value_t::string)
            throw mucs_exception(
                "Roster entries must be of type string: " +
                this->parse_path(entry.key()));

        // Normalize user (lowercase)
        user_orig = entry.key();
        user = user_orig;
        transform(user.begin(), user.end(), user.begin(), ::tolower);

        // Normalize lab ids (uppercase)
        lab_id_orig = entry.value().get<string>();
        lab_id = lab_id_orig;
        transform(lab_id.begin(), lab_id.end(), lab_id.begin(), ::toupper);

        auto begin = this->lab_letters.begin();
        auto end = this->lab_letters.end();
        if (::find(begin, end, lab_id) == end)
            throw mucs_exception(
                "Lab session letter '" + lab_id_orig + "' not recognized: " +
                this->parse_path(user));

        (*this)[user] = lab_id;
    }
}
