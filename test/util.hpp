#ifndef T_UTIL_HPP
#define T_UTIL_HPP


#include <string>

#include "date.h"
#include "json.hpp"

#include "mucs/random.hpp"

#include "../src/config.hpp"

using namespace std;
using namespace date;
using json = nlohmann::json;


#define INVALID_VALUE_TYPE "Value type not permitted by 'type' constraint"
#define MISSING_PROPERTY   "Missing required property '"


json new_config_data(json j = json::object());

string rand_course();

string rand_filename();

string rand_hw_name();

string rand_lab_asgmt_name();

string rand_lab_sesh_id();

string rand_string_digits(int n);

string rand_user();


class RandLabSesh {

private:
    LabSesh ls;

public:
    RandLabSesh(const string& id = rand_lab_sesh_id());

    RandLabSesh& today(bool b = true);
    RandLabSesh& now(bool b = true);

    const LabSesh& get() const;

};


class RandLabAsgmt {

private:
    string name;

    year_month_weekday ymwd;

public:
    RandLabAsgmt(const string& n = rand_lab_asgmt_name());

    RandLabAsgmt& this_week(bool this_week = true);

    LabAsgmt get() const;

};


#endif
