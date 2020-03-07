#ifndef MUCS_HPP
#define MUCS_HPP


#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "mucs/except.hpp"

#include "config.hpp"
#include "consts.hpp"
#include "exec.hpp"
#include "path.hpp"
#include "util.hpp"

using namespace std;
using namespace chrono;


struct Mucs {

    string course;
    string assignment_type;
    vector<string> sources;

    Config config;

    void init();

    void submit();

};


void submit_summary(
    const string& course,
    const LabSesh& lab,
    const string& assignment,
    const string& user,
    const vector<string>& sources
);


#endif
