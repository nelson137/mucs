#ifndef COURSE_CONFIG_HPP
#define COURSE_CONFIG_HPP


#include <fstream>
#include <string>

#include "json.hpp"

class ICourseConfig;
#include "homeworks.hpp"
#include "labsessions.hpp"
#include "roster.hpp"

#include "mucs/except.hpp"
#include "mucs/random.hpp"

using namespace std;
using json = nlohmann::json;


class ICourseConfig : public json {

public:
    string filename;

    ICourseConfig(const json& data);

    virtual void require_prop(
        const string& key,
        const json::value_type& type
    ) const = 0;

};


class CourseConfig : public ICourseConfig {

public:
    CourseConfig(const string& filename, const json& data);

    void require_prop(const string& key, const json::value_type& type) const;

};


class MockCourseConfig : public ICourseConfig {

public:
    MockCourseConfig(const initializer_list_t& j);

    void require_prop(const string& key, const json::value_type& type) const;

};


#endif
