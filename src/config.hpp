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
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "date.h"
#include "json.hpp"
#include "valijson/adapters/nlohmann_json_adapter.hpp"
#include "valijson/schema.hpp"
#include "valijson/schema_parser.hpp"
#include "valijson/validator.hpp"

#include "mucs/except.hpp"

#include "consts.hpp"
#include "path.hpp"
#include "util.hpp"

using namespace std;
using namespace chrono;
using namespace date;
using namespace valijson;
using namespace valijson::adapters;
using json = nlohmann::json;


/*************************************************
 * Homeworks
 ************************************************/


struct Hw {

    string name;
    sys_seconds duedate;

    Hw();
    Hw(string n, sys_seconds dd);

    struct compare {
        bool operator()(const Hw& a, const Hw& b) const;
    };

    friend void from_json(const json& j, Hw& hw);
    friend void to_json(json& j, const Hw& hw);

};


struct Homeworks : public set<Hw, Hw::compare>, public Tabular {

    using set<Hw, Hw::compare>::set;

    list<vector<string>> to_table() const override;

    friend void from_json(const json& j, Homeworks& homeworks);
    friend void to_json(json& j, const Homeworks& homeworks);

};


/*************************************************
 * LabSessions
 ************************************************/


struct LabSesh {

    string id;
    weekday wd;
    seconds start;
    seconds end;

    LabSesh();
    LabSesh(const string& i);
    LabSesh(string id, weekday w, seconds s, seconds e);

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
 * LabAssignments
 ************************************************/


struct LabAsgmt {

    string name;
    year_month_day start;
    year_month_day end;

    LabAsgmt();
    LabAsgmt(const string& n);
    LabAsgmt(const string& n, year_month_day s, year_month_day e);

    struct compare {
        bool operator()(
            const pair<string,LabAsgmt>& a,
            const pair<string,LabAsgmt>& b
        ) const;
    };

    string str() const;

    friend void from_json(const json& j, LabAsgmt& la);
    friend void to_json(json& j, const LabAsgmt& la);

};


struct LabAssignments : public set<pair<string,LabAsgmt>, LabAsgmt::compare> {

    using set<pair<string,LabAsgmt>, LabAsgmt::compare>::set;

    friend void from_json(const json& j, LabAssignments& lab_assignments);
    friend void to_json(json& j, const LabAssignments& lab_assignments);

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
    LabAssignments lab_assignments;

    Homeworks homeworks;

    Roster roster;

    Config();
    Config(const IPath& config_p);

    Config& parse(const json& root, const string& filename = "");

    void validate_config(const json& root) const;

    static mucs_exception error(
        const string& msg,
        const initializer_list<string>& keys = {}
    );

    string get_assignment(const string& type) const;
    string get_current_lab() const;
    string get_current_hw() const;

    vector<LabSesh> get_user_labs(const string& user) const;
    LabSesh get_lab(const string& id) const;

    friend void from_json(const json& j, Config& c);
    friend void to_json(json& j, const Config& c);

};


#endif
