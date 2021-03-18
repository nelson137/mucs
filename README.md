# mucs

Assignment submission system for the University of Missouri — Columbia, Department of Computer Science.

# TABLE OF CONTENTS

- [Table of Contents](#table-of-contents)
- [Synopsis](#synopsis)
- [Mucs Root](#mucs-root)
- [Config](#config)
    - [Example](#example)
    - [Schema](#schema)
    - [How To Use Overrides](#how-to-use-overrides)
- [Roster](#roster)
- [Version Command](#version-command)
- [Admin Command](#admin-command)
    - [Admin Password](#admin-password)
    - [Admin Dump Command](#admin-dump-command)
    - [Admin Update Password Command](#admin-update-password-command)
- [Submit Command](#submit-command)
- [Developer's Guide](#developers-guide)
    - [Build For Debugging](#build-for-debugging)
    - [Release Install](#release-install)
    - [Test Coverage](#test-coverage)
- [Author](#author)

# SYNOPSIS

    mucs [-h]
    mucs version
    mucs admin dump [-a] [-o] [-r] [-s] [-w] COURSE
    mucs admin update-password COURSE
    mucs submit COURSE ASSIGNMENT FILES...

# MUCS ROOT

Mucs relies on a local directory structure called `MUCS_ROOT` to work properly. `MUCS_ROOT` contains the mucs binary as well as some helper scripts, course [config files](#config), a [roster](#roster), all [student submissions](#submit-command), and various other important components. The absolute path of `MUCS_ROOT` must be defined at compile-time, see the [Developer's Guide](#developers-guide) for more information.

# CONFIG

The behavior of Mucs is controlled by JSON configuration files. All Mucs commands require the `COURSE` argument so that it can load the config for that course. Mucs is hard-coded at compile-time to look for configs in a specific directory. The release installation on the Clark servers looks in `/group/cs1050/config.d` for these config files.

For example, if a user executes a shell command like `mucs admin dump 1050` Mucs will attempt to load the config file `/group/cs1050/config.d/1050`.

For a config to be considered valid it must contain valid JSON and validate agains the [Schema](#schema) successfully. If a config doesn't meet these requirements an error message will be printed. **Note that no mucs commands will work for that course until the config is fixed. It will only print the error message.**

## Example

`/group/cs1050/config.d/1050`

    {
        "course_number": "1050",
        "admin_hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        "overrides": {
            "labs": [
                { "user": "jer676", "session": "A" },
                { "user": "nwewnh", "session": "A", "assignment": "lab1" }
            ],
            "homeworks": [
                { "user": "jer676", "name": "hw1" }
            ]
        },
        "homeworks": [
            { "name": "hw1", "duedate": "2021-01-29 17:59:59" }
        ],
        "lab-sessions": [
            { "id": "A", "day": "Mon", "start": "08:00:00", "end": "10:30:00" }
        ],
        "lab-assignments": [
            { "name": "lab1", "week": "2021 Jan 4" }
        ]
    }

## Schema

JSON object keys are referenced with dot notation, i.e. a key like `a.b` indicates the following JSON structure:

    { "a": { "b": 3 } }

Key names like `a[].b` reference key `b` of an object that is an element of array `a`:

    { "a": [ { "b": 3 } ] }

A type annotation like `[X]` is an array whose elements are of type `X`.

The following keys are supported by a mucs config file for a course:

**course_id** `string` *REQUIRED*
<br/>
The identifier for the course that the config describes.

**admin_hash** `string` *REQUIRED*
<br/>
The SHA256 hex-digest of the admin password. See [Admin Password](#admin-password) for more information.

**overrides** `object` *REQUIRED*
<br/>
Specifies the overrides for lab sessions and due dates for assignments on a per student basis. See [How To Use Overrides](#how-to-use-overrides) for more information.

**overrides.labs** `[object]` *REQUIRED*
<br/>
A list of lab session overrides.

**overrides.labs[].user** `string` *REQUIRED*
<br/>
The username of the student whose lab session is being overridden.

**overrides.labs[].session** `string` *REQUIRED*
<br/>
The id of the lab session the student is being overriden to.

**overrides.labs[].assignment** `string`
<br/>
The name of the lab assignment for which the override applies. If this key is absent the override applies to all assignments.

**overrides.homeworks** `[object]` *REQUIRED*
<br/>
A list of homework overrides.

**overrides.homeworks[].user** `string` *REQUIRED*
<br/>
The username of the student whose homework due date is being overridden.

**overrides.homeworks[].name** `string` *REQUIRED*
<br/>
The name of the homework assignment whose due date should be ignored.

**homeworks** `[object]` *REQUIRED*
<br/>
A list of homework assignments.
<br/>
Example of an element: `{ "name": "hw1", "duedate": "2021-01-29 17:59:59" }`

**homeworks[].name** `string` *REQUIRED*
<br/>
The name of the homeworks assignment.

**homeworks[].duedate** `string` *REQUIRED*
<br/>
The due date of the homework assignment. Must be a date-time in the format `YYYY-mm-dd HH:MM:SS`. The time must use the 24-hour clock.

**labs-sessions** `[object]` *REQUIRED*
<br/>
A list of lab sessions.
<br/>
Example of an element: `{ "id": "A", "day": "mon": "start": "10:00:00", "end": "12:30:00" }`

**lab-sessions[].id** `string` *REQUIRED*
<br/>
The id of the lab session.

**lab-sessions[].day** `string` *REQUIRED*
<br/>
The day of the week that the lab session is held. Can be the full word or three letter abbreviation. The first letter may or may not be capitalized.

**lab-sessions[].start** `string` *REQUIRED*
<br/>
The start time of the lab session. Must be a time in the format `HH:MM:SS`. The time must use the 24-hour clock.

**lab-sessions[].end** `string` *REQUIRED*
<br/>
The end time of the lab session. The format is the same as **lab-sessions[].start**.

**lab-assignments** `[object]` *REQUIRED*
<br/>
A list of lab assignments.
<br/>
Example of an element: `{ "name": "lab1", "week": "2020 Jan 1" }`
<br/>
*NOTE: The first day of this example lab assignment is 2020-01-06. See **lab-assignments[].week** for more info.*

**lab-assignments[].name** `string` *REQUIRED*
<br/>
The name of the lab session.

**lab-assignments[].week** `string` *REQUIRED*
<br/>
A description of the week that the assignment is active for all lab session in the format `YEAR MONTH WEEK_INDEX`. The `YEAR` must be a four digit integer. The `MONTH` must be the three letter abbreviation of the name of the month with the first letter capitalized. The `WEEK_INDEX` must be a single digit integer indicating the week of the month, Monday through Sunday, where week 1 is the week of the first monday of the month. The `WEEK_INDEX` should not be less than one.

## How To Use Overrides

For all of these examples, only the `overrides` section of the config is shown.

**One time lab session override**

    "overrides": {
        "labs": [
            { "user": "USER", "session": "NEW_LAB_ID", "assignment": "ASSIGNMENT_NAME" }
        ],
        "homeworks": []
    }

**Indefinite lab session override**

    "overrides": {
        "labs": [
            { "user": "USER", "session": "NEW_LAB_ID" }
        ],
        "homeworks": []
    }

**Homework duedate override**

    "overrides": {
        "labs": [],
        "homeworks": [
            { "user": "USER", "name": "ASSIGNMENT_NAME" }
        ]
    }

# ROSTER

The roster specifies which lab session students are assigned to. The roster is made up of text files whose names are the same as their corresponding lab session ids (`lab-sessions[].id`). The file contains only the usernames of the students belonging to that lab session, each on its own line. These roster files must be in the roster directory which is defined at compile-time. The release installation on the Clark servers uses `/group/cs1050/roster.d` as the roster directory. For example, a lab session with id `X` might have the following roster file:

**/group/cs1050/roster.d/X**

    nwewnh
    jer676

# VERSION COMMAND

The version command prints the version, a short description, and various compile-time constants.

# ADMIN COMMAND

The admin command is used for mucs administration. All admin sub-commands will first prompt the user for the admin password and validate it against the hash in the config.

## Admin Password

The admin password is stored as an SHA256 hex-digest in the config file. It is suggested that the `admin_hash` value in the config file not be modified as it is managed by the [Admin Update Password Command](#admin-update-password-command). If the admin password does need to be manually changed the following bash one-liner can be used to get the SHA256 hex-digest of the desired new password:

    read -s p && printf "$p" | sha256sum

The command will wait for you to type the new password and hit Enter. The SHA256 hex-digest of the entered password will be printed so that you can copy and paste it into the config. Be sure to not include the trailing dash or any spaces around the hex-digest when pasting it in the config.

## Admin Dump Command

    mucs admin dump [-a] [-o] [-r] [-s] [-w] COURSE

Print information from a config file.

This command supports optional flags that filter its output to only the specified pieces of information. For example, executing a shell command like `mucs admin dump -aw 1050` will print only the homework and lab assignments. Invoking this command with no options behaves as if it were invoked with all options.

This command has one positional argument, `COURSE`, which

The optional flags are the following:

**-a**, **--lab-assignments**
<br/>
Print the lab assignments.

**-o**, **--overrides**
<br/>
Print the current lab and homework names.

**-r**, **--roster**
<br/>
Print all students and the lab session in which they are enrolled.

**-s**, **--lab-sessions**
<br/>
Print the lab sessions.

**-w**, **--homeworks**
<br/>
Print the homeworks.

## Admin Update Password Command

    mucs admin update-password COURSE

Update the `admin_hash` value in a config file.

This command will prompt the user for a new password, ask for it to be entered again for confirmation, then replace the old SHA256 hex-digest with that of the new password.

# SUBMIT COMMAND

    mucs submit COURSE ASSIGNMENT FILES...

Submit an assignment.

## Arguments

`COURSE` is the id of the course, this determines which config is loaded.

`ASSIGNMENT` is the name of the assignment. This must be one of `lab-assignments[].name` or `homeworks[].name`.

`FILES` is one or more files to be submitted.

## Description

All submissions are accepted, whether *valid* or *invalid*. A homework submission is considered *valid* if the current datetime is before the duedate of the `ASSIGNMENT` and the `FILES` compile successfully with the compile script. A lab assignment submission is considered *valid* if the current user's lab session (which is queried from the roster) is active, the current week matches that of `ASSIGNMENT`, and the `FILES` compile successfully with the compile script. Submissions that do not meet these requirements are considered *invalid*.

Submissions are stored hierarchically in [Mucs Root](#mucs-root). The `FILES` for a submissions are copied into the submission directory, which has the format:

`MUCS_ROOT/submissions/COURSE/LAB_SESSION/ASSIGNMENT/ARCHIVE/USER_DATE_TIME`.

Components are as follows:

Component | Description
--------- | -----------
`COURSE` | The `COURSE` CLI argument
`LAB_SESSION` | The lab session of the current username, queried from the [Roster](#roster)
`ASSIGNMENT` | The `ASSIGNMENT` CLI argument
`ARCHIVE` | Either ".valid" or ".invalid", determined by the conditions described above
`USER` | The current username
`DATE` | The current date in the format `YYYY-mm-dd`
`TIME` | The current time in the format `HH:MM:SS` using the 24-hour clock

If the submission is *valid* then a relative symbolic link named `USER` is created (or updated if one already exists) in `MUCS_ROOT/submissions/COURSE/LAB_SESSION/ASSIGNMENT` which points to the true submission directory. This is done so that the most recent *valid* submission by a user for a given assignment is readily available for grading. The link is relative so that the assignment directory can be copied to another location and the links remain intact.

# DEVELOPER'S GUIDE

## Project Structure

Path | Description
---- | -----------
include/ | Third party (header-only) libraries
scripts/ | Random scripts (see scripts table below)
src/ | Implementation files and headers
src/models/ | Config file model classes
test/ | Test file and headers
test_root/ | Static test data<br/>Note: should be used *exclusively* for unit tests

Script | Description
------ | -----------
compile | The compile script for release installations
gen-roster | Generate mucs roster files from xlsx lab roster files and the JSON file created by `get-course-students`
get-course-students | Download a JSON file from the Canvas API containing all students in a Canvas course
install-fzf | Download the latest fzf binary<br/>Note: this is run by `post-install`
post-install | Finish installation, do stuff that cmake can't
simple-compile | Simple compile script that can be used for testing a local build
student-vimrc | Install a simple .vimrc for students; if a .vimrc already exists it will ask whether to make a backup or override

## How to build and install

When running cmake to configure the build system, three optional parameters are supported:

    # Run inside the build directory, which is in the root of the repository
    cmake .. [-D_DEBUG] [-D_COVERAGE] [-D_PREFIX=<path>]
        [-D_MUCS_ROOT=<path>] [-D_COMPILE_SCRIPT=<path>]

- `_DEBUG`: compile for debugging
- `_COVERAGE`: compile with coverage information
- `_PREFIX`: for the targetted install location for release.
- `_MUCS_ROOT`: TODO
- `_COMPILE_SCRIPT`: TODO

### Build for debugging

    mkdir build; cd build
    cmake .. -D_DEBUG=1
    make -j8

### Build and install local

The `test_root` directory should not be used for testing a local build of mucs because it contains static testing data that some tests rely on. To test a local build mucs should be built and installed into a clean directory (e.g. `build/my_root`)

### Build for production and install

    mkdir build; cd build
    cmake .. -D_PREFIX=/group/cs1050
    make -j8
    ./mucs_test  # make sure tests are passing
    make install

### Test Coverage

A script is provided that will configure and build, run the tests, then generate an html file that will show functions and lines that were tested in all source files. Open `build/coverage/index.html` in a web browser to see the analysis. If the test binary has already been built it will be recompiled with the coverage flags.

    ./cov

# AUTHOR

Nelson Earle <nwewnh@umsystem.edu>
