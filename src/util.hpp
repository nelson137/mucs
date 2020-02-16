#ifndef UTIL_HPP
#define UTIL_HPP


#include <chrono>
#include <ctime>
#include <deque>
#include <iomanip>
#include <iostream>
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
#include "exec.hpp"

using namespace std;
using namespace chrono;
using json = nlohmann::json;


time_t current_time();

int current_weekday();

string format_datetime(const tm& t, const string& fmt);

string format_datetime(time_t tt, const string& fmt);

string format_weekday(int weekday);

int get_term_width();

string get_user();

string join_paths(string a, deque<string> parts);

system_clock::time_point parse_datetime(const string& dt_str);

time_t parse_time(const string& t_str);

int parse_weekday(const string& w_str);

bool prompt_yesno(const string& prompt = "");

vector<string> string_split(const string& s, const string& delim);

string string_strip(string s);


template<typename... String>
string error_config(
    const string& msg,
    const string& filename,
    const String&... pack_keys
) {
    vector<string> keys = { pack_keys... };
    ostringstream ret;
    ret << msg << ": " << filename;
    for (const auto& k : keys)
        ret << "[\"" << k << "\"]";
    return ret.str();
}


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


template<typename Container>
void stl_extend(Container& a, const Container& b) {
    a.insert(a.end(), b.begin(), b.end());
}


template<typename Container>
string stl_join(const Container& c, const string& delim = ",") {
    ostringstream ss;
    if (c.size())
        ss << c[0];
    if (c.size() > 1)
        for (auto i=c.begin()+1; i!=c.end(); i++)
            ss << delim << *i;
    return ss.str();
}


template<typename Container, typename UnaryOp>
void stl_transform(Container& c, UnaryOp op) {
    transform(c.begin(), c.end(), c.begin(), op);
}


string string_wrap(
    string s,
    const string& prefix = FG_RESET,
    const string& suffix = FG_RESET
);

string w_bold (string s);
string w_green(string s);
string w_red  (string s);


#endif
