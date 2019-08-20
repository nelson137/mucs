# vim: foldmethod=marker

# Imports {{{

import datetime
import json
import os
import re
from collections import namedtuple

from consts import *
from exc import *
from util import *

# }}}


class Homeworks(dict):
    key = 'homeworks'

    def __init__(self, config):
        self.ERROR_NO_HOMEWORKS = \
            'Config must specify a homeworks object: ' + config.filename
        self.ERROR_INVALID_TYPE_HOMEWORKS = \
            'Config expected type object for key "%s": %s' \
            % (self.key, config.filename)

        parse_obj = '%s["%s"]["%s"]' % (config.filename, self.key, '%s')
        self.ERROR_INVALID_TYPE_ENTRY = \
            'Homework entries must be of type string: ' + parse_obj
        self.ERROR_FMT = \
            'Homework entries must be in the format ' \
            '"yyyy-mm-dd hh:mm:ss": ' + parse_obj

        if self.key not in config:
            raise ValidationError(self.ERROR_NO_HOMEWORKS)

        if not isinstance(config[self.key], dict):
            raise ValidationError(self.ERROR_INVALID_TYPE_HOMEWORKS)

        self.parse(config[self.key])

    def __repr__(self):
        fmt = HOMEWORK_PRETTY_FMT
        return str({n: e.strftime(fmt) for n, e in self.items()})

    @staticmethod
    def parse_duedate(dd):
        return datetime.datetime.strptime(dd, HOMEWORK_FMT)

    def parse(self, homeworks):
        for name, duedate in homeworks.items():
            if not isinstance(duedate, str):
                raise ValidationError(self.ERROR_INVALID_TYPE_ENTRY % name)

            try:
                self[name.lower()] = self.parse_duedate(duedate)
            except ValueError:
                raise ValidationError(self.ERROR_FMT % name)


class LabSesh(namedtuple('LabSesh', 'weekday start end')):
    def get_tuple(self):
        weekday = weekday_to_str(self.weekday)
        start = self.start.strftime(TIME_PRETTY_FMT)
        end = self.end.strftime(TIME_PRETTY_FMT)
        return (weekday, start, end)


class LabSessions(dict):
    key = 'labs'

    def __init__(self, config):
        self.ERROR_NO_LABS = \
            'Config must specify a labs object: ' + config.filename
        self.ERROR_INVALID_TYPE_LABS = \
            'Config expected type object for key "%s": %s' \
            % (self.key, config.filename)

        parse_obj = '%s["%s"]["%s"]' % (config.filename, self.key, '%s')
        self.ERROR_INVALID_TYPE_ENTRY = \
            'Lab entries must be of type string: ' + parse_obj
        self.ERROR_FMT = \
            'Lab entries must be in the format ' \
            '"<weekday> <start_time> - <end_type>": ' + parse_obj

        if self.key not in config:
            raise ValidationError(self.ERROR_NO_LABS)

        if not isinstance(config[self.key], dict):
            raise ValidationError(self.ERROR_INVALID_TYPE_LABS)

        self.parse(config[self.key])

    def parse(self, labs):
        for letter, entry in labs.items():
            if not isinstance(entry, str):
                raise ValidationError(self.ERROR_INVALID_TYPE_ENTRY % letter)

            mo = re.search('(\S+)\s*(\S+)\s*-\s*(\S+)', entry)
            if not mo:
                raise ValidationError(self.ERROR_FMT % letter)

            weekday_, start_, end_ = mo.groups()

            weekday = parse_weekday(weekday_)
            if weekday is None:
                raise ValidationError(self.ERROR_WEEKDAY_FMT % letter)

            start = parse_time(start_)
            if start is None:
                raise ValidationError(self.ERROR_TIME_FMT % letter)

            end = parse_time(end_)
            if end is None:
                raise ValidationError(self.ERROR_TIME_FMT % letter)

            self[letter.upper()] = LabSesh(weekday, start, end)

    def pretty_grid(self):
        for letter, sesh in self.items():
            yield (letter, *sesh.get_tuple())


class Roster(dict):
    key = 'roster'

    def __init__(self, config, session_letters):
        self.ERROR_NO_ROSTER = \
            'Config must specify a roster object: ' + config.filename
        self.ERROR_INVALID_TYPE_ROSTER = \
            'Config expected type object for key "%s": %s' \
            % (self.key, config.filename)

        parse_obj = '%s["%s"]["%s"]' % (config.filename, self.key, '%s')
        self.ERROR_INVALID_TYPE_ENTRY = \
            'Roster entries must be of type string: ' + parse_obj
        self.ERROR_UNKNOWN_LETTER = \
            'Lab session letter not recognized: ' + parse_obj

        self.session_letters = list(session_letters)

        if self.key not in config:
            raise ValidationError(self.ERROR_NO_ROSTER)

        if not isinstance(config[self.key], dict):
            raise ValidationError(self.ERROR_INVALID_TYPE_ROSTER)

        self.parse(config[self.key])

    def parse(self, roster):
        for pawprint, letter in roster.items():
            if not isinstance(letter, str):
                raise ValidationError(self.ERROR_INVALID_TYPE_ENTRY % pawprint)

            letter = letter.upper()

            if letter not in self.session_letters:
                raise ValidationError(self.ERROR_UNKNOWN_LETTER % pawprint)

            self[pawprint.lower()] = letter.upper()


class CourseConfig(dict):
    def __init__(self, fn):
        ERROR_INVALID_TYPE_ROOT = \
            'Config expected root to be an object: ' + fn
        ERROR_NO_COURSE_NUM = \
            'Configs must specify a course_number: ' + fn
        ERROR_NO_ADMIN_HASH = \
            'Configs must specify an admin_hash: ' + fn

        self.filename = fn

        try:
            with open(fn, 'r') as f:
                data = json.load(f)
        except json.JSONDecodeError:
            raise ValidationError('Invalid json: ' + fn)

        if not isinstance(data, dict):
            raise ValidationError(ERROR_INVALID_TYPE_ROOT)

        if 'course_number' not in data:
            raise ValidationError(ERROR_NO_COURSE_NUM)

        if 'admin_hash' not in data:
            raise ValidationError(ERROR_NO_ADMIN_HASH)

        super().__init__(data)

        self['course_number'] = data['course_number']
        self['admin_hash'] = data['admin_hash']
        self['homeworks'] = Homeworks(self)
        self['labs'] = LabSessions(self)
        self['current_lab'] = data.get('current_lab')
        self['roster'] = Roster(self, self['labs'].keys())

    def get_current_hw(self, suppress=False):
        now = datetime.datetime.now()
        homeworks = sorted(
            self['homeworks'].items(),
            key=lambda hw: hw[1])

        for name, duedate in homeworks:
            if now < duedate:
                return name

        if not suppress:
            # No hw found
            raise SubcommandError(
                'No open homework assignments for course: ' + course)

    def get_current_lab(self, suppress=False):
        current_lab = self['current_lab']

        if not suppress:
            # No current_lab in config
            if current_lab is None:
                raise SubcommandError(
                    'No open labs for course: ' + self['course_number'])

        return current_lab


class Configs(dict):
    def __init__(self, config_dir):
        if not os.path.exists(config_dir):
            die('Config path does not exist:', config_dir)
        if not os.path.isdir(config_dir):
            die('Config path must be a directory:', config_dir)

        config_filenames = []
        for dirpath, dirnames, filenames in os.walk(config_dir):
            for fn in filenames:
                path = os.path.join(dirpath, fn)
                ext = os.path.splitext(path)[1]
                if ext == '.json':
                    config_filenames.append(path)

        for fn in config_filenames:
            cfg = CourseConfig(fn)
            course_num = cfg['course_number']
            self[course_num] = cfg

    def get_config(self, course):
        if course not in self.keys():
            raise SubcommandError('Course not recognized: ' + course)
        return self[course]
