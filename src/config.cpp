#include "config.hpp"


inline Hw::Hw() {
}


inline Hw::Hw(ICourseConfig *cc) : config(cc) {
}


void Hw::parse(const json& j) {
}


inline Homeworks::Homeworks() {
}


inline Homeworks::Homeworks(ICourseConfig *cc) : Homeworks() {
    this->config = cc;
}


void Homeworks::parse(const json& j) {
    string id;
    for (auto& hw : j.items()) {
        if (hw.value().type() != json::value_t::string)
            throw mucs_exception(this->config->error_msg(
                "Homework entries must be of type string",
                "homeworks",
                hw.key()));

        // Normalize homework names (lowercase)
        id = hw.key();
        stl_transform(id, ::tolower);

        this->operator[](id) = Hw(this->config);
        hw.value().get_to(this->operator[](id));
    }
}


inline void from_json(const json& j, Hw& hw) {
    hw.parse(j);
}


inline void from_json(const json& j, Homeworks& homeworks) {
    homeworks.parse(j);
}


void to_json(json& j, const Hw& hw) {
}


void to_json(json& j, const Homeworks& homeworks) {
}


inline LabSesh::LabSesh() {
}


inline LabSesh::LabSesh(
    ICourseConfig *cc,
    const string& i
) : config(cc), id(i) {
}


void LabSesh::parse(const json& j) {
    if (j.type() != json::value_t::string)
        throw mucs_exception(this->config->error_msg(
            "Lab entries must be of type string",
            "labs",
            this->id));

    // Normalize lab time (lowercase)
    string lab_time = j.get<string>();
    stl_transform(lab_time, ::tolower);

    smatch cm;
    regex lab_re(R"((\S+)\s*(\S+)\s*-\s*(\S+))");
    if (not regex_match(lab_time, cm, lab_re))
        throw mucs_exception(this->config->error_msg(
            "Lab entries must be in the format " \
                "\"<weekday> <start_time> - <end_time>\"",
            "labs",
            this->id));

    this->weekday = parse_weekday(cm[1]);
    this->start = parse_time(cm[2]);
    this->end = parse_time(cm[3]);
}


bool LabSesh::is_active() const {
    // auto now = current_time();
    // if (now.weekday() != this->weekday)
        // return false;
    // return DateTime::compare_time(this->start, now) <= 0 &&
        // DateTime::compare_time(now, this->end) < 0;
    return false;
}


tuple<string,string,string> LabSesh::get_pretty() const {
    return make_tuple(
        format_weekday(this->weekday),
        format_time(this->start),
        format_time(this->end)
    );
}


inline LabSessions::LabSessions() : map<string,LabSesh>() {
}


inline LabSessions::LabSessions(ICourseConfig *cc) : LabSessions() {
    this->config = cc;
}


void LabSessions::parse(const json& j) {
    string id;
    for (auto& lab : j.items()) {
        // Normalize lab ids (uppercase)
        id = lab.key();
        stl_transform(id, ::toupper);

        this->operator[](id) = LabSesh(this->config, id);
        lab.value().get_to(this->operator[](id));
        this->all_ids.push_back(id);
    }
}


inline void from_json(const json& j, LabSesh& ls) {
    ls.parse(j);
}


inline void from_json(const json& j, LabSessions& lab_sessions) {
    lab_sessions.parse(j);
}


void to_json(json& j, const LabSesh& ls) {
    string weekday, start, end;
    tie(weekday, start, end) = ls.get_pretty();
    string repr = weekday + " " + start + " - " + end;
    j = { {ls.id, repr} };
}


void to_json(json& j, const LabSessions& lab_sessions) {
}


inline Roster::Roster() : map<string,vector<string>>() {
}


inline Roster::Roster(ICourseConfig *cc) : Roster() {
    this->config = cc;
}


void Roster::parse(const json& j) {
    string user_orig, user, lab_ids, id;
    for (auto& entry : j.items()) {
        if (entry.value().type() != json::value_t::string)
            throw mucs_exception(this->config->error_msg(
                "Roster entries must be of type string",
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
            if (not stl_contains(this->config->lab_sessions.all_ids, id))
                throw mucs_exception(this->config->error_msg(
                    "Lab id '" + id_orig + "' not recognized",
                    "roster",
                    user_orig));
            this->operator[](user).push_back(id);
        }
    }
}


inline void from_json(const json& j, Roster& roster) {
    roster.parse(j);
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
    const string& key,
    const json::value_type& type
) {
    string t_name(type.type_name());

    if (j.count(key) == 0)
        throw mucs_exception(
            "Config must specify a '" + key + "' " + t_name + ": " +
            this->filename);

    if (j[key].type() != type)
        throw mucs_exception(
            "Config expected type '" + t_name + "' for key \"" + key +
            "\": " + this->filename);
}


void ICourseConfig::parse(const json& j) {
    this->require_prop(j, "filename", json::value_t::string);
    j["filename"].get_to(this->filename);

    this->require_prop(j, "course_id", json::value_t::string);
    j["course_id"].get_to(this->course_id);

    this->require_prop(j, "admin_hash", json::value_t::string);
    j["admin_hash"].get_to(this->admin_hash);

    this->homeworks = Homeworks(this);
    this->require_prop(j, "homeworks", json::value_t::object);
    j["homeworks"].get_to(this->homeworks);

    this->lab_sessions = LabSessions(this);
    this->require_prop(j, "labs", json::value_t::object);
    j["labs"].get_to(this->lab_sessions);

    this->roster = Roster(this);
    this->require_prop(j, "roster", json::value_t::object);
    j["roster"].get_to(this->roster);
}


inline CourseConfig::CourseConfig() {
}


CourseConfig::CourseConfig(const string& filename) {
    if (not path_exists(filename))
        throw mucs_exception("Config file does not exist: " + filename);
    if (not path_is_file(filename))
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

    data["filename"] = filename;
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
