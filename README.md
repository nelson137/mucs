# mucs

Assignment submission system for the University of Missouri — Columbia Department of Computer Science.

## TABLE OF CONTENTS

- [Table of Contents](#table-of-contents)
- [Synopsis](#synopsis)
- [Options](#options)
- [Config](#config)
    - [Example of an invalid config](#example-invalid-config)
    - [Example of a valid config](#example-valid-config)
    - [Config Properties](#config-properties)
- [Admin Command](#admin-command)
    - [Admin Password](#admin-password)
    - [Admin Dump Command](#admin-dump-command)
    - [Admin Update Password Command](#admin-update-password-command)
    - [Admin Host Registration Command](#admin-host-registration-command)
- [Submit Command](#submit-command)
- [Bugs](#bugs)
- [Author](#author)

## SYNOPSIS

    mucs [-h]
    mucs admin dump [-c] [-l] [-r] [-w]
    mucs admin update-password
    mucs admin update-roster
    mucs submit COURSE ASSIGNMENT FILE [...FILES]

## CONFIG

The behavior of Mucs is controlled by configuration files. All Mucs commands require the `COURSE` argument so that it can load the config for the specified course. Mucs is hard-coded to look for configs in the directory `/group/cs1050/config.d/`. For example, if a shell command executing Mucs begins with `mucs submit 1050`, then Mucs will attempt to load the config file `/group/cs1050/config.d/1050`.

If the config file for the specified course is invalid an error message will be printed. **Note that Mucs will not do anything other than print this error message until the file is fixed.** For a config file to be considered valid it must contain a valid json object that specifies all [Config Properties](#config-properties).

### Example Invalid Config

`/group/cs1050/config.d/invalid`

    {
        "course_number": "1050"
        // JSON does not support comments
    }

Executing Mucs:

    $ mucs submit invalid

    Invalid json: /group/cs1050/config.d/invalid

### Example Valid Config

The admin hash used in this example is the message-digest of an empty string.

`/group/cs1050/config.d/1050`

    {
        "course_number": "1050",
        "admin_hash": "d41d8cd98f00b204e9800998ecf8427e",
        "overrides": ["jer676"],
        "current_lab": "lab1",
        "homeworks": { "hw1": "2019-9-13 21:00:00" },
        "labs": { "A": "mon 08:00:00 - 10:30:00" },
        "roster": { "nwewnh": "A" }
    }

### Config Properties

**admin_hash** `string`
<br/>
A string of the md5 message-digest of the admin password. See [Admin Password](#admin-password) for more information.

**course_number** `string`
<br/>
A string of the identifier for the course that the config describes. It corresponds to argument `COURSE` in the [Submit Command](#submit-command).

**current_lab** `string`
A string of the current lab assignment. This property must be kept up to date with the weekly labs. The first is typically `lab1`.

**homeworks** `"name": "yyyy-mm-dd hh:mm:ss"`
<br/>
An object describing homework assignments and their due dates. The key of each property is the name of the assignment. The value of each property is a string of the assignment's due date. The due date is an ISO 8601 datetime, precise to the whole second, but with a space separating the date and time instead of the letter `T`.
<br/>
Example entry: `"hw1": "2019-09-13 17:00:00"`

**labs** `"lab_id": "weekday hh:mm:ss - hh:mm:ss"`
<br/>
An object describing lab sessions. The key of each property is the lab's identifier. It's value is a string of the weekday that the lab is held, followed by its start and end time. Weekdays can be either the full weekday or its abbreviation to three letters, case insensitive. The times are ISO 8601 times, precise to the whole second.
<br/>
Example entry: `"A": "mon 08:00:00 - 10:30:00"`

**overrides** `[string]`
<br/>
An array where each element is a string of the pawprint of a student who will be allowed to submit any assignment at any time. It will force the `COURSE` argument for the [Submit Command](#submit-command) to always be `override`, meaning the submission directory will always be `/group/cs1050/submissions/COURSE/LAB_ID/overrides/PAWPRINT` regardless of the current assignment. **Note that submissions always delete any previous submission**, therefore the `overrides` array is should only be used for one assignment.
<br/>
Example: `"overrides": ["nwewnh"]`

**roster** `"pawprint": "lab_id[,lab_id2...]"`
<br/>
An object describing the students in the course and the lab session(s) in which they are enrolled. The key of each property is the student's pawprint. It's value is a string describing the labs that the student attends in the form of a comma-separated list of the labs' ids.
<br/>
Example entry with one lab `"nwewnh": "A"`
<br/>
Example entry with two labs: `"nwewnh": "B,C"`

## ADMIN COMMAND

The admin command is used for mucs administration. All admin sub-commands will first prompt the user for the admin password.

### Admin Password

The admin password is stored as an md5 message-digest in the config file. It is suggested that the `admin_hash` value in the config file is not modified as it is managed by the [Admin Update Password Command](#admin-update-password-command). If the admin password needs to be manually reset, you can use the following bash one-liner to compute the md5 message-digest of the desired new password:

    read -s p; printf "$p" | md5sum

The command will wait for you to type the new password and hit Enter. The md5 message-digest of the new password will be printed out so that you can copy it into the config.

### Admin Dump Command

Print information from the config files.

##### Admin Dump Options

The options for this command filter its output to only the specified pieces of information. For example, the shell command `mucs admin dump -lc` will print only the labs and the current lab assignment. Invoking this command with no options behaves as if it were invoked with all options.

**-c**, **--current-assignments**
<br/>
Print the current lab and homework names.

**-l**, **--labs**
<br/>
Print the lab sessions.

**-r**, **--roster**
<br/>
Print the students and the lab session(s) in which they are enrolled.

**-w**, **--homeworks**
<br/>
Print the homeworks.

### Admin Update Password Command

TODO

### Admin Host Registration Command

TODO

## SUBMIT COMMAND

TODO

## AUTHOR

Nelson Earle <nwewnh@mail.missouri.edu>
