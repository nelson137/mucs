#include "config.hpp"


Config Config::parse(const json& root) {
    Config config;

    parse_key(root, "course_id", "string", config.course_id,
        "Config objects require key 'course_id' of type 'string'");
    parse_key(root, "admin_hash", "string", config.admin_hash,
        "Config objects require key 'admin_hash' of type 'string'");
    parse_key(root, "homeworks", "array", config.homeworks,
        "Config objects require key 'homeworks' of type 'array'");
    parse_key(root, "lab-sessions", "object", config.lab_sessions,
        "Config objects require key 'lab-sessions' of type 'object'");
    parse_key(root, "lab-assignments", "object", config.lab_assignments,
        "Config objects require key 'lab-assignments' of type 'object'");
    parse_key(root, "roster", "object", config.roster,
        "Config objects require key 'roster' of type 'object'");

    /**
     * for user,labs in roster:
     *   for l in labs:
     *     if l not in lab_sessions:
     *       raise Exception
     */
    for (const auto& roster_e : config.roster)
        for (const string& lab : roster_e.second)
            if (config.lab_sessions.find(lab) == config.lab_sessions.end())
                throw Config::error(
                    "Lab id not recognized", {"roster", roster_e.first});

    return config;
}


Config Config::parse_file(const IPath& p) {
    const string& filename = p.str();

    if (not p.exists())
        throw mucs_exception("Config file does not exist: " + filename);
    if (not p.is_file())
        throw mucs_exception(
            "Config path must be a regular file: " + filename);

    json data;
    try {
        data = json::parse(p.read());
    } catch (const json::parse_error& pe) {
        throw mucs_exception("Invalid json: " + filename);
    }

    Config config = Config::parse(data);
    config.filename = filename;
    return config;
}


mucs_exception Config::error(
    const string& msg,
    const initializer_list<string>& keys
) {
    ostringstream ret;
    ret << msg << ": {filename}";
    for (const auto& k : keys)
        ret << "[\"" << k << "\"]";
    return ret.str();
}


string Config::get_assignment(const string& type) const {
    if (type == "lab")
        return this->get_current_lab();
    else if (type == "hw")
        return this->get_current_hw();

    throw mucs_exception("Assignment type not recognized: " + type);
}


string Config::get_current_lab() const {
    for (auto& e : this->lab_assignments)
        if (e.second.start <= NOW && NOW < e.second.end)
            return e.first;

    throw mucs_exception(
        "No open lab assignments for course: " + this->course_id);
}


string Config::get_current_hw() const {
    for (const Hw& hw : this->homeworks)
        if (NOW < hw.duedate)
            return hw.name;

    throw mucs_exception(
        "No open homework assignments for course: " + this->course_id);
}


LabSesh Config::get_lab(const string& user) const {
    const vector<LabSesh>& user_labs = this->get_user_labs(user);
    LabSesh lab;

    if (user_labs.size() == 1) {
        lab = user_labs[0];
        if (lab.is_active() == false)
            throw mucs_exception(lab.format(
                "Lab {id} is not in session: {weekday} from {start} to {end}"
            ));
    } else {
        auto active_lab = stl_find_if(user_labs, mem_fn(&LabSesh::is_active));
        if (active_lab == user_labs.end())
            throw mucs_exception(
                "None of your labs are in session:", stl_join(user_labs));
        lab = *active_lab;
    }

    return lab;
}


vector<LabSesh> Config::get_user_labs(const string& user) const {
    const vector<string>& user_lab_ids = this->roster.at(user);
    vector<LabSesh> user_labs;
    user_labs.reserve(user_lab_ids.size());

    transform(
        user_lab_ids.begin(),
        user_lab_ids.end(),
        back_inserter(user_labs),
        [&] (const string& id) -> const LabSesh& {
            return this->lab_sessions.at(id);
        }
    );

    return user_labs;
}


void from_json(const json& j, Config& c) {
    if (j.count("course_id") > 0)
        j["course_id"].get_to(c.course_id);

    if (j.count("admin_hash") > 0)
        j["admin_hash"].get_to(c.admin_hash);

    if (j.count("homeworks") > 0)
        j["homeworks"].get_to(c.homeworks);

    if (j.count("lab-sessions") > 0)
        j["lab-sessions"].get_to(c.lab_sessions);

    if (j.count("lab-assignments") > 0)
        j["lab-assignments"].get_to(c.lab_assignments);

    if (j.count("roster") > 0)
        j["roster"].get_to(c.roster);
}


void to_json(json& j, const Config& c) {
    j = {
        {"course_id", c.course_id},
        {"admin_hash", c.admin_hash},
        {"homeworks", c.homeworks},
        {"lab-sessions", c.lab_sessions},
        {"lab-assignments", c.lab_assignments},
        {"roster", c.roster},
    };
}
