#ifndef ROSTER_HPP
#define ROSTER_HPP


#include <string>

#include "json.hpp"

#include "courseconfig.hpp"
#include "exc.hpp"

using namespace std;
using json = nlohmann::json;


class Roster : public json {

private:
    string filename;
    string key = "roster";
    json::value_type j_type = json::value_t::object;

public:
    Roster(CourseConfig& config);

    string parse_path(const string&  child_key) const;
    string parse_path(const string&& child_key) const;
    void parse(json& roster);

};


#endif
