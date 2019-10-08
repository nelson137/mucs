#ifndef LAB_SESSIONS_HPP
#define LAB_SESSIONS_HPP


#include <string>

#include "json.hpp"

#include "mucs/except.hpp"

#include "courseconfig.hpp"

using namespace std;
using json = nlohmann::json;


class LabSessions : public json {

private:
    string filename;
    string key = "labs";

public:
    LabSessions();
    LabSessions(ICourseConfig& config);

    string parse_path(const string&  child_key) const;
    string parse_path(const string&& child_key) const;

    void parse(ICourseConfig& config);

};


#endif
