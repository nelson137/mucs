#ifndef CONFIG_HPP
#define CONFIG_HPP


#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <fstream>
#include <functional>
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

#include "../consts.hpp"
#include "../except.hpp"
#include "../path.hpp"
#include "../util.hpp"

using namespace std;
using namespace chrono;
using namespace date;
using namespace placeholders;
using namespace valijson;
using namespace valijson::adapters;
using json = nlohmann::json;


struct IAssignment {

    string name;

    virtual bool is_active() const = 0;

};


/*************************************************
 * Homeworks
 ************************************************/


struct Hw : public IAssignment {

    sys_seconds duedate;

    Hw();
    Hw(string n);
    Hw(string n, sys_seconds dd);

    struct compare {
        bool operator()(const Hw& a, const Hw& b) const;
    };

    bool is_active() const override;

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
    LabSesh(string i);
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


struct LabAsgmt : public IAssignment {

    year_month_day start;
    year_month_day end;

    LabAsgmt();
    LabAsgmt(string n);
    LabAsgmt(string n, year_month_day s, year_month_day e);

    struct compare {
        bool operator()(const LabAsgmt& a, const LabAsgmt& b) const;
    };

    string week_str() const;

    bool is_active() const override;

    friend void from_json(const json& j, LabAsgmt& la);
    friend void to_json(json& j, const LabAsgmt& la);

};


struct LabAssignments
        : public set<LabAsgmt, LabAsgmt::compare>, public Tabular {

    using set<LabAsgmt, LabAsgmt::compare>::set;

    list<vector<string>> to_table() const override;

    friend void from_json(const json& j, LabAssignments& lab_assignments);
    friend void to_json(json& j, const LabAssignments& lab_assignments);

};


/*************************************************
 * Roster
 ************************************************/


struct Roster : public map<string, vector<string>>, public Tabular {

    using map<string, vector<string>>::map;

    void insert(string pawprint, string lab_id);

    const vector<string>& safe_get(const string& user) const;

    list<vector<string>> to_table() const override;

};


/*************************************************
 * Config
 ************************************************/


struct Config {

    json j_root;

    string filename;

    string course_id;

    string admin_hash;

    LabSessions lab_sessions;
    LabAssignments lab_assignments;

    Homeworks homeworks;

    Roster roster;

    Config();
    Config(const Path& config_p);
    Config(json root, string fn = "");

    Config& validate(const Path& schema_p = Path(SCHEMA_PATH));
    Config& deserialize();

    void load_roster(const Path& roster_d);

    static mucs_exception error(
        const string& msg,
        const initializer_list<string>& keys = {}
    );

    const IAssignment& validate_and_get_asgmt(const string& name) const;

    vector<LabSesh> get_student_labs(const string& user) const;
    LabSesh validate_and_get_lab(const string& user) const;

    friend void from_json(const json& j, Config& c);
    friend void to_json(json& j, const Config& c);

};


#endif
