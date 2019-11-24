#include "config.hpp"


IConfig::IConfig() {
}


void IConfig::require_prop(
    const json& j,
    const string& k,
    const json::value_type& t
) {
    if (j.count(k) == 0 || j[k].type() != t)
        throw mucs_exception(error_config(
            "Config requires key \"" + k + "\" with type " + t.type_name(),
            this->filename));
}


void IConfig::parse(const json& j) {
    j["filename"].get_to(this->filename);

    this->require_prop(j, "course_id",   json::value_t::string);
    this->require_prop(j, "admin_hash",  json::value_t::string);
    this->require_prop(j, "homeworks",   json::value_t::object);
    this->require_prop(j, "current_lab", json::value_t::string);
    this->require_prop(j, "labs",        json::value_t::object);
    this->require_prop(j, "roster",      json::value_t::object);

    j["course_id"].get_to(this->course_id);

    j["admin_hash"].get_to(this->admin_hash);

    this->homeworks = Homeworks(this->filename);
    j["homeworks"].get_to(this->homeworks);

    j["current_lab"].get_to(this->current_lab);

    this->lab_sessions = LabSessions(this->filename);
    j["labs"].get_to(this->lab_sessions);

    this->roster = Roster(this->filename, this->lab_sessions.all_ids);
    j["roster"].get_to(this->roster);
}


string IConfig::get_current_lab(const string& user) {
    vector<string> user_labs = this->roster[user];

    if (user_labs.size() == 1) {
        string id = user_labs[0];
        auto ls = this->lab_sessions[id];
        if (not ls.is_active())
            throw mucs_exception(ls.format(
                "Lab {id} is not in session: {weekday} from {start} to {end}"
            ));
    } else {
        auto is_active = [&](const string& id) {
            return this->lab_sessions[id].is_active();
        };
        if (none_of(user_labs.begin(), user_labs.end(), is_active))
            throw mucs_exception(
                "None of your labs are in session:", stl_join(user_labs));
    }

    return this->current_lab;
}


string IConfig::get_current_hw() const {
    system_clock::time_point now = system_clock::now();
    for (auto& e : this->homeworks)
        if (now < e.second.duedate)
            return e.first;

    throw mucs_exception(
        "No open homework assignments for course: " + this->course_id);
}


Config::Config() {
}


Config::Config(const Path& p) {
    if (not p.exists())
        throw mucs_exception("Config file does not exist: " + p.str());
    if (not p.is_file())
        throw mucs_exception(
            "Config path must be a regular file: " + p.str());

    auto data = json::object();
    ifstream fs(p.str());

    try {
        fs >> data;
        fs.close();
    } catch (const json::parse_error& pe) {
        fs.close();
        throw mucs_exception("Invalid json: " + p.str());
    }

    data["filename"] = p.str();
    this->parse(data);
}


MockConfig::MockConfig(const json& j) {
    j["filename"].get_to(this->filename);

    if (j.count("course_id") > 0)
        j["course_id"].get_to(this->course_id);

    if (j.count("admin_hash") > 0)
        j["admin_hash"].get_to(this->admin_hash);

    if (j.count("homeworks") > 0)
        j["homeworks"].get_to(this->homeworks);

    if (j.count("labs") > 0)
        j["labs"].get_to(this->lab_sessions);

    if (j.count("roster") > 0)
        j["roster"].get_to(this->roster);
}


void from_json(const json& j, IConfig& cc) {
    cc.parse(j);
}


void to_json(json& j, const IConfig& cc) {
    j = {
        {"filename", cc.filename},
        {"course_id", cc.course_id},
        {"admin_hash", cc.admin_hash},
        {"homeworks", cc.homeworks},
        {"labs", cc.lab_sessions},
        {"current_lab", cc.current_lab},
        {"roster", cc.roster},
    };
}
