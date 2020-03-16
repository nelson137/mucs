#ifndef MUCS_HPP
#define MUCS_HPP


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


struct Mucs {

    string course;
    string assignment_type;
    vector<string> sources;

    void submit();

};


void submit_summary(
    const string& course,
    const LabSesh& lab,
    const string& assignment,
    const string& user,
    const vector<string>& sources
);

function<void()> mucs_callback(Mucs& mucs, void(Mucs::*subcmd)());


#endif
