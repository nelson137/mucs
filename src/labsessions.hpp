#ifndef LAB_SESSIONS_HPP
#define LAB_SESSIONS_HPP


#include <string>

#include "json.hpp"

#include "courseconfig.hpp"
#include "exc.hpp"

using namespace std;
using json = nlohmann::json;


class LabSessions : public json {

private:
    string filename;
    string key = "labs";
    json::value_type j_type = json::value_t::object;

public:
    LabSessions(CourseConfig& config);

    string parse_path(const string&  child_key) const;
    string parse_path(const string&& child_key) const;
    void parse(json& lab_sessions);

};


#endif
