# vim: foldmethod=marker

# Imports {{{

import re

from consts import *

# }}}


class _ColorException(Exception):
    def __init__(self, full_msg):
        self.full_msg = full_msg
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


class SubmissionError(_ColorException):
    pass


class ValidationError(_ColorException):
    pass
