#ifndef UTIL_HPP
#define UTIL_HPP


#include <algorithm>
#include <chrono>
#include <ctime>
#include <deque>
#include <iomanip>
#include <ios>
#include <iostream>
#include <list>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <pwd.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "json.hpp"

#include "mucs/except.hpp"

#include "consts.hpp"
#include "proc.hpp"

using namespace chrono;
using namespace std;
using json = nlohmann::json;


extern system_clock::time_point NOW;


struct Tabular {

    virtual list<vector<string>> to_table() const = 0;

};


system_clock::time_point current_time();

int current_weekday();

string format_datetime(const tm& t, const string& fmt);

string format_datetime(const system_clock::time_point& tp, const string& fmt);

string format_weekday(int weekday);

int get_term_width();

string get_user();

string join_paths(string a, deque<string> parts);

system_clock::time_point parse_datetime(const string& dt_str);

system_clock::time_point parse_time(const string& t_str);

int parse_weekday(const string& w_str);

void print_table(const list<vector<string>>& table, const string& delim = "  ");

vector<string> string_split(const string& s, const string& delim);

string string_strip(string s);

tm tm_zero();


template<typename... String>
string join_paths(string a, string b, String... rest) {
    deque<string> parts = {b, rest...};
    return join_paths(a, parts);
}


template<int n = 2, char c = '0'>
ostream& prefix_char(ostream& os) {
    return os << setw(n) << setfill(c);
}


template<int n = 2>
ostream& prefix_zeros(ostream& os) {
    return os << prefix_char<n,'0'>;
}


template<typename Container, typename T>
bool stl_contains(const Container& c, const T& val) {
    return ::find(c.begin(), c.end(), val) != c.end();
}


template<
    typename Container,
    typename ContainerIt = typename Container::const_iterator,
    typename UnaryOp
>
ContainerIt stl_find_if(const Container& container, const UnaryOp& op) {
    return find_if(container.begin(), container.end(), op);
}


template<typename Container>
string stl_join(const Container& c, const string& delim = ",") {
    if (c.size() == 0)
        return "";
    return accumulate(
        c.cbegin() + 1, c.cend(),
        c.front(),
        [&] (const string& a, const string& b) -> string {
            return a + delim + b;
        }
    );
}


template<typename Container, typename UnaryOp>
void stl_transform(Container& c, UnaryOp op) {
    transform(c.begin(), c.end(), c.begin(), op);
}


string w_bold (const string& s);
string w_green(const string& s);
string w_red  (const string& s);


#endif
