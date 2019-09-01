# vim: foldmethod=marker

# Imports {{{

import calendar
import datetime
import getpass
import hashlib
import shutil
import subprocess
import sys
from subprocess import DEVNULL
from textwrap import TextWrapper

from .consts import *
from .exc import *
from .printer import printline

# }}}


__all__ = ['die', 'get_password', 'get_term_width', 'md5', 'parse_time',
           'parse_weekday', 'print_table', 'weekday_to_str']


def die(*args):
    args = list(args)
    code = 1
    if args and isinstance(args[0], int):
        code = args.pop(0)
    if args:
        printline(W_RED(' '.join(args)), file=sys.stderr)
    sys.exit(code)


def get_password(prompt):
    print(prompt, end='', flush=True, file=sys.stderr)
    try:
        return getpass.getpass('')
    except KeyboardInterrupt:
        print()
        die(0)


def get_term_width(factor=TERM_WIDTH_FACTOR):
    w = shutil.get_terminal_size((80, 24)).columns
    return int(w * factor)


def md5(msg):
    """Return the hex digest of an md5-hashed string."""
    return hashlib.md5(msg.encode()).hexdigest()


def parse_time(time_str):
    try:
        return datetime.datetime.strptime(time_str, TIME_FMT).time()
    except ValueError:
        return None


def parse_weekday(weekday_str):
    return {
        'monday': 0, 'mon': 0,
        'tuesday': 1, 'tue': 1,
        'wednesday': 2, 'wed': 2,
        'thursday': 3, 'thu': 3,
        'friday': 4, 'fri': 4,
        'saturday': 5, 'sat': 5,
        'sunday': 6, 'sun': 6
    }.get(weekday_str.lower())


def print_table(data, fmt=None, indent='', sep='  ', wrap_last=True):
    if not data:
        return

    wrap_w = get_term_width() - len(indent) * 2

    # Convert data to a list of tuples of strings
    data = list(tuple(map(str, row)) for row in data)
    ncols = len(data[0])

    # Default format is the columns joined by 2 spaces
    if fmt is None:
        fmt = sep.join(['%s'] * ncols)

    fmt_minus_one = fmt[:fmt.rfind('%s')]

    # Get the max width for each column
    max_widths = [max(len(row[c]) for row in data)
                  for c in range(ncols-1)]
    max_widths.append(-1)  # Don't pad last column with spaces

    for row in data:
        padded_cols = tuple(
            col.ljust(width) for col, width in zip(row, max_widths))

        if wrap_last:
            sub_i = ' ' * len(fmt_minus_one % padded_cols[:-1])
            wrapper = TextWrapper(width=wrap_w, subsequent_indent=sub_i)
            for line_seg in wrapper.wrap(fmt % padded_cols):
                print(indent + line_seg)
        else:
            print(indent + fmt % padded_cols)


def weekday_to_str(weekday):
    return calendar.day_name[weekday]
