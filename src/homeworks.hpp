#ifndef HOMEWORKS_HPP
#define HOMEWORKS_HPP


#include <cctype>
#include <chrono>
#include <initializer_list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>

#include "json.hpp"

#include "util.hpp"

#include "mucs/except.hpp"

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

    Hw(const string& fn, const string& n);

    static Hw from_iter(const string& fn, const json::const_iterator& it);

};


struct Homeworks : public set<pair<string,Hw>, Hw::compare> {

    string filename;

    using set<pair<string,Hw>,Hw::compare>::set;

    Homeworks(const string& fn);

};


void from_json(const json& j, Hw& hw);

void from_json(const json& j, Homeworks& homeworks);

void to_json(json& j, const Hw& hw);

void to_json(json& j, const Homeworks& homeworks);


#endif
