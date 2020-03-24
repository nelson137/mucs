#ifndef MUCS_HPP
#define MUCS_HPP


#include <iostream>
#include <string>
#include <vector>

#include "mucs/except.hpp"

#include "config.hpp"
#include "consts.hpp"
#include "path.hpp"
#include "proc.hpp"
#include "util.hpp"

using namespace std;


struct Mucs {

    string course;
    string assignment_type;
    vector<string> sources;

    function<void()> invoke(void (Mucs::*subcmd)(const Config&));

    void submit(const Config& config);

};


#endif
