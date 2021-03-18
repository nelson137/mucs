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


/**
 * Character class type.
 *
 * chars_t objects can be combined with operator|.
 */
class chars_t {

private:
    string _choices;

public:
    chars_t(const string& cs);

    /**
     * Return a string of all characters in this character class.
     */
    string get() const;

    /**
     * Return the number of characters in this character class.
     */
    size_t size() const;

    /**
     * Return the character at index i of the characters in this character
     * class.
     */
    char operator[](const int i) const;

    /**
     * Return a new chars_t that is the concatenation of a and b.
     */
    friend chars_t operator|(const chars_t& a, const chars_t& b);

    /**
     * Print all of the characters in this character class.
     */
    friend ostream& operator<<(ostream& os, const chars_t& cc);

};

/**
 * Define some useful character classes.
 */
static chars_t chars_whitespace(" \t\n");
static chars_t chars_lower("abcdefghijklmnopqrstuvwxyz");
static chars_t chars_upper("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
static chars_t chars_nums("0123456789");
static chars_t chars_alphanum = chars_nums | chars_upper | chars_lower;
static chars_t chars_hex = chars_t("abcdef") | chars_nums;


/**
 * Return a random character from character class chars.
 */
char rand_char(const chars_t& chars = chars_alphanum);


/**
 * Return a random integer on the interval [start=0,end).
 */
int rand_int(const int start, const int end);
int rand_int(const int end);


/**
 * Return a random string of length size from character class chars.
 */
string rand_string(const int size = 10, const chars_t& chars = chars_alphanum);


/**
 * Return a json object containing random test data.
 */
json new_config_data(json j = json::object());

/**
 * Return a random course name.
 */
string rand_course();

/**
 * Return a random filename.
 */
string rand_filename();

/**
 * Return a random homework assignment name.
 */
string rand_hw_name();

/**
 * Return a random lab assignment name.
 */
string rand_lab_asgmt_name();

/**
 * Return a random json object that can be deserialized to a LabSesh.
 */
json rand_lab_sesh_data(json j = json::object());

/**
 * Return a random lab session id.
 */
string rand_lab_sesh_id();

/**
 * Return a string of random digits that is n characters long.
 */
string rand_string_digits(int n);

/**
 * Return a random username.
 */
string rand_user();


class RandLabSesh {

private:
    LabSesh ls;

public:
    RandLabSesh();
    RandLabSesh(const string& id);

    /**
     * Set whether the LabSesh is active for the current weekday.
     */
    RandLabSesh& today(bool b = true);

    /**
     * Set whether the LabSesh is active for the current time.
     */
    RandLabSesh& now(bool b = true);

    /**
     * Return the built LabSesh object.
     */
    const LabSesh& get() const;

};


class RandLabAsgmt {

private:
    string name;

    year_month_weekday ymwd;

public:
    RandLabAsgmt(const string& n = rand_lab_asgmt_name());

    /**
     * Set whether the LabAsgmt is active for this week.
     */
    RandLabAsgmt& this_week(bool this_week = true);

    /**
     * Return the built LabAsgmt object.
     */
    LabAsgmt get() const;

};


#endif
