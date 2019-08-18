#!/usr/bin/env python3
# vim: foldmethod=marker

# Imports {{{

import getpass
import os
import subprocess
import sys
from argparse import ArgumentParser
from subprocess import DEVNULL

from consts import *
from exc import *
from models import *
from util import *

# }}}


def admin(ns, configs):
    if os.geteuid() != 0:
        raise SubcommandError('Not authorized')

    cfg = configs.get_course_config(ns.course)

    if ns.admin_cmd == 'dump':
        admin_dump(ns, cfg)
    elif ns.admin_cmd == 'update':
        admin_update(ns, cfg)
    else:
        # Error: invalid subcommand
        # Not possible, caught by argparse
        pass


def admin_dump(ns, config):
    if ns.dump_flags:
        ns.dump_flags = DumpFlags.combine(ns.dump_flags)
    else:
        ns.dump_flags = DumpFlags.all_flags()

    if ns.dump_flags & DumpFlags.homeworks:
        print()
        print(W_BOLD('Homeworks:'))
        print_table(config['homeworks'].items())

    if ns.dump_flags & DumpFlags.labs:
        print()
        print(W_BOLD('Lab Sessions:'))
        grid = sorted(config['labs'].pretty_grid())
        print_table(grid, fmt='%s:  %s  %s - %s')

    if ns.dump_flags & DumpFlags.roster:
        print()
        print(W_BOLD('Roster:'))
        print_table(sorted(config['roster'].items()))

    print()


def admin_update(ns, config):
    print('update:', ns)

    print()
    p = subprocess.run(['../bin/mucs-update-roster'], stdout=DEVNULL)
    if p.returncode == 0:
        print(W_GREEN('Successfully updated %s roster' % ns.course))
    elif p.returncode == 1:
        print(W_RED('Could not update %s roster' % ns.course))
    print()


def examples():
    print()
    print('To submit all of the files in the current directory:')
    print('    mucs submit 1050 hw *')
    print()
    print('To submit specific files:')
    print('    mucs submit 2050 lab impl.c impl.h')
    print()


def get_current_assignment(config, assignment_type):
    course = config['course_number']

    if assignment_type == 'hw':
        current_assignment = config.get_current_hw()
        if current_assignment is None:
            raise SubcommandError(
                'No open homework assignments for course: ' + course)
        else:
            return current_assignment

    elif assignment_type == 'lab':
        current_assignment = config['current_lab']
        if current_assignment is None:
            raise SubcommandError(
                'No open labs for course: ' + course)
        else:
            return current_assignment

    else:
        raise SubcommandError(
            'Assignment type not recognized (must be hw or lab): '
            + assignment_type)


def submit(ns, configs):
    cfg = configs.get_course_config(ns.course)

    current_assignment = get_current_assignment(cfg, ns.assignment_type)

    if not ns.yes:
        spacer = get_term_width() * '='

        print()
        print(W_GREEN(spacer))
        print('Submission Review:')
        print_table([
            ('Course:', W_BOLD(ns.course)),
            ('Assignment:', W_BOLD(current_assignment)),
            ('Files:', W_BOLD(' '.join(ns.sources)))
        ], indent='        ')
        print(W_GREEN(spacer))
        print()

        user_in = input('Are you sure you want to submit [Y/n]? ')
        print()
        if user_in.lower() != 'y':
            die(0, 'Submission cancelled\n')

    # TODO: submit files
    # filedao.submit(ns.course, current_assignment, ns.sources)

    print(W_GREEN('Submission complete\n'))
    die(0)


def main(argv):
    parser = ArgumentParser(prog='mucs')

    subparsers = parser.add_subparsers(dest='cmd')

    # Subcommand Admin

    parser_admin = subparsers.add_parser('admin')

    subparsers_admin = parser_admin.add_subparsers(dest='admin_cmd')

    admin_subcmd_args = ArgumentParser(add_help=False)
    admin_subcmd_args.add_argument(
        'course', help='the target course number')

    # Subcommand Admin Dump

    parser_admin_dump = subparsers_admin.add_parser(
        'dump', parents=[admin_subcmd_args])

    parser_admin_dump.add_argument(
        '-w', '--homeworks',
        action='append_const', const=DumpFlags.homeworks, dest='dump_flags',
        help='print homeworks')
    parser_admin_dump.add_argument(
        '-l', '--labs',
        action='append_const', const=DumpFlags.labs, dest='dump_flags',
        help='print lab sessions')
    parser_admin_dump.add_argument(
        '-r', '--roster',
        action='append_const', const=DumpFlags.roster, dest='dump_flags',
        help='print the roster')
    parser_admin_dump.add_argument(
        '-c', '--current-assignments',
        action='append_const', const=DumpFlags.current, dest='dump_flags',
        help='print the current assignments')

    # Subcommand Admin Update

    parser_admin_update = subparsers_admin.add_parser(
        'update', parents=[admin_subcmd_args])

    # Subcommand Examples

    parser_examples = subparsers.add_parser('examples', add_help=False)

    # Subcommand Submit

    parser_submit = subparsers.add_parser('submit')
    parser_submit.add_argument(
        '-y', '--yes', action='store_true',
        help='do not ask for submit confirmation')
    parser_submit.add_argument(
        'course',
        help='the course number to submit the assignment')
    parser_submit.add_argument(
        'assignment_type', choices=('hw', 'lab'), metavar='assignment_type',
        help='the type of the assignment being turned in (hw or lab)')
    parser_submit.add_argument(
        'sources', nargs='+',
        help='source files to turn in')

    namespace = parser.parse_args()

    # Parse Configs

    try:
        configs = Configs(CONFIG_DIR)
    except ValidationError as ve:
        die(ve.color_message)

    # Execute

    try:
        if namespace.cmd == 'admin':
            admin(namespace, configs)
        elif namespace.cmd == 'examples':
            examples()
        elif namespace.cmd == 'submit':
            submit(namespace, configs)
        else:
            parser.print_help()
    except SubcommandError as se:
        die(se.color_message)

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
