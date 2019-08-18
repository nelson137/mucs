submit

===============================================================================
CONTENTS

  1. Lab Sessions File .......................... lab-sessions
  2. Homework Assignments File .................. homeworks

===============================================================================
1. Lab Sessions File

The lab sessions file is JSON with the following format:

    [
        {
            "session": "A",
            "weekday": 0,
            "start": "14:00:00",
            "end": "16:30:00"
        },
        {
            "session": "B,C",
            "weekday": 1,
            "start": "12:00:00",
            "end": "14:30:00"
        }
        ...
    ]

The root object of the file is an array. Each element of the array is an
object containing the necessary information for a lab session.

Each lab session object must contain the following keys:
    "session": a string of the letter associated with the lab session.
    "weekday": an integer from 0 to 6. 0 is Monday, 4 is Friday, and 5 and 6
        are Saturday and Sunday.
    "start": a string of the lab session's start time.
    "end": a string of the lab session's end time.

The "start" and "end" keys must have the format "hour:minute:second" where
hour is an integer from 0 to 23, and minute and second are integers from 0 to
59.

===============================================================================
2. Homework Assignments File

The homework assignments file is JSON with the following format:

    [
        {
            "name": "HW1",
            "duedate": "2018-01-02T17:00:00"
        },
        ...
    ]

The root object of the file is an array. Each element of the array is an
object containing the necessary information for a homework assignment.

Each homework assignment object must contain the following keys:
    "name": a string of the assignment's name.
    "duedate": a string of the due date of the assignment.

The "duedate" key must have the format
"year-month-dayThour:minute:second":
    year is a 4 digit integer,
    month is an integer from 1 to 12,
    day is an integer from 1 to 29, 30, or 31 (whichever is the last day of
        that month),
    the letter "T",
    hour is an integer from 0 to 23,
    minute and second are integers from 0 to 59

===============================================================================
  vim: ft=help tw=78 et ts=4 sw=4 sts=4
