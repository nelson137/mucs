#ifndef COURSE_CONFIG_HPP
#define COURSE_CONFIG_HPP


#include <fstream>
#include <initializer_list>
#include <map>
#include <string>

#include "json.hpp"

#include "mucs/except.hpp"
#include "mucs/random.hpp"

#include "util.hpp"

#define CONFIG_DIR "/group/cs1050/config.d"

using namespace std;
using json = nlohmann::json;


class ICourseConfig {

public:
    string filename;
    string course_id;
    string admin_hash;
    map<string,string> homeworks;
    map<string,string> lab_sessions;
    map<string,vector<string>> roster;

    ICourseConfig();

    void parse(const json& j);

};


class CourseConfig : public ICourseConfig {

public:
    CourseConfig();

    CourseConfig(const string& filename);

};


class MockCourseConfig : public ICourseConfig {

public:
    MockCourseConfig(const json& j);

};


void from_json(const json& j, CourseConfig& cc);

void to_json(json& j, const CourseConfig& cc);


#endif
