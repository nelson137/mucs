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

    // Subcommand Invocation : mucs.cpp

    void invoke(void (Mucs::*subcmd)());

    function<void()> get_invoke(void (Mucs::*subcmd)());

    // Util : mucs.cpp

    virtual void admin_authenticate();

    virtual bool prompt_yesno(const string& prompt);

    virtual void submit_summary(const LabSesh& lab, const string& assignment);

    virtual bool try_compile_sources();

    virtual void update_config_admin_hash(const string& new_hash);

    // Submit Subcommand : mucs-submit.cpp

    void submit();

    // Admin Subcommand : mucs-admin.cpp

    void admin_dump();

    void admin_update_password();

};


#endif
