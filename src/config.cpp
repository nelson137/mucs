#include "config.hpp"


Config::Config() {
}


Config& Config::get() {
    static Config instance;
    return instance;
}


Config& Config::parse(const json& root) {
    get_to_required(root, "course_id",   "string", this->course_id);
    get_to_required(root, "admin_hash",  "string", this->admin_hash);
    get_to_required(root, "homeworks",   "object", this->homeworks);
    get_to_required(root, "current_lab", "string", this->current_lab);
    get_to_required(root, "labs",        "object", this->lab_sessions);
    get_to_required(root, "roster",      "object", this->roster);
    return *this;
}


Config& Config::parse_file(const Path& p) {
    const string& filename = p.str();

    if (not p.exists())
        throw mucs_exception("Config file does not exist: " + filename);
    if (not p.is_file())
        throw mucs_exception(
            "Config path must be a regular file: " + filename);

    auto data = json::object();
    ifstream fs(filename);

    try {
        fs >> data;
        fs.close();
    } catch (const json::parse_error& pe) {
        fs.close();
        throw mucs_exception("Invalid json: " + filename);
    }

    this->filename = filename;
    return this->parse(data);
}


string Config::get_assignment(const string& type) const {
    if (type == "lab")
        return this->get_current_lab();
    else if (type == "hw")
        return this->get_current_hw();

    throw mucs_exception("Assignment type not recognized: " + type);
}


string Config::get_current_lab() const {
    return this->current_lab;
}


string Config::get_current_hw() const {
    system_clock::time_point now = system_clock::now();
    for (auto& e : this->homeworks)
        if (now < e.second.duedate)
            return e.first;

    throw mucs_exception(
        "No open homework assignments for course: " + this->course_id);
}


void from_json(const json& j, Config& c) {
    if (j.count("course_id") > 0)
        j["course_id"].get_to(c.course_id);

    if (j.count("admin_hash") > 0)
        j["admin_hash"].get_to(c.admin_hash);

    if (j.count("homeworks") > 0)
        j["homeworks"].get_to(c.homeworks);

    if (j.count("labs") > 0)
        j["labs"].get_to(c.lab_sessions);

    if (j.count("roster") > 0)
        j["roster"].get_to(c.roster);
}


void to_json(json& j, const Config& c) {
    j = {
        {"course_id", c.course_id},
        {"admin_hash", c.admin_hash},
        {"homeworks", c.homeworks},
        {"labs", c.lab_sessions},
        {"current_lab", c.current_lab},
        {"roster", c.roster},
    };
}
