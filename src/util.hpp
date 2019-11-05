#ifndef UTIL_HPP
#define UTIL_HPP


#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include <dirent.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "json.hpp"

#include "mucs/except.hpp"

#include "execargs.hpp"

using namespace std;
using json = nlohmann::json;


void die(string msg);

string format_time(time_t t);

string format_weekday(int weekday);

vector<string> list_dir(const string& path);

time_t parse_time(const string& t_str);

int parse_weekday(string w_str);

bool path_is_valid(const string& path);

/**
 * Return specific information about path.
 */
bool path_exists(const string& path);
bool path_is_dir(const string& path);
bool path_is_file(const string& path);

tuple<string, string> path_split_ext(const string& path);

void rmdir(const string& path);

vector<string> string_split(const string& s, const string& delim = ",");

void verify_dir_exists(const string& dirpath);


template<typename... T>
string join_paths(const string& a, const string& b, T... rest) {
    vector<string> components = { b, rest... };
    string p = a;
    for (auto c : components)
        p += "/" + c;
    return p;
}


template<typename... T>
void make_path(const string& base, T... comps) {
    struct stat s;
    string path = base;
    vector<string> components = { comps... };

    for (auto c : components) {
        path = join_paths(path, c);
        if(stat(path.c_str(), &s) == 0) {   // If path exists
            if (S_ISDIR(s.st_mode) == 0)    //   If not a directory
                die(ERR_NOT_A_DIR + path);  //     Error
        } else {                            // If path doesn't exist
            if (mkdir(path.c_str(), 0770))  //   Attempt to make directory
                die(ERR_MKDIR + path);      //     Error if failed
        }
    }
}


template<typename Container, typename T>
bool stl_contains(const Container& c, const T& val) {
    return ::find(c.begin(), c.end(), val) != c.end();
}


template<typename Container, typename UnaryOp>
inline void stl_transform(Container& c, UnaryOp& op) {
    transform(c.begin(), c.end(), c.begin(), op);
}


template<typename... T>
void verify_paths(const string& comp1, T... ts) {
    vector<string> paths = { comp1, ts... };
    for (auto p : paths) {
        if (not path_is_valid(p))
            die(ERR_INVALID_PARAM + p);
    }
}


#endif
