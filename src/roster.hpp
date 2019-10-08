#ifndef ROSTER_HPP
#define ROSTER_HPP


#include <string>

#include "json.hpp"

#include "mucs/except.hpp"

#include "courseconfig.hpp"

using namespace std;
using json = nlohmann::json;


class Roster : public json {

private:
    string filename;
    string key = "roster";

public:
    Roster();
    Roster(ICourseConfig& config);

    string parse_path(const string&  child_key) const;
    string parse_path(const string&& child_key) const;

    void parse(ICourseConfig& config);

};


#endif
