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


/**
 * Interface for all assignment types
 */
struct IAssignment {

    string name;

    /**
     * Return whether the assignment can be turned in.
     * In most cases, this is when the assignment's due date is in the future
     * or the current date and time is within the due date "window".
     */
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

    /**
     * Return whether this lab is ongoing.
     * This is true if today is the same weekday as wd and the current time is
     * between start and end.
     */
    bool is_active() const;

    /**
     * Return a string where template strings are replaced with formatted
     * member fields.
     *
     * The supported template strings are as follows:
     *   {id}         The id.
     *   {weekday}    The weekday string (Sunday, Monday, ...).
     *   {weekday_n}  The weekday integer (Sunday=0, Monday=1, ...).
     *   {start}      The start time (24 hour clock).
     *   {start_p}    The start time (12 hour clock).
     *   {end}        The end time (24 hour clock).
     *   {end_p}      The end time (12 hour clock).
     */
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

    /**
     * Stringify this object as it would be used in the JSON config.
     * Format: YEAR MONTH_ABREV WEEK_INDEX
     *
     * The first week of the month (i.e. the week of the first Monday of the
     * month) is index 1. Index 0 should not be used. For example, the first
     * week of January 2020 (2020-1-6 to 2020-1-12) would be "2020 Jan 1"
     */
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


struct Roster : public map<string, string>, public Tabular {

    using map<string, string>::map;

    /**
     * Insert a new record into the roster object.
     * Throw if a record already exists for user.
     */
    void safe_insert(string user, string lab_id);

    /**
     * Return the lab id that is associated with user.
     * Throw if user is not found.
     */
    const string& safe_get(const string& user) const;

    list<vector<string>> to_table() const override;

};


/*************************************************
 * Config
 ************************************************/


struct Config {

    // The serialized JSON of the parsed config file.
    json j_root;

    string filename;

    string course_id;

    string admin_hash;

    LabSessions lab_sessions;
    LabAssignments lab_assignments;

    Homeworks homeworks;

    Roster roster;

    /**
     * All constructors with arguments do the minimum amount of work to populate
     * j_root, maybe initialize filename, and nothing else. Validation and
     * deeper deserialization is done later in methods.
     */
    Config();
    Config(const Path& config_p);
    Config(json root, string fn = "");

    /**
     * Validate j_root against a given schema file.
     */
    Config& validate(const Path& schema_p = Path(SCHEMA_PATH));

    /**
     * Deserialize the child nodes of j_root into their data members.
     */
    Config& deserialize();

    /**
     * Populate the roster data member from the files in the given directory.
     */
    void load_roster(const Path& roster_d);

    /**
     * Return an error message of the config filename and path to the invalid
     * node.
     *
     * The filename will be a template string that gets replaced with the real
     * value in main().
     */
    static mucs_exception error(
        const string& msg,
        const initializer_list<string>& keys = {}
    );

    /**
     * Return an assignment whose name equals the given name.
     * Throw if none is found.
     */
    const IAssignment& validate_and_get_asgmt(const string& name) const;

    /**
     * Return the active LabSesh that is associated with user in roster.
     * Throw if user isn't in roster.
     * Throw if the LabSesh isn't active.
     */
    const LabSesh& validate_and_get_lab(const string& user) const;

    friend void from_json(const json& j, Config& c);
    friend void to_json(json& j, const Config& c);

};


#endif
