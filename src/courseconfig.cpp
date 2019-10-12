#include "courseconfig.hpp"


ICourseConfig::ICourseConfig(json data) : json(data) {
}


CourseConfig::CourseConfig(string filename, json data) : ICourseConfig(data) {
    this->filename = filename;

    this->require_prop("course_number", json::value_t::string);
    this->require_prop("admin_hash", json::value_t::string);
    this->require_prop("homeworks", json::value_t::object);
    this->require_prop("labs", json::value_t::object);
    this->require_prop("roster", json::value_t::object);

    LabSessions lab_sessions;
    (*this)["homeworks"] = Homeworks(*this);
    (*this)["labs"] = lab_sessions = LabSessions(*this);
    (*this)["roster"] = Roster(*this, lab_sessions.all_letters);
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
    this->filename = "/tmp/mock_course_config." + rand_string();
}


void MockCourseConfig::require_prop(string& key, json::value_type type) const {
}


void MockCourseConfig::require_prop(string&& key, json::value_type type) const {
}
