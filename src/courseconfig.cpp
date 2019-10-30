#include "courseconfig.hpp"


ICourseConfig::ICourseConfig() {
}


void ICourseConfig::parse(const json& j) {
    if (j.count("filename") == 0)
        throw mucs_exception("Config has no filename");
    j["filename"].get_to(this->filename);

    auto require_prop = [&](
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
    };

    require_prop("course_id", json::value_t::string);
    this->course_id = j["course_id"];

    require_prop("admin_hash", json::value_t::string);
    this->admin_hash = j["admin_hash"];

    auto error_msg = [&](const string& msg, const string& a, const string& b) {
        return msg + ": " + this->filename + "[\"" + a + "\"][\"" + b + "\"]";
    };

    require_prop("homeworks", json::value_t::object);
    json homeworks = j["homeworks"];
    string hw_name;
    for (auto& hw : homeworks.items()) {
        if (hw.value().type() != json::value_t::string)
            throw mucs_exception(error_msg(
                "Homework entries must be of type string",
                "homeworks",
                hw.key()));

        // Normalize homework names (lowercase)
        hw_name = hw.key();
        transform(hw_name.begin(), hw_name.end(), hw_name.begin(), ::tolower);

        this->homeworks[hw_name] = hw.value();
    }

    vector<string> all_lab_ids;

    require_prop("labs", json::value_t::object);
    json lab_sessions = j["labs"];
    string lab_id;
    for (auto& lab : lab_sessions.items()) {
        if (lab.value().type() != json::value_t::string)
            throw mucs_exception(error_msg(
                "Lab entries must be of type string",
                "labs",
                lab.key()));

        // Normalize lab ids (uppercase)
        lab_id = lab.key();
        transform(lab_id.begin(), lab_id.end(), lab_id.begin(), ::toupper);

        this->lab_sessions[lab_id] = lab.value();
        all_lab_ids.push_back(lab_id);
    }

    require_prop("roster", json::value_t::object);
    json roster = j["roster"];
    string user_orig, user, lab_ids, id;
    for (auto& entry : roster.items()) {
        if (entry.value().type() != json::value_t::string)
            throw mucs_exception(error_msg(
                "Roster entries must be of type string",
                "roster",
                entry.key()));

        // Normalize user (lowercase)
        user = user_orig = entry.key();
        transform(user.begin(), user.end(), user.begin(), ::tolower);

        lab_ids = entry.value().get<string>();

        this->roster[user] = {};

        for (auto&& id_orig : string_split(lab_ids)) {
            id = id_orig;
            // Normalize lab ids (uppercase)
            transform(id.begin(), id.end(), id.begin(), ::toupper);
            if (not vector_contains(all_lab_ids, id))
                throw mucs_exception(error_msg(
                    "Lab id '" + id_orig + "' not recognized",
                    "roster",
                    user_orig));
            this->roster[user].push_back(id);
        }
    }
}


CourseConfig::CourseConfig() {
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
        this->filename = j["filename"];

    if (j.count("course_id") > 0)
        this->course_id = j["course_id"].get<string>();

    if (j.count("admin_hash") > 0)
        this->admin_hash = j["admin_hash"].get<string>();

    if (j.count("homeworks") > 0)
        this->homeworks = j["homeworks"].get<map<string,string>>();

    if (j.count("labs") > 0)
        this->lab_sessions = j["labs"].get<map<string,string>>();

    if (j.count("roster") > 0)
        this->roster = j["roster"].get<map<string,vector<string>>>();
}


void from_json(const json& j, CourseConfig& cc) {
    cc.parse(j);
}


void to_json(json& j, const CourseConfig& cc) {
    j = {
        {"filename", cc.filename},
        {"course_id", cc.course_id},
        {"admin_hash", cc.admin_hash},
        {"homeworks", cc.homeworks},
        {"labs", cc.lab_sessions},
        {"roster", cc.roster},
    };
}
