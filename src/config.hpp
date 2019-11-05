#ifndef CONFIG_HPP
#define CONFIG_HPP


#include <cctype>
#include <ctime>
#include <fstream>
#include <initializer_list>
#include <map>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

#include "json.hpp"

#include "mucs/except.hpp"
#include "mucs/random.hpp"

#include "util.hpp"

#define CONFIG_DIR "/group/cs1050/config.d"

using namespace std;
using json = nlohmann::json;


struct ICourseConfig;


struct Hw {

    ICourseConfig *config;
    string name;
    time_t duedate;

    Hw();

    Hw(ICourseConfig *cc);

    void parse(const json& j);

};


struct Homeworks : public map<string, Hw> {

    ICourseConfig *config;

    Homeworks();

    Homeworks(ICourseConfig *cc);

    void parse(const json& j);

};


void from_json(const json& j, Hw& hw);

void from_json(const json& j, Homeworks& homeworks);

void to_json(json& j, const Hw& hw);

void to_json(json& j, const Homeworks& homeworks);


struct LabSesh {

    ICourseConfig *config;
    string id;
    int weekday;
    time_t start;
    time_t end;

    LabSesh();

    LabSesh(ICourseConfig *cc, const string& i);

    void parse(const json& j);

    bool is_active() const;

    tuple<string,string,string> get_pretty() const;

};


struct LabSessions : public map<string, LabSesh> {

    ICourseConfig *config;
    vector<string> all_ids;

    LabSessions();

    LabSessions(ICourseConfig *cc);

    void parse(const json& j);

};


void from_json(const json& j, LabSesh& ls);

void from_json(const json& j, LabSessions& lab_sessions);

void to_json(json& j, const LabSesh& ls);

void to_json(json& j, const LabSessions& lab_sessions);


struct Roster : public map<string, vector<string>> {

    ICourseConfig *config;

    Roster();

    Roster(ICourseConfig *cc);

    void parse(const json& j);

};


void from_json(const json& j, Roster& roster);

void to_json(json& j, const Roster& roster);


struct ICourseConfig {

    string filename;
    string course_id;
    string admin_hash;
    Homeworks homeworks;
    LabSessions lab_sessions;
    Roster roster;

    ICourseConfig();

    string error_msg(const string& msg, const string& a, const string& b);

    void require_prop(
        const json& j,
        const string& key,
        const json::value_type& type
    );

    void parse(const json& j);

};


struct CourseConfig : public ICourseConfig {

    CourseConfig();

    CourseConfig(const string& filename);

};


struct MockCourseConfig : public ICourseConfig {

    MockCourseConfig(const json& j);

};


void from_json(const json& j, ICourseConfig& cc);

void to_json(json& j, const ICourseConfig& cc);


#endif
