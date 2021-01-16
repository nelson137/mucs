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
    string asgmt_name;
    vector<Path> sources;

    enum DumpFlags {
        DumpOverrides      = 1 << 0,
        DumpLabSessions    = 1 << 1,
        DumpLabAssignments = 1 << 2,
        DumpHomeworks      = 1 << 3,
        DumpRoster         = 1 << 4,
    };

    int dump_flags = 0;

    // CLI : mucs-cli.cpp

    /**
     * Wrapper for calling subcommands.
     *
     * Some setup needs to be done before any subcommand is called:
     *   - Store the current user's username
     *   - Initialize the config data member
     *   - Validate and deserialize the config
     *   - Load the roster
     */
    virtual void invoke(void (Mucs::*subcmd)());

    /**
     * Return a CLI application object.
     */
    virtual unique_ptr<CLI::App> get_cli();

    // Util : mucs.cpp

    /**
     * Run the compile script with the source files and return whether
     * compilation was successful.
     */
    virtual bool compile_sources() const;

    /**
     * Print prompt then get and return user input.
     *
     * Terminal attributes are set to make password entry safe, such as
     * disabling echo.
     */
    virtual string prompt_password(const string& prompt) const;

    /**
     * Print prompt and return true if the user entered yes, otherwise false.
     *
     * Any input that begins with a 'Y' or 'y' is considered to be a yes.
     */
    virtual bool prompt_yesno(const string& prompt) const;

    /**
     * Print a summary of the pending submission.
     *
     * The summary includes:
     *   - A notice if the submission is late or doesn't compile
     *   - The course
     *   - The lab id
     *   - The assignment name
     *   - The user's username
     *   - The files to be submitted
     */
    virtual void submit_summary(
        const LabSesh& lab,
        const string& asgmt_name,
        bool in_submit_window,
        bool compiles
    ) const;

    /**
     * Modify the config file, replacing the old admin_hash with new_hash.
     */
    virtual void update_config_admin_hash(const string& new_hash) const;

    // Submit Subcommand : mucs-submit.cpp

    /**
     * Callback function for the submit subcommand.
     *
     * The main validation logic for submissions is done here.
     */
    virtual void submit();

    /**
     * Submit the files, create any necessary directories and copy the files.
     *
     * If create_link is true then create a relative symbolic link in asgmt_d
     * to submit_d_rel.
     *
     * This logic is separate from Mucs::submit() so it can be stubbed in tests
     * so that they run faster and any potential dangerous write-to-disk
     * operations are not performed.
     */
    virtual void copy_submission_files(
        const Path& asgmt_d,
        const Path& submit_d_rel,
        bool create_link
    ) const;

    // Admin Subcommand : mucs-admin.cpp

    /**
     * Prompt the user for the admin password and validate the input against
     * admin_hash.
     * Throw if the entered password is incorrect.
     */
    virtual void admin_authenticate() const;

    // Admin Dump Subcommand : mucs-admin.cpp

    /**
     * Requires admin authentication.
     * Callback function for the admin dump subcommand.
     *
     * Print config data according to the flags given on the CLI.
     */
    virtual void admin_dump();

    // Admin Update-Password Subcommand : mucs-admin.cpp

    /**
     * Requires admin authentication.
     * Callback function for the admin update-password subcommand.
     *
     * Prompt for a new admin password twice, verify that they match, then
     * update the config file with the new hash.
     */
    virtual void admin_update_password();

};


#endif
