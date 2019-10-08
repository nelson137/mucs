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
    def __init__(self, submission_d, course, current_assignment):
        self.course = course
        self.current_assignment = current_assignment
        self.username = getpass.getuser()

        self.submit_d = submission_d
        self.course_d = submission_d / ('cs' + course)
        # self.course_makefile_f = self.course_d / 'Makefile'
        self.assignment_d = self.course_d / current_assignment
        self.submit_d = self.assignment_d / self.username
        # self.submit_makefile = self.submit_d / 'Makefile'
        self.submit_exe = PosixPath('/group/cs1050/bin/mucs-submit')

    @staticmethod
    def make_tempdir():
        tempdir = tempfile.mkdtemp(prefix=TEMPDIR_PREFIX)
        os.chmod(tempdir, S_IRWXU | S_IRWXG | S_IRWXO)
        return PosixPath(tempdir)

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
        # if not self.course_makefile_f.exists():
            # raise MucsError(
                # 'Course Makefile does not exist',
                # reason=str(self.course_makefile_f))

        if not shutil.which(self.submit_exe):
            raise MucsError('Executable not found', reason=self.submit_exe)

    def submit(self, sources):
        self.pre_submit()

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

        # # Symbolic link course Makefile into submission directory
        # if not self.submit_makefile.exists():
            # os.chdir(self.submit_d)
            # PosixPath('Makefile').symlink_to('../../Makefile')
            # os.chdir(cwd)

        # Copy sources into submit directory
        return subprocess.run(
            [self.submit_exe, self.course, self.current_assignment,
             self.username, tempdir],
            stdout=DEVNULL, stderr=PIPE)

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
