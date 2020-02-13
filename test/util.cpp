#include "util.hpp"


string error_id_unrecognized(
    const string& fn,
    const string& user,
    const string& id
) {
    return "Lab id not recognized: " + fn + "[\"roster\"][\"" + user + "\"]";
}


string error_prop(const string& fn, const string& k, const string& t) {
    return "Config requires key \"" + k + "\" with type " + t + ": " + fn;
}


Config& clean_config() {
    auto& config = Config::get();

    config.filename = rand_string();

    if (config.course_id.size())    config.course_id = "";
    if (config.admin_hash.size())   config.admin_hash = "";
    if (config.homeworks.size())    config.homeworks = {};
    if (config.lab_sessions.size()) config.lab_sessions = {};
    if (config.current_lab.size())  config.current_lab = "";
    if (config.roster.size())       config.roster = {};

    return config;
}


json new_config_data(json j) {
    clean_config();

    if (j.count("course_id") == 0)   j["course_id"] = rand_string(4);
    if (j.count("admin_hash") == 0)  j["admin_hash"] = "!";
    if (j.count("homeworks") == 0)   j["homeworks"] = json::object();
    if (j.count("labs") == 0)        j["labs"] = json::object();
    if (j.count("current_lab") == 0) j["current_lab"] = "";
    if (j.count("roster") == 0)      j["roster"] = json::object();

    return j;
}


Config& new_config(const json& j) {
    return clean_config().parse(new_config_data(j));
}
