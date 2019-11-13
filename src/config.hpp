#ifndef CONFIG_HPP
#define CONFIG_HPP


#include <cctype>
#include <chrono>
#include <ctime>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "json.hpp"

#include "mucs/except.hpp"
#include "mucs/random.hpp"

#include "path.hpp"
#include "util.hpp"

using namespace std;
using namespace chrono;
using json = nlohmann::json;


struct Hw {

    string filename;
    string name;
    system_clock::time_point duedate;

    struct compare {
        bool operator()(
            const pair<string,Hw>& a,
            const pair<string,Hw>& b
        ) const;
    };

    Hw();

    Hw(const string& fn, const string& n);

    static Hw from_iter(const string& fn, const json::const_iterator& it);

};


struct Homeworks : public set<pair<string,Hw>,Hw::compare> {

    string filename;

    Homeworks(map<string,Hw> m = {});

    Homeworks(initializer_list<pair<string,Hw>> il);

    Homeworks(const string& fn);

};


void from_json(const json& j, Hw& hw);

void from_json(const json& j, Homeworks& homeworks);

void to_json(json& j, const Hw& hw);

void to_json(json& j, const Homeworks& homeworks);


struct LabSesh {

    string filename;
    string id;
    int weekday;
    time_t start;
    time_t end;

    LabSesh();

    LabSesh(const string& fn, const string& i);

    static LabSesh from_iter(const string& fn, const json::const_iterator& it);

    bool is_active() const;

    string w_raw() const;
    string w_pretty() const;

    string s_raw() const;
    string s_pretty() const;

    string e_raw() const;
    string e_pretty() const;

};


struct LabSessions : public map<string, LabSesh> {

    string filename;
    vector<string> all_ids;

    LabSessions();

    LabSessions(const string& fn);

};


void from_json(const json& j, LabSesh& ls);

void from_json(const json& j, LabSessions& lab_sessions);

void to_json(json& j, const LabSesh& ls);

void to_json(json& j, const LabSessions& lab_sessions);


struct Roster : public map<string, vector<string>> {

    string filename;
    vector<string> all_lab_ids;

    Roster();

    Roster(const string& fn, const vector<string>& lab_ids);

};


void from_json(const json& j, Roster& roster);

void to_json(json& j, const Roster& roster);


struct ICourseConfig {

    string filename;
    string course_id;
    string admin_hash;
    Homeworks homeworks;
    LabSessions lab_sessions;
    string current_lab;
    Roster roster;

    ICourseConfig();

    string error_msg(const string& msg, const string& a, const string& b);

    void require_prop(
        const json& j,
        const string& key,
        const json::value_type& type
    );

    void parse(const json& j);

    string get_current_lab(const string& user);

    string get_current_hw() const;

};


struct CourseConfig : public ICourseConfig {

    CourseConfig();
    CourseConfig(const Path& p);

};


struct MockCourseConfig : public ICourseConfig {

    MockCourseConfig(const json& j);

};


void from_json(const json& j, ICourseConfig& cc);

void to_json(json& j, const ICourseConfig& cc);


#endif
