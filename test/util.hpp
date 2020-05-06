#ifndef T_UTIL_HPP
#define T_UTIL_HPP


#include <string>

#include "json.hpp"

#include "mucs/random.hpp"

#include "../src/config.hpp"

using namespace std;
using json = nlohmann::json;


int current_year();

string error_id_unrecognized(const string& user, const string& id);

string error_prop(const string& k, const string& t);

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

    static const vector<string> MONTHS;
    string month;

    int week_n;

public:
    RandLabAsgmt(const string& n = rand_lab_asgmt_name());

    RandLabAsgmt& not_this_week();
    RandLabAsgmt& this_week();

    LabAsgmt get() const;

};


#endif
