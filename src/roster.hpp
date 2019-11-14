#ifndef ROSTER_HPP
#define ROSTER_HPP


#include <cctype>
#include <string>
#include <map>
#include <vector>

#include "json.hpp"

#include "mucs/except.hpp"

#include "util.hpp"

using namespace std;
using json = nlohmann::json;


struct Roster : public map<string, vector<string>> {

    string filename;
    vector<string> all_lab_ids;

    using map<string,vector<string>>::map;

    Roster(const string& fn, const vector<string>& lab_ids);

};


void from_json(const json& j, Roster& roster);

void to_json(json& j, const Roster& roster);


#endif
