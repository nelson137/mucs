#!/usr/bin/env python3
# vim: foldmethod=marker

# Imports {{{

import getpass
import subprocess
from argparse import ArgumentParser
from subprocess import DEVNULL, PIPE

from consts import *
from dao import *
from exc import MucsError
from models import *
from printer import Printer, printline
from util import *

# }}}


def admin(ns, configs):
    error_no_command = 'No command given'

    try:
        cfg = configs.get_config(ns.course)
    except AttributeError:
        raise MucsError(error_no_command)

    passwd_in = get_password('Admin password for %s: ' % ns.course)
    if md5(passwd_in) != cfg['admin_hash']:
        raise MucsError('Not authorized')

    if ns.admin_cmd == 'dump':
        admin_dump(ns, cfg)
    elif ns.admin_cmd == 'update-password':
        admin_update_password(ns, cfg)
    elif ns.admin_cmd == 'update-roster':
        admin_update_roster(ns, cfg)
    else:
        raise MucsError(error_no_command)


def admin_dump(ns, config):
    if ns.dump_flags:
        ns.dump_flags = DumpFlags.combine(ns.dump_flags)
    else:
        ns.dump_flags = DumpFlags.all_flags()

    if ns.dump_flags & DumpFlags.current:
        with Printer():
            print(W_BOLD('Current Assignments:'))
            print_table([
                ('hw:', config.get_current_hw()),
                ('lab:', config.get_current_lab())
            ])

    if ns.dump_flags & DumpFlags.homeworks:
        with Printer():
            print(W_BOLD('Homeworks:'))
            print_table(config['homeworks'].items())

    if ns.dump_flags & DumpFlags.labs:
        with Printer():
            print(W_BOLD('Lab Sessions:'))
            grid = sorted(config['labs'].pretty_grid())
            print_table(grid, fmt='%s:  %s  %s - %s')

    if ns.dump_flags & DumpFlags.roster:
        with Printer():
            print(W_BOLD('Roster:'))
            print_table(sorted(config['roster'].items()))


def admin_update_password(ns, config):
    with Printer():
        new_passwd = get_password('New admin password for %s: ' % ns.course)
        confirm = get_password(
            'Confirm new admin password for %s: ' % ns.course)

        if confirm != new_passwd:
            raise MucsError('Passwords do not match')

        h = md5(new_passwd)

        with open('./etc/mucs/cs1050.json', 'r') as f:
            p = subprocess.run(
                ['/usr/bin/jq', '-M', '--indent', '4', '.admin_hash="%s"' % h],
                stdin=f, stdout=PIPE, stderr=DEVNULL)
        if p.returncode == 0:
            with open('./etc/mucs/cs1050.json', 'w') as f:
                f.write(p.stdout.decode())
        else:
            raise MucsError('Error updating admin hash')


def admin_update_roster(ns, config):
    p = subprocess.run(['../bin/mucs-update-roster'])
    if p.returncode == 0:
        printline(W_GREEN('Successfully updated %s roster' % ns.course))
    elif p.returncode == 1:
        printline(W_RED('Could not update %s roster' % ns.course))


def examples():
    with Printer():
        print('To submit all of the files in the current directory:')
        print('    mucs submit 1050 hw *')
        print()
        print('To submit specific files:')
        print('    mucs submit 2050 lab impl.c impl.h')


def submit(ns, configs):
    cfg = configs.get_config(ns.course)
    username = getpass.getuser()

    if ns.assignment_type == 'hw':
        current_assignment = cfg.get_current_hw()
        if current_assignment is None:
            raise MucsError(
                'No open homework assignments for course',
                reason=ns.course)

    elif ns.assignment_type == 'lab':
        current_assignment = cfg.get_current_lab()
        if current_assignment is None:
            raise MucsError('No open labs for course', reason=ns.course)

        if username not in cfg['roster']:
            raise MucsError('User not in course', ns.course, reason=username)

        letter = cfg['roster'][username]
        sesh = cfg['labs'][letter]
        if not sesh.is_active():
            weekday, start, end = sesh.get_tuple()
            raise MucsError(
                'Lab %s is not in session' % letter,
                reason='%s from %s to %s' % (weekday, start, end))

    else:
        pass  # Not possible, caught by parser

    if not ns.yes:
        spacer = get_term_width() * '='

        with Printer():
            print(W_GREEN(spacer))
            print('Submission Review:')
            print_table([
                ('Course:', W_BOLD(ns.course)),
                ('Assignment:', W_BOLD(current_assignment)),
                ('User:', W_BOLD(username)),
                ('Files:', W_BOLD(' '.join(ns.sources)))
            ], indent='  ')
            print(W_GREEN(spacer))

            user_in = input('Are you sure you want to submit [Y/n]? ')
            if user_in.lower() != 'y':
                die(0, '\nSubmission cancelled')

    filedao = FileDao(SUBMISSION_D, ns.course, current_assignment)
    filedao.submit(ns.sources)

    printline(W_GREEN('Submission complete'))


def mucs(namespace):
    # Parse Configs
    try:
        configs = Configs(CONFIG_D)
    except MucsError as me:
        die(me.color_msg)

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
    except MucsError as me:
        die(me.color_msg)


def main():
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

    # Subcommand Admin Update-Password

    parser_admin_update = subparsers_admin.add_parser(
        'update-password', parents=[admin_subcmd_args])

    # Subcommand Admin Update-Roster

    parser_admin_update = subparsers_admin.add_parser(
        'update-roster', parents=[admin_subcmd_args])

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

    mucs(parser.parse_args())


if __name__ == '__main__':
    main()
