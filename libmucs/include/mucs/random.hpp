#ifndef MUCS_RANDOM_HPP
#define MUCS_RANDOM_HPP


#include <ostream>
#include <random>
#include <string>

using namespace std;


class chars_t {

private:
    string _choices;

public:
    chars_t(string cs) : _choices(cs) {}

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
 * Return a random integer on the interval [start=0,end).
 */
int rand_int(int start, int end);
int rand_int(int end);


/**
 * Return a random character from chars.
 */
char rand_char(chars_t chars = chars_alphanum);


/**
 * Return a random string of length size from chars.
 */
string rand_string(int size = 10, chars_t chars = chars_alphanum);


#endif
