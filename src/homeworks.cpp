#include "homeworks.hpp"


Homeworks::Homeworks(ICourseConfig& config) {
    this->parse(config);
}


string Homeworks::parse_path(const string& child_key) const {
    return this->filename + "[\"" + this->key + "\"][\"" + child_key + "\"]";
}


string Homeworks::parse_path(const string&& child_key) const {
    return this->parse_path(child_key);
}


void Homeworks::parse(ICourseConfig& config) {
    this->filename = config.filename;

    json homeworks = config[this->key];
    string hw_name;

    for (auto& hw : homeworks.items()) {
        if (hw.value().type() != json::value_t::string)
            throw mucs_exception(
                "Homework entries must be of type string: " +
                this->parse_path(hw.key()));

        // Normalize homework names (lowercase)
        hw_name = hw.key();
        transform(hw_name.begin(), hw_name.end(), hw_name.begin(), ::tolower);

        (*this)[hw_name] = hw.value();
    }
}
