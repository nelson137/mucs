#ifndef MUCS_HPP
#define MUCS_HPP


#include <algorithm>
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

    Config config;
    string user;

    // From cli

    string course;
    string assignment_type;
    vector<Path> sources;

    // Subcommand invocation

    void invoke(void (Mucs::*subcmd)());

    function<void()> get_invoke(void (Mucs::*subcmd)());

    // Util

    virtual bool prompt_yesno(const string& prompt);

    // Submit subcommand

    virtual bool try_compile_sources();

    virtual void submit_summary(const LabSesh& lab, const string& assignment);

    void submit();

};


#endif
