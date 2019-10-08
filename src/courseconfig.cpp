#include "courseconfig.hpp"


ICourseConfig::ICourseConfig() {
    this->rand_filename();
}


ICourseConfig::ICourseConfig(initializer_list_t j) : json(j) {
    this->rand_filename();
}


ICourseConfig::ICourseConfig(string filename, json data) : json(data) {
    this->filename = filename;
}


void ICourseConfig::rand_filename() {
    this->filename = "/tmp/mock_course_config." + rand_string();
}


CourseConfig::CourseConfig(
    string filename,
    json data
) : ICourseConfig(filename, data) {
    this->require_prop("course_number", json::value_t::string);
    this->require_prop("admin_hash", json::value_t::string);
    this->require_prop("homeworks", json::value_t::object);
    this->require_prop("labs", json::value_t::object);
    this->require_prop("roster", json::value_t::object);

    for (auto& el : this->items()) {
        if (el.key() == "homeworks")
            (*this)["homeworks"] = Homeworks(*this);
        else if (el.key() == "labs")
            (*this)["labs"] = LabSessions(*this);
        else if (el.key() == "roster")
            (*this)["roster"] = Roster(*this);
    }
}


void CourseConfig::require_prop(string& key, json::value_type type) const {
    string t_name(type.type_name());

    if (this->count(key) == 0)
        throw mucs_exception(
            "Config must specify a '" + key + "' " + t_name + ": " +
            this->filename);

    if ((*this)[key].type() != type)
        throw mucs_exception(
            "Config expected type '" + t_name + "' for key \"" + key + "\": " +
            this->filename);
}


void CourseConfig::require_prop(string&& key, json::value_type type) const {
    this->require_prop(key, type);
}


MockCourseConfig::MockCourseConfig(initializer_list_t j) : ICourseConfig(j) {
}


void MockCourseConfig::require_prop(string& key, json::value_type type) const {
}


void MockCourseConfig::require_prop(string&& key, json::value_type type) const {
}
