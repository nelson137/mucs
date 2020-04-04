#ifndef MUCS_HPP
#define MUCS_HPP


#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "picosha2.h"

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

    enum DumpFlags {
        DumpHomeworks = 1 << 0,
        DumpLabs      = 1 << 1,
        DumpRoster    = 1 << 2,
        DumpCurrents  = 1 << 3,
    };

    int dump_flags = 0;

    // Subcommand invocation

    void invoke(void (Mucs::*subcmd)());

    function<void()> get_invoke(void (Mucs::*subcmd)());

    // Util

    virtual bool prompt_yesno(const string& prompt);

    // Submit subcommand

    virtual bool try_compile_sources();

    virtual void submit_summary(const LabSesh& lab, const string& assignment);

    void submit();

    // Admin subcommand

    virtual void admin_authenticate();

    void admin_dump();

};


#endif
