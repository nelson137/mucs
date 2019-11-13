#ifndef UTIL_HPP
#define UTIL_HPP


#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
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

string format_time(time_t tt, const string& fmt);

string format_weekday(int weekday);

int get_term_width();

string get_user();

system_clock::time_point parse_datetime(const string& dt_str);

time_t parse_time(const string& t_str);

int parse_weekday(string w_str);

tuple<string, string> path_split_ext(const string& path);

string prompt_user(const string& prompt = "");

vector<string> string_split(const string& s, const string& delim = ",");

void verify_dir_exists(const string& dirpath);


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
string join_paths(string a, string b = "", String... rest) {
    if (not a.size()) return b;
    if (not b.size()) return a;

    if (a.back() == '/' && b.front() == '/')
        a.erase(a.end() - 1);
    else if (a.back() != '/' && b.front() != '/')
        a += "/";

    return a + join_paths(b, rest...);
}


template<int n = 2, char c = '0'>
inline ostream& prefix_char(ostream& os) {
    return os << setw(n) << setfill(c);
}


template<int n = 2>
inline ostream& prefix_zeros(ostream& os) {
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
    for (auto i=c.begin()+1; i!=c.end(); i++)
        ss << delim << *i;
    return ss.str();
}


template<typename Container, typename UnaryOp>
inline void stl_transform(Container& c, UnaryOp op) {
    transform(c.begin(), c.end(), c.begin(), op);
}


template<template<typename> typename Container, typename T, typename UnaryOp>
inline void stl_transform(Container<T>& c, UnaryOp op) {
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
