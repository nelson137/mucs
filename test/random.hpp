#ifndef RANDOM_HPP
#define RANDOM_HPP


#include <ostream>
#include <random>
#include <string>

#include "date.h"
#include "json.hpp"

#include "../src/models/config.hpp"

using namespace std;
using namespace date;
using json = nlohmann::json;


#define INVALID_VALUE_TYPE "Value type not permitted by 'type' constraint"
#define MISSING_PROPERTY   "Missing required property '"


class chars_t {

private:
    string _choices;

public:
    chars_t(const string& cs);

    string get() const;

    size_t size() const;

    char operator[](const int index) const;

    friend chars_t operator|(const chars_t& a, const chars_t& b);

    friend ostream& operator<<(ostream& os, const chars_t& cc);

};

// Using the implemented constructor
static chars_t chars_lower("abcdefghijklmnopqrstuvwxyz");
static chars_t chars_upper("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
static chars_t chars_nums("0123456789");
static chars_t chars_hex_alpha("abcdef");
// Using the default copy constructor
static chars_t chars_alphanum(chars_nums | chars_upper | chars_lower);
static chars_t chars_hex(chars_nums | chars_hex_alpha);


/**
 * Return a random character from chars.
 */
char rand_char(const chars_t& chars = chars_alphanum);


/**
 * Return a random integer on the interval [start=0,end).
 */
int rand_int(const int start, const int end);
int rand_int(const int end);


/**
 * Return a random string of length size from chars.
 */
string rand_string(const int size = 10, const chars_t& chars = chars_alphanum);


/**
 * Generate random test data
 */

json new_config_data(json j = json::object());

string rand_course();

string rand_filename();

string rand_hw_name();

string rand_lab_asgmt_name();

json rand_lab_sesh_data(json j = json::object());

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
