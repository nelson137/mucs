#include "labsessions.hpp"


LabSessions::LabSessions() {
}


LabSessions::LabSessions(ICourseConfig& config) {
    this->parse(config);
}


string LabSessions::parse_path(const string& child_key) const {
    return this->filename + "[\"" + this->key + "\"][\"" + child_key + "\"]";
}


void LabSessions::parse(ICourseConfig& config) {
    this->filename = config.filename;

    json lab_sessions = config[this->key];
    string lab_id;

    for (auto& lab : lab_sessions.items()) {
        if (lab.value().type() != json::value_t::string)
            throw mucs_exception(
                "Lab entries must be of type string: " +
                this->parse_path(lab.key()));

        // Normalize lab ids (uppercase)
        lab_id = lab.key();
        transform(lab_id.begin(), lab_id.end(), lab_id.begin(), ::toupper);

        (*this)[lab_id] = lab.value();
        this->all_ids.push_back(lab_id);
    }
}
