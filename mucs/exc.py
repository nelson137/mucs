# vim: foldmethod=marker

# Imports {{{

import re

from consts import *

# }}}


__all__ = ['MucsError']


class _ColorError(Exception):
    def __init__(self, *msg_pieces, reason=None):
        self.msg = ' '.join(msg_pieces)

        if reason is None:
            self.color_msg = W_RED(self.msg)
        else:
            reason = str(reason)
            self.msg += ': '
            self.color_msg = W_RED(self.msg) + W_BOLD(reason)
            self.msg += reason

        super().__init__(self.msg)


class MucsError(_ColorError):
    pass
