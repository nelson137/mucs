# vim: foldmethod=marker

# Imports {{{

import getpass
import os
import re
import shutil
import stat
import subprocess
from pathlib import PosixPath
from subprocess import DEVNULL

from consts import *
from exc import MucsError
from util import *

# }}}


class FileDao:
    def __init__(self, submission_d, course, current_assignment):
        username = getpass.getuser()

        self.course_d = submission_d / ('cs' + course)
        # self.course_makefile_f = self.course_d / 'Makefile'
        self.assignment_d = self.course_d / current_assignment
        self.submit_d = self.assignment_d / username
        # self.submit_makefile = self.submit_d / 'Makefile'

    def new_logfile(self):
        logfile_nums = [0]
        for fn in os.listdir(self.submit_d):
            try:
                num = re.search('make.(\d+).log', fn).groups()[0]
                logfile_nums.append(int(num))
            except AttributeError:
                pass  # re.search() found no match in fn
        next_num = max(logfile_nums) + 1
        basename = 'make.%d.log' % next_num
        return open(self.submit_d / basename, 'w')

    def pre_submit(self):
        if not self.course_d.exists():
            raise MucsError(
                'Course submission directory does not exist',
                reason=str(self.course_d))

        # if not self.course_makefile_f.exists():
            # raise MucsError(
                # 'Course Makefile does not exist',
                # reason=str(self.course_makefile_f))

        if not self.assignment_d.exists():
            try:
                self.assignment_d.mkdir()
            except (FileNotFoundError, OSError):
                raise MucsError(
                    'Could not make assignment directory',
                    reason=str(self.assignment_d))

        if not self.submit_d.exists():
            try:
                self.submit_d.mkdir()
            except (FileNotFoundError, OSError):
                raise MucsError(
                    'Could not make submission directory',
                    reason=str(self.submit_d))

    def submit(self, sources):
        cwd = PosixPath.cwd()

        self.pre_submit()

        # # Symbolic link course Makefile into submission directory
        # if not self.submit_makefile.exists():
            # os.chdir(self.submit_d)
            # PosixPath('Makefile').symlink_to('../../Makefile')
            # os.chdir(cwd)

        # Check that all sources exist
        for src in sources:
            if not PosixPath(src).exists():
                raise MucsError('File not found', reason=src)

        # Copy sources into submit directory
        # chmod 060 src
        for src in sources:
            src_submit_fn = self.submit_d / os.path.basename(src)
            shutil.copyfile(src, src_submit_fn)
            os.chmod(src_submit_fn, stat.S_IRGRP | stat.S_IWGRP)

        # # Run make
        # # Delete
        # with self.new_logfile() as log:
        #     make_args = ['/usr/bin/make', '-C', str(self.submit_d)]
        #     p = subprocess.run(make_args, stdout=log, stderr=log)
        #     if p.returncode != 0:
        #         subprocess.run([*make_args, '__wipe'],
        #                        stdout=DEVNULL, stderr=DEVNULL)
        #         raise MucsError(
        #             'Could not compile sources',
        #             reason=' '.join(sources))
