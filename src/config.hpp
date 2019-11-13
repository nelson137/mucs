#ifndef CONFIG_HPP
#define CONFIG_HPP


#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>

#include "json.hpp"

#include "mucs/except.hpp"
#include "mucs/random.hpp"

#include "homeworks.hpp"
#include "labs.hpp"
#include "path.hpp"
#include "roster.hpp"
#include "util.hpp"

using namespace std;
using namespace chrono;
using json = nlohmann::json;


struct IConfig {

    string filename;
    string course_id;
    string admin_hash;
    Homeworks homeworks;
    LabSessions lab_sessions;
    string current_lab;
    Roster roster;

    IConfig();

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


struct Config : public IConfig {

    Config();
    Config(const Path& p);

};


struct MockConfig : public IConfig {

    MockConfig(const json& j);

};


void from_json(const json& j, IConfig& cc);

void to_json(json& j, const IConfig& cc);


#endif
