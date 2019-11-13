#include "config.hpp"


inline Hw::Hw() {
}


inline Hw::Hw(const string& fn, const string& n) : filename(fn), name(n) {
    // Normalize name (lowercase)
    stl_transform(this->name, ::tolower);
}


Hw Hw::from_iter(const string& fn, const json::const_iterator& it) {
    Hw hw(fn, it.key());
    it.value().get_to(hw);
    return hw;
}


inline bool Hw::compare::operator()(
    const pair<string,Hw>& a,
    const pair<string,Hw>& b
) const {
    return a.second.duedate < b.second.duedate;
}


inline Homeworks::Homeworks(map<string,Hw> m)
    : set(m.begin(), m.end())
{}


inline Homeworks::Homeworks(initializer_list<pair<string,Hw>> il)
    : set(il.begin(), il.end())
{}


inline Homeworks::Homeworks(const string& fn) : Homeworks() {
    this->filename = fn;
}


inline void from_json(const json& j, Hw& hw) {
    hw.duedate = parse_datetime(j.get<string>());
}


void from_json(const json& j, Homeworks& homeworks) {
    string id;
    for (auto it=j.begin(); it!=j.end(); it++) {
        if (it.value().type() != json::value_t::string)
            throw mucs_exception(error_config(
                "Homework entries must be of type string",
                homeworks.filename,
                "homeworks",
                it.key()));

        auto hw = Hw::from_iter(homeworks.filename, it);
        homeworks.insert({ hw.name, hw });
    }
}


void to_json(json& j, const Hw& hw) {
    time_t tt = duration_cast<seconds>(hw.duedate.time_since_epoch()).count();
    tm *t = localtime(&tt);

    ostringstream os;
    os << put_time(t, "%Y-%m-%d %T");
    j = json(os.str());
}


void to_json(json& j, const Homeworks& homeworks) {
    j = json::object();
    for (auto& hw : homeworks)
        j[hw.first] = json(hw.second);
}


inline LabSesh::LabSesh() {
}


inline LabSesh::LabSesh(
    const string& fn,
    const string& i
) : filename(fn), id(i) {
    // Normalize id (uppercase)
    stl_transform(this->id, ::toupper);
}


LabSesh LabSesh::from_iter(const string& fn, const json::const_iterator& it) {
    LabSesh ls(fn, it.key());
    it.value().get_to(ls);
    return ls;
}


bool LabSesh::is_active() const {
    if (current_weekday() != this->weekday)
        return false;
    time_t now = current_time();
    return this->start <= now && now < this->end;
}


string LabSesh::w_raw() const {
    return to_string(this->weekday);
}


string LabSesh::w_pretty() const {
    return format_weekday(this->weekday);
}


string LabSesh::s_raw() const {
    return format_time(this->start, "%H:%M:%S");
}


string LabSesh::s_pretty() const {
    return format_time(this->start, "%I:%M:%S%P");
}


string LabSesh::e_raw() const {
    return format_time(this->end, "%H:%M:%S");
}


string LabSesh::e_pretty() const {
    return format_time(this->end, "%I:%M:%S%P");
}


inline LabSessions::LabSessions() : map<string,LabSesh>() {
}


inline LabSessions::LabSessions(const string& fn) : LabSessions() {
    this->filename = fn;
}


void from_json(const json& j, LabSesh& ls) {
    if (j.type() != json::value_t::string)
        throw mucs_exception(error_config(
            "Lab entries must be of type string", ls.filename, "labs", ls.id));

    // Normalize lab specification (lowercase)
    string lab_spec = j.get<string>();
    stl_transform(lab_spec, ::tolower);

    regex lab_spec_re(R"(\s*(\S+)\s+(\S+)\s*-\s*(\S+)\s*)");
    smatch match;
    if (not regex_match(lab_spec, match, lab_spec_re))
        throw mucs_exception(error_config(
            "Lab entries must be in the format " \
                "\"<weekday> <start_time> - <end_time>\"",
            ls.filename,
            "labs",
            ls.id));

    ls.weekday = parse_weekday(match[1]);
    ls.start = parse_time(match[2]);
    ls.end = parse_time(match[3]);
}


void from_json(const json& j, LabSessions& lab_sessions) {
    for (auto it=j.begin(); it!=j.end(); it++) {
        auto ls = LabSesh::from_iter(lab_sessions.filename, it);
        lab_sessions[ls.id] = ls;
        lab_sessions.all_ids.push_back(ls.id);
    }
}


void to_json(json& j, const LabSesh& ls) {
    ostringstream repr;
    repr << ls.w_pretty() << ' ' << ls.s_raw() << " - " << ls.s_raw();
    j = { {ls.id, repr.str()} };
}


void to_json(json& j, const LabSessions& lab_sessions) {
    j = json::object();
    for (auto& ls : lab_sessions)
        j[ls.first] = json(ls.second);
}


inline Roster::Roster() : map<string,vector<string>>() {
}


inline Roster::Roster(
    const string& fn,
    const vector<string>& lab_ids
) : Roster() {
    this->filename = fn;
    this->all_lab_ids = lab_ids;
}


void from_json(const json& j, Roster& roster) {
    string user_orig, user, lab_ids, id;
    for (auto& entry : j.items()) {
        if (entry.value().type() != json::value_t::string)
            throw mucs_exception(error_config(
                "Roster entries must be of type string",
                roster.filename,
                "roster",
                entry.key()));

        // Normalize user (lowercase)
        user = user_orig = entry.key();
        stl_transform(user, ::tolower);

        lab_ids = entry.value().get<string>();

        for (auto&& id_orig : string_split(lab_ids)) {
            id = id_orig;
            // Normalize lab ids (uppercase)
            stl_transform(id, ::toupper);
            if (not stl_contains(roster.all_lab_ids, id))
                throw mucs_exception(error_config(
                    "Lab id not recognized",
                    roster.filename,
                    "roster",
                    user_orig));
            roster[user].push_back(id);
        }
    }
}


void to_json(json& j, const Roster& roster) {
    j = json::object();
    for (auto& entry : roster)
        j[entry.first] = entry.second;
}


inline ICourseConfig::ICourseConfig() {
}


inline string ICourseConfig::error_msg(
    const string& msg,
    const string& a,
    const string& b
) {
    return msg + ": " + this->filename + "[\"" + a + "\"][\"" + b + "\"]";
}


void ICourseConfig::require_prop(
    const json& j,
    const string& k,
    const json::value_type& t
) {
    if (j.count(k) == 0 || j[k].type() != t)
        throw mucs_exception(error_config(
            "Config requires key \"" + k + "\" with type " + t.type_name(),
            this->filename));
}


void ICourseConfig::parse(const json& j) {
    this->require_prop(j, "filename",    json::value_t::string);
    this->require_prop(j, "course_id",   json::value_t::string);
    this->require_prop(j, "admin_hash",  json::value_t::string);
    this->require_prop(j, "homeworks",   json::value_t::object);
    this->require_prop(j, "current_lab", json::value_t::string);
    this->require_prop(j, "labs",        json::value_t::object);
    this->require_prop(j, "roster",      json::value_t::object);

    j["filename"].get_to(this->filename);

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


string ICourseConfig::get_current_lab(const string& user) {
    if (not this->roster.count(user))
        throw mucs_exception("User not in course: " + user);

    vector<string> user_labs = this->roster[user];

    if (user_labs.size() == 1) {
        string id = user_labs[0];
        auto ls = this->lab_sessions[id];
        if (not ls.is_active()) {
            ostringstream msg;
            msg << "Lab " << ls.id << " is not in session: " << ls.w_pretty()
                << " from " << ls.s_pretty() << " to " << ls.e_pretty();
            throw mucs_exception(msg.str());
        }
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


string ICourseConfig::get_current_hw() const {
    system_clock::time_point now = system_clock::now();
    for (auto& e : this->homeworks)
        if (now < e.second.duedate)
            return e.first;

    throw mucs_exception(
        "No open homework assignments for course: " + this->course_id);
}


inline CourseConfig::CourseConfig() {
}


CourseConfig::CourseConfig(const Path& p) {
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
        throw mucs_exception(
            "Invalid json: " + p.str() + "\n" +
            string(pe.what()));
    }

    data["filename"] = p.str();
    this->parse(data);
}


MockCourseConfig::MockCourseConfig(const json& j) {
    this->filename = "/tmp/mock_course_config." + rand_string();

    if (j.count("filename") > 0)
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


inline void from_json(const json& j, ICourseConfig& cc) {
    cc.parse(j);
}


inline void to_json(json& j, const ICourseConfig& cc) {
    j = {
        {"filename", cc.filename},
        {"course_id", cc.course_id},
        {"admin_hash", cc.admin_hash},
        {"homeworks", cc.homeworks},
        {"labs", cc.lab_sessions},
        {"roster", cc.roster},
    };
}
