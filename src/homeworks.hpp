#ifndef HOMEWORKS_HPP
#define HOMEWORKS_HPP


#include <string>

#include "json.hpp"

#include "courseconfig.hpp"
#include "exc.hpp"

using namespace std;
using json = nlohmann::json;


class Homeworks : public json {

private:
    string filename;
    string key = "homeworks";

public:
    Homeworks(CourseConfig& config);

    string parse_path(const string&  child_key) const;
    string parse_path(const string&& child_key) const;
    void parse(json& homeworks);

};


#endif
