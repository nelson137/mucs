#ifndef LABS_HPP
#define LABS_HPP


#include <cctype>
#include <ctime>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include "json.hpp"

#include "mucs/except.hpp"

#include "util.hpp"

using namespace std;
using json = nlohmann::json;


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

    using map<string,LabSesh>::map;

    LabSessions(const string& fn);

};


void from_json(const json& j, LabSesh& ls);

void from_json(const json& j, LabSessions& lab_sessions);

void to_json(json& j, const LabSesh& ls);

void to_json(json& j, const LabSessions& lab_sessions);


#endif
