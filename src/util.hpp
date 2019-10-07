#ifndef UTIL_HPP
#define UTIL_HPP


#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include <dirent.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "exc.hpp"
#include "execargs.hpp"

using namespace std;


void die(string);

vector<string> list_dir(string path);

bool path_is_valid(string&);

/**
 * Return specific information about path.
 */
bool path_exists(string path);
bool path_is_dir(string path);
bool path_is_file(string path);

tuple<string, string> path_split_ext(string path);

void rmdir(string&);

void verify_dir_exists(string&);


template<typename... T>
string join_paths(string a, string b, T... rest) {
    vector<string> components = { b, rest... };
    string p = a;
    for (auto c : components)
        p += "/" + c;
    return p;
}


template<typename... T>
void make_path(string base, T... comps) {
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


template<typename... T>
void verify_paths(string comp1, T... ts) {
    vector<string> paths = { comp1, ts... };
    for (auto p : paths) {
        if (not path_is_valid(p))
            die(ERR_INVALID_PARAM + p);
    }
}


#endif
