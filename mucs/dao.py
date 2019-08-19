# vim: foldmethod=marker

# Imports {{{

import datetime
import getpass
import os
import shutil
import subprocess
from pathlib import PosixPath
from subprocess import DEVNULL

from consts import *
from exc import *
from util import *

# }}}


class FileDao:
    def __init__(self, course, current_assignment):
        username = getpass.getuser()
        now = datetime.datetime.now().strftime(MAKE_LOG_FMT)

        self.course_d = PosixPath('./group') / ('cs' + course)
        self.course_makefile_f = self.course_d / 'Makefile'
        self.assignment_d = self.course_d / current_assignment
        self.submit_d = self.assignment_d / username
        self.submit_makefile = self.submit_d / 'Makefile'
        self.make_log = self.submit_d / ('makelog.' + now)

    def pre_submit(self):
        if not self.course_d.exists():
            raise SubmissionError(
                'Course submission directory does not exist: ' +
                str(self.course_d))

        if not self.course_makefile_f.exists():
            raise SubmissionError(
                'Course Makefile does not exist: ' +
                str(self.course_makefile_f))

        if not self.assignment_d.exists():
            raise SubmissionError(
                'Assignment directory does not exist: ' +
                str(self.assignment_d))

        if not self.submit_d.exists():
            try:
                self.submit_d.mkdir()
            except (FileNotFoundError, OSError):
                raise SubmissionError(
                    'Could not make submission directory: ' +
                    str(self.submit_d))

    def submit(self, sources):
        cwd = PosixPath.cwd()

        self.pre_submit()

        # Symbolic link course Makefile into submission directory
        if not self.submit_makefile.exists():
            os.chdir(self.submit_d)
            PosixPath('Makefile').symlink_to('../../Makefile')
            os.chdir(cwd)

        # Check that all sources exist
        for src in sources:
            if not PosixPath(src).exists():
                raise SubmissionError('File not found: ' + src)

        # Copy sources into submit directory
        for src in sources:
            shutil.copy(src, self.submit_d)

        # Run make
        # Delete
        with open(self.make_log, 'w') as log:
            make_args = ['/usr/bin/make', '-C', str(self.submit_d)]
            p = subprocess.run(make_args, stdout=log, stderr=log)
            if p.returncode != 0:
                subprocess.run([*make_args, '__wipe'],
                               stdout=DEVNULL, stderr=DEVNULL)
                raise SubmissionError(
                    'Could not compile sources: ' + ' '.join(sources))
