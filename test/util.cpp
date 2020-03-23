#include "util.hpp"


int current_year() {
    time_t now_t = system_clock::to_time_t(NOW);
    tm *now = localtime(&now_t);
    return now->tm_year + 1900;
}


string error_id_unrecognized(const string& user, const string& id) {
    return "Lab id not recognized: {filename}[\"roster\"][\"" + user + "\"]";
}


string error_prop(const string& k, const string& t) {
    return "Config requires key \"" + k + "\" with type " + t + ": {filename}";
}


json new_config_data(json j) {
    auto default_val = [&] (const string& key, const json& val) {
        if (j.count(key) == 0)
            j[key] = val;
    };

    default_val("filename", rand_string());
    default_val("course_id", rand_string(4));
    default_val("admin_hash", "!");
    default_val("homeworks", json::object());
    default_val("labs", json::object());
    default_val("current_lab", "");
    default_val("roster", json::object());

    return j;
}
