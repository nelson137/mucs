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

    virtual unique_ptr<CLI::App> get_cli();

    // Util : mucs.cpp

    virtual bool compile_sources() const;

    virtual string get_user() const;

    virtual string prompt_password(const string& prompt) const;

    virtual bool prompt_yesno(const string& prompt) const;

    virtual void submit_summary(
        const LabSesh& lab,
        const string& assignment
    ) const;

    virtual void update_config_admin_hash(const string& new_hash) const;

    // Submit Subcommand : mucs-submit.cpp

    virtual void submit();

    virtual void copy_submission_files(
        const Path& asgmt_d,
        const Path& submit_d_rel
    ) const;

    // Admin Subcommand : mucs-admin.cpp

    virtual void admin_authenticate() const;

    virtual void admin_dump();

    virtual void admin_update_password();

};


#endif
