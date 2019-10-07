#include "labsessions.hpp"


LabSessions::LabSessions(CourseConfig& config) {
    this->filename = config.filename;
    config.require_prop(this->key, this->j_type);
    this->parse(config[this->key]);
}


string LabSessions::parse_path(const string& child_key) const {
    return this->filename + '[' + this->key + "][" + child_key + ']';
}


string LabSessions::parse_path(const string&& child_key) const {
    return this->parse_path(child_key);
}


void LabSessions::parse(json& lab_sessions) {
    string lab_id;

    for (auto& lab : lab_sessions.items()) {
        if (lab.value().type() != json::value_t::string)
            throw mucs_exception(
                "Lab entries must be of type string: " +
                this->parse_path(lab.key()));

        // Normalize lab ids (lowercase)
        lab_id = lab.key();
        transform(lab_id.begin(), lab_id.end(), lab_id.begin(), ::tolower);

        (*this)[lab_id] = lab.value();
    }
}
