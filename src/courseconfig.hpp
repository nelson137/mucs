#ifndef COURSE_CONFIG_HPP
#define COURSE_CONFIG_HPP


#include <string>

#include "json.hpp"

class CourseConfig;
#include "exc.hpp"
#include "homeworks.hpp"
#include "labsessions.hpp"
#include "roster.hpp"

using namespace std;
using json = nlohmann::json;


class CourseConfig : public json {

public:
    string filename;

    CourseConfig(string& filename, json& data);

    void require_prop(string&  key, json::value_type type) const;
    void require_prop(string&& key, json::value_type type) const;

};


#endif
