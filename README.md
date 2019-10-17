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

## OPTIONS

**`-h`**, **`--help`**
<br/>
Print the help message.

## CONFIG

Mucs is a config-driven program that looks in `/group/cs1050/config.d/` for files with the extension `.json`. Each of these files must contain valid json or an error message will be printed on execution. **Note that mucs will not do anything other than print the error message until the invalid json is fixed.**

### Example Invalid Config

`/group/cs1050/config.d/invalid.json:`

    {
        "course_number": "1050"
        // JSON does not support comments
    }

Executing `mucs`:

    $ mucs -h

    Invalid json: /group/cs1050/config.d/invalid.json

### Example Valid Config

`/group/cs1050/config.d/cs1050.json:`

    {
        "course_number": "1050",
        "admin_hash": "",
        "overrides": ["nwewnh"],
        "homeworks": {
            "hw1": "2019-9-13 21:00:00"
        },
        "labs": {
            "A": "mon 08:00:00 - 10:30:00"
        },
        "roster": {
            "nwewnh": "A"
        }
    }

### Config Properties

**`course_number`**
<br/>
*`string`*
<br/>
A string identifier for the course being described by the config. It corresponds to argument `COURSE` in the [Submit Command](#submit-command).

**`admin_hash`**
<br/>
*`string`*
<br/>
The string md5 message-digest of the admin password. See [Admin Password](#admin-password) for more information.

**`overrides`**
<br/>
*`[string]`*
<br/>
An array of strings where each element is a string of the pawprint of a student whose due date checks will all be overriden. When these students use the [Submit Command](#submit-command), the submission directory will always be `/group/cs1050/submissions/COURSE/LAB_ID/overrides/PAWPRINT`, regardless of the `ASSIGNMENT`. **Note that subsequent submissions will delete the previous one**. These students' submissions will also never be denied, regardless of whether there an open lab session or homework assignment.

**`homeworks`**
<br/>
*`{ "name": "yyyy-mm-dd hh:mm:ss" }`*
<br/>
An object specifying homework assignments and their due dates. The key of each property is the name of the assignment. It is suggested that the names follow the format `hwN` where N is the N-th homework assignment for the semester, e.g. hw1, hw2. The value of each property is a string of the assignment's due date. The due date is an ISO 8601 datetime but with a space separating the date and time instead of a `T`. The time is only precise down to the whole second. An example `homeworks` entry: `"hw1": "2019-9-13 21:00:00"`.

**`labs`**
<br/>
*`{ lab_id": "weekday hh:mm:ss - hh:mm:ss" }`*
<br/>
An object specifying the lab sessions. The key of each property is the lab's identifier. The convention for lab ids is capital letters starting with A. The value of each property is a string of the weekday that the lab is held, followed by its start and end times. Weekdays can be either the full weekday or its abbreviation to three letters, e.g. monday, tue. The times are ISO 8601 times with precision down to the whole second. An example `labs` entry: `"A": "mon 08:00:00 - 10:30:00"`.

**`roster`**
<br/>
*`{ "pawprint": "lab_id[,lab_id2[,...]]" }`*
<br/>
An object specifying the students in the course and the lab session(s) in which they are enrolled. The key of each property is the pawprint of the student. The value of each property is a string describing the labs that the student attends in the form of a comma-separated list of the labs' ids. An entry for a student that attends one lab would look like this: `"nwewnh": "A"`. An entry for a student that attends two labs would look like this: `"nwewnh": "B,C"`.

## ADMIN COMMAND

The admin command is used for mucs administration. All admin sub-commands will first prompt the user for the admin password.

### Admin Password

The admin password is stored as an md5 message-digest in the config file. It is suggested that the `admin_hash` value in the config file is not modified as it is managed by the [Admin Update Password Command](#admin-update-password-command). If the admin password needs to be manually reset, you can use the following bash one-liner to compute the md5 message-digest of the desired new password:

    read -s p; printf "$p" | md5sum

This command will wait for you to type the new password and hit Enter. It will print out the new password's md5 hash-digest so that you can copy it into the config.

### Admin Dump Command

Print information from the config files.

##### Admin Dump Options

The options for this command filter down its output to only the specified pieces of information. Invoking this command with no options is the same as invoking it with all options: `mucs admin dump -clrw`.

**`-c`**, **`--current-assignments`**
<br/>
Print the current lab and homework names.

**`-l`**, **`--labs`**
<br/>
Print the lab sessions.

**`-r`**, **`--roster`**
<br/>
Print the students and the lab session(s) that they attend.

**`-w`**, **`--homeworks`**
<br/>
Print the homeworks and their due dates.

### Admin Update Password Command

TODO

### Admin Host Registration Command

TODO

## SUBMIT COMMAND

TODO

## BUGS

See [GitHub Issues](https://github.com/nelson137/mucs/issues).

## AUTHOR

Nelson Earle <nwewnh@mail.missouri.edu>
