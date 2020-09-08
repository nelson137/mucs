#ifndef MUCS_HPP
#define MUCS_HPP


#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "CLI11.hpp"
#include "picosha2.h"
#include "rang.hpp"

#include "except.hpp"

#include "consts.hpp"
#include "models/config.hpp"
#include "path.hpp"
#include "proc.hpp"
#include "util.hpp"

using namespace std;


struct Mucs {

    Config config;
    string user;

    // From cli

    string course;
    string assignment;
    vector<Path> sources;

    enum DumpFlags {
        DumpLabSessions    = 1 << 0,
        DumpLabAssignments = 1 << 1,
        DumpHomeworks      = 1 << 2,
        DumpRoster         = 1 << 3,
    };

    int dump_flags = 0;

    // CLI : mucs-cli.cpp

    virtual void invoke(void (Mucs::*subcmd)());

    unique_ptr<CLI::App> get_cli();

    // Util : mucs.cpp

    virtual void admin_authenticate() const;

    virtual void dump_homeworks() const;

    virtual void dump_lab_assignments() const;

    virtual void dump_lab_sessions() const;

    virtual void dump_roster() const;

    virtual string get_user() const;

    virtual string prompt_password(const string& prompt) const;

    virtual bool prompt_yesno(const string& prompt) const;

    virtual void submit_summary(
        const LabSesh& lab,
        const string& assignment
    ) const;

    virtual bool try_compile_sources() const;

    virtual void update_config_admin_hash(const string& new_hash) const;

    // Submit Subcommand : mucs-submit.cpp

    virtual void submit();

    // Admin Subcommand : mucs-admin.cpp

    virtual void admin_dump();

    virtual void admin_update_password();

};


#endif
