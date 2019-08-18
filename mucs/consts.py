# vim: foldmethod=marker

# Imports {{{

from enum import IntFlag, auto

# }}}


RESET = '\33[0m'
BOLD = '\33[1m'
UNBOLD = '\33[21m'
GREEN = '\33[32m'
RED = '\33[91m'

TERM_WIDTH_FACTOR = .75

CONFIG_DIR = './etc/mucs'

HOMEWORK_FMT = '%Y-%m-%d %H:%M:%S'
HOMEWORK_PRETTY_FMT = '%b %d, %Y %I:%M:%S %p'
TIME_FMT = '%H:%M:%S'
TIME_PRETTY_FMT = '%I:%M:%S %p'


def W_BOLD(*msgs):
    return BOLD + ' '.join(msgs) + UNBOLD


def W_GREEN(*msgs):
    return GREEN + ' '.join(msgs) + RESET


def W_RED(*msgs):
    return RED + ' '.join(msgs) + RESET


class DumpFlags(IntFlag):
    homeworks = auto()
    labs = auto()
    roster = auto()
    current = auto()

    @staticmethod
    def combine(flags):
        ret = 0
        for f in flags:
            ret |= f
        return ret

    @classmethod
    def all_flags(cls):
        return cls.combine(
            getattr(cls, a)
            for a in dir(cls)
            if not a.startswith('_'))
