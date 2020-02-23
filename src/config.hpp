#ifndef CONFIG_HPP
#define CONFIG_HPP


#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "json.hpp"

#include "mucs/except.hpp"

#include "path.hpp"
#include "util.hpp"

using namespace std;
using namespace chrono;
using json = nlohmann::json;


/*************************************************
 * Homeworks
 ************************************************/


struct Hw {

    string name;
    system_clock::time_point duedate;

    struct compare {
        bool operator()(
            const pair<string,Hw>& a,
            const pair<string,Hw>& b
        ) const;
    };

};


struct Homeworks : public set<pair<string,Hw>, Hw::compare> {

    using set<pair<string,Hw>, Hw::compare>::set;

};


void from_json(const json& j, Hw& hw);

void from_json(const json& j, Homeworks& homeworks);

void to_json(json& j, const Hw& hw);

void to_json(json& j, const Homeworks& homeworks);


/*************************************************
 * LabSessions
 ************************************************/


struct LabSesh {

    string id;
    int weekday;
    system_clock::time_point start;
    system_clock::time_point end;

    LabSesh();
    LabSesh(const string& i);

    operator string() const;
    friend ostream& operator<<(ostream& os, const LabSesh& ls);

    bool is_active() const;

    string format(string fmt) const;

};


struct LabSessions : public map<string, LabSesh> {

    using map<string, LabSesh>::map;

};


void from_json(const json& j, LabSesh& ls);

void from_json(const json& j, LabSessions& lab_sessions);

void to_json(json& j, const LabSesh& ls);

void to_json(json& j, const LabSessions& lab_sessions);


/*************************************************
 * Roster
 ************************************************/


struct Roster : public map<string, vector<string>> {

    using map<string, vector<string>>::map;

};


void from_json(const json& j, Roster& roster);

void to_json(json& j, const Roster& roster);


/*************************************************
 * Config
 ************************************************/


class Config {

private:
    Config();

public:
    string filename;

    string course_id;

    string admin_hash;

    LabSessions lab_sessions;
    vector<string> lab_ids;
    string current_lab;

    Homeworks homeworks;

    Roster roster;

    static Config& get();

    template<typename Dest>
    static void get_to_required(
        const json& parent,
        const string& key,
        const string& type,
        Dest& dest
    ) {
        if (parent.count(key) == 0 || parent[key].type_name() != type)
            throw mucs_exception::config(
                "Config requires key \"" + key + "\" with type " + type,
                get().filename);
        parent[key].get_to(dest);
    }

    // Make sure no copies can be made
    Config(const Config&) = delete;
    void operator=(const Config&) = delete;

    Config& parse(const json& root);
    Config& parse_file(const Path& p);

    string get_assignment(const string& type) const;
    string get_current_lab() const;
    string get_current_hw() const;

    vector<LabSesh> get_user_labs(const string& user) const;
    LabSesh get_lab(const string& id) const;

};


void from_json(const json& j, Config& c);

void to_json(json& j, const Config& c);


#endif
