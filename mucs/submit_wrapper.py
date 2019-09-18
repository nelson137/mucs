# vim: foldmethod=marker

# Imports {{{

import getpass
import os
import re
import shutil
import stat
import subprocess
import tempfile
from pathlib import PosixPath
from stat import S_IRWXG, S_IRWXO, S_IRWXU
from subprocess import DEVNULL, PIPE

from .consts import *
from .exc import MucsError
from .util import *

# }}}


class SubmitWrapper:
    def __init__(self, submission_d, course, assignment):
        self.course = course
        self.assignment = assignment
        self.user = getpass.getuser()
        self.submit_exe = '/group/cs1050/bin/mucs-submit'

    @staticmethod
    def make_tempdir():
        tempdir = tempfile.mkdtemp(prefix=TEMPDIR_PREFIX)
        os.chmod(tempdir, S_IRWXU | S_IRWXG | S_IRWXO)
        return PosixPath(tempdir)

    def submit(self, sources):
        if not shutil.which(self.submit_exe):
            raise MucsError('Executable not found', reason=self.submit_exe)

        cwd = PosixPath.cwd()
        tempdir = self.make_tempdir()

        for src in sources:
            src = PosixPath(src)
            if not src.is_absolute():
                src = cwd / src
            if not src.exists():
                raise MucsError('File not found', reason=src)
            if src.is_dir():
                raise MucsError('Cannot submit directories', reason=src)
            shutil.copyfile(src, tempdir / src.name)

        # Copy sources into submit directory
        return subprocess.run(
            [self.submit_exe, self.course, self.assignment, self.user,
             tempdir],
            stdout=DEVNULL, stderr=PIPE)
