#ifndef ROSTER_HPP
#define ROSTER_HPP


#include <algorithm>
#include <string>
#include <vector>

#include "json.hpp"

#include "mucs/except.hpp"

#include "courseconfig.hpp"
#include "util.hpp"

using namespace std;
using json = nlohmann::json;


class Roster : public json {

private:
    string filename;
    string key = "roster";
    vector<string> lab_ids;

public:
    Roster();
    Roster(ICourseConfig& config, vector<string> ids);

    string parse_path(const string& child_key) const;

    void parse(ICourseConfig& config);

    bool is_valid_lab_id(const string& id);

};


#endif
