#ifndef CONFIG_HPP
#define CONFIG_HPP


#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <fstream>
#include <initializer_list>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "json.hpp"

#include "mucs/except.hpp"

#include "consts.hpp"
#include "path.hpp"
#include "util.hpp"

using namespace chrono;
using namespace std;
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

    friend void from_json(const json& j, Hw& hw);
    friend void to_json(json& j, const Hw& hw);

};


struct Homeworks : public set<pair<string,Hw>, Hw::compare>, public Tabular {

    using set<pair<string,Hw>, Hw::compare>::set;

    list<vector<string>> to_table() const override;

    friend void from_json(const json& j, Homeworks& homeworks);
    friend void to_json(json& j, const Homeworks& homeworks);

};


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

    friend void from_json(const json& j, LabSesh& ls);
    friend void to_json(json& j, const LabSesh& ls);

};


struct LabSessions : public map<string, LabSesh>, public Tabular {

    using map<string, LabSesh>::map;

    list<vector<string>> to_table() const override;

    friend void from_json(const json& j, LabSessions& lab_sessions);
    friend void to_json(json& j, const LabSessions& lab_sessions);

};


/*************************************************
 * Roster
 ************************************************/


struct Roster : public map<string, vector<string>>, public Tabular {

    using map<string, vector<string>>::map;

    list<vector<string>> to_table() const override;

    friend void from_json(const json& j, Roster& roster);
    friend void to_json(json& j, const Roster& roster);

};


/*************************************************
 * Config
 ************************************************/


struct Config {

    string filename;

    string course_id;

    string admin_hash;

    LabSessions lab_sessions;
    string current_lab;

    Homeworks homeworks;

    Roster roster;

    static Config parse(const json& root);
    static Config parse_file(const IPath& p);

    static mucs_exception error(
        const string& msg,
        const initializer_list<string>& keys = {}
    );

    template<typename Dest>
    static void get_to_required(
        const json& parent,
        const string& key,
        const string& type,
        Dest& dest
    ) {
        if (parent.count(key) == 0 || parent[key].type_name() != type)
            throw Config::error(
                "Config requires key \"" + key + "\" with type " + type);
        parent[key].get_to(dest);
    }

    string get_assignment(const string& type) const;
    string get_current_lab() const;
    string get_current_hw() const;

    vector<LabSesh> get_user_labs(const string& user) const;
    LabSesh get_lab(const string& id) const;

    friend void from_json(const json& j, Config& c);
    friend void to_json(json& j, const Config& c);

};


#endif
