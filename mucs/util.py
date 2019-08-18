# vim: foldmethod=marker

# Imports {{{

import calendar
import datetime
import re
import shutil
import subprocess
import sys
from subprocess import DEVNULL
from textwrap import TextWrapper

from consts import *

# }}}

__all__ = ['die', 'get_term_width', 'parse_time', 'parse_weekday',
           'print_table', 'weekday_to_str', 'DumpFlags', 'SubcommandError',
           'ValidationError']


def die(*args):
    args = list(args)
    code = 1
    if args and isinstance(args[0], int):
        code = args.pop(0)
    if args:
        print(W_RED(' '.join(args)), file=sys.stderr)
    sys.exit(code)


# XXX
def file_compiles(fn):
    p = subprocess.run(
        ['gcc', '-Wall', '-Werror', '-c', fn, '-o', '/dev/null'],
        stdout=DEVNULL, stderr=DEVNULL)
    return p.returncode == 0


def get_term_width(factor=TERM_WIDTH_FACTOR):
    w = shutil.get_terminal_size((80, 24)).columns
    return int(w * factor)


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


def print_table(data, fmt=None, indent=''):
    w = get_term_width()

    # Convert data to a list of tuples of strings
    data = list(tuple(map(str, row)) for row in data)
    ncols = len(data[0])

    # Default format is the columns joined by 2 spaces
    fmt = ('  %s' * ncols)[2:] if fmt is None else fmt

    # Get the max width for each column
    max_widths = [max(len(row[i]) for row in data)
                  for i in range(ncols)]

    for row in data:
        # Format each column with ljust
        padded_cols = tuple(
            col.ljust(width)
            for col, width in zip(row, max_widths))

        sub_indent = ' ' * len(padded_cols[0])
        wrapper = TextWrapper(width=w, subsequent_indent=sub_indent)

        # Print the formatted row
        # Lines that don't need wrapping only have one line segment
        for line_seg in wrapper.wrap(fmt % padded_cols):
            print(indent + line_seg)


def weekday_to_str(weekday):
    return calendar.day_name[weekday]


class _ColorException(Exception):
    def __init__(self, full_msg):
        super().__init__(full_msg)

        mo = re.search('^(.+):\s*(.+)$', full_msg)
        if mo:
            msg, reason = mo.groups()
            msg += ':'
            self.message = '%s %s' % (msg, reason)
            self.color_message = '%s %s' % (W_RED(msg), W_BOLD(reason))
        else:
            msg, reason = full_msg, ''
            self.message = msg
            self.color_message = W_RED(msg)


class SubcommandError(_ColorException):
    pass


class ValidationError(_ColorException):
    pass
