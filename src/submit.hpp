#ifndef SUBMIT_HPP
#define SUBMIT_HPP


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


struct SubmitOptions {

    string course;
    string assignment_type;
    vector<string> sources;

};


struct Submitter {

    Path submit_d;

    Submitter(const Path& sub_d);

    bool submit(const vector<string>& sources);

};


void submit(SubmitOptions& opts);


#endif
