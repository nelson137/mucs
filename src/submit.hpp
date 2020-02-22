#ifndef SUBMIT_HPP
#define SUBMIT_HPP


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


struct SubmitOptions {

    string course;
    string assignment_type;
    vector<string> sources;

};


void submit_summary(
    const string& course,
    const string& lab,
    const string& assignment,
    const string& user,
    const vector<string>& sources
);

void submit(SubmitOptions& opts);


#endif
