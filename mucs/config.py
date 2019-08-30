# vim: foldmethod=marker

# Imports {{{

import datetime
import json
import os
import re
from collections import namedtuple

from consts import *
from exc import MucsError
from util import *

# }}}


class Homeworks(dict):
    key = 'homeworks'

    def __init__(self, config):
        self.parse_obj = config.filename + ('["%s"]' % self.key) + '["%s"]'

        if self.key not in config:
            raise MucsError(
                'Config must specify a homeworks object',
                reason=config.filename)

        if not isinstance(config[self.key], dict):
            raise MucsError(
                'Config expected type object for key "%s"' % self.key,
                reason=config.filename)

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
                raise MucsError(
                    'Homework entries must be of type string',
                    reason=self.parse_obj % name)

            try:
                self[name.lower()] = self.parse_duedate(duedate)
            except ValueError:
                raise MucsError(
                    'Homework entries must be in the format '
                    '"yyyy-mm-dd hh:mm:ss"',
                    reason=self.parse_obj % name)


class LabSesh(namedtuple('LabSesh', 'weekday start end')):
    def get_tuple(self):
        weekday = weekday_to_str(self.weekday)
        start = self.start.strftime(TIME_PRETTY_FMT)
        end = self.end.strftime(TIME_PRETTY_FMT)
        return (weekday, start, end)

    def is_active(self):
        now = datetime.datetime.now()
        if now.weekday() != self.weekday:
            return False
        return self.start <= now.time() <= self.end


class LabSessions(dict):
    key = 'labs'

    def __init__(self, config):
        self.parse_obj = config.filename + ('["%s"]' % self.key) + '["%s"]'

        if self.key not in config:
            raise MucsError(
                'Config must specify a labs object',
                reason=config.filename)

        if not isinstance(config[self.key], dict):
            raise MucsError(
                'Config expected type object for key "%s"' % self.key,
                reason=config.filename)

        self.parse(config[self.key])

    def parse(self, labs):
        for letter, entry in labs.items():
            if not isinstance(entry, str):
                raise MucsError(
                    'Lab entries must be of type string',
                    reason=self.parse_obj % letter)

            mo = re.search('(\S+)\s*(\S+)\s*-\s*(\S+)', entry)
            if not mo:
                raise MucsError(
                    'Lab entries must be in the format '
                    '"<weekday> <start_time> - <end_type>"',
                    reason=self.parse_obj % letter)

            weekday_, start_, end_ = mo.groups()

            weekday = parse_weekday(weekday_)
            if weekday is None:
                raise MucsError(
                    'Lab entry invalid weekday',
                    reason=self.parse_obj % letter)

            start = parse_time(start_)
            if start is None:
                raise MucsError(
                    'Lab entry invalid start time',
                    reason=self.parse_obj % letter)

            end = parse_time(end_)
            if end is None:
                raise MucsError(
                    'Lab entry invalid end time',
                    reason=self.parse_obj % letter)

            self[letter.upper()] = LabSesh(weekday, start, end)

    def pretty_grid(self):
        for letter, sesh in self.items():
            yield (letter, *sesh.get_tuple())


class Roster(dict):
    key = 'roster'

    def __init__(self, config, session_letters):
        self.parse_obj = '%s["%s"]["%s"]' % (config.filename, self.key, '%s')

        self.session_letters = list(session_letters)

        if self.key not in config:
            raise MucsError(
                'Config must specify a roster object',
                reason=config.filename)

        if not isinstance(config[self.key], dict):
            raise MucsError(
                'Config expected type object for key "%s"' % self.key,
                reason=config.filename)

        self.parse(config[self.key])

    def parse(self, roster):
        for pawprint, letter in roster.items():
            if not isinstance(letter, str):
                raise MucsError(
                    'Roster entries must be of type string',
                    reason=self.parse_obj % pawprint)

            letter = letter.upper()

            if letter not in self.session_letters:
                raise MucsError(
                    'Lab session letter not recognized',
                    reason=self.parse_obj % pawprint)

            self[pawprint.lower()] = letter.upper()


class CourseConfig(dict):
    def __init__(self, fn):
        self.filename = fn

        try:
            with open(fn, 'r') as f:
                data = json.load(f)
        except json.JSONDecodeError:
            raise MucsError('Invalid json: ' + fn)

        if not isinstance(data, dict):
            raise MucsError('Config expected root to be an object', reason=fn)

        if 'course_number' not in data:
            raise MucsError('Configs must specify a course_number', reason=fn)

        if 'admin_hash' not in data:
            raise MucsError('Configs must specify an admin_hash', reason=fn)

        super().__init__(data)

        self['course_number'] = data['course_number']
        self['admin_hash'] = data['admin_hash']
        self['homeworks'] = Homeworks(self)
        self['labs'] = LabSessions(self)
        self['current_lab'] = data.get('current_lab')
        self['roster'] = Roster(self, self['labs'].keys())

    def get_current_hw(self):
        now = datetime.datetime.now()
        homeworks = sorted(
            self['homeworks'].items(),
            key=lambda hw: hw[1])

        for name, duedate in homeworks:
            if now < duedate:
                return name

        return None

    def get_current_lab(self):
        current_lab = self['current_lab']
        return current_lab


class Configs(dict):
    def __init__(self, config_dir):
        if not os.path.exists(config_dir):
            raise MucsError(
                'Config directory does not exist', reason=config_dir)
        if not os.path.isdir(config_dir):
            raise MucsError(
                'Config path must be a directory', reason=config_dir)

        for dirpath, dirnames, filenames in os.walk(config_dir):
            for fn in filenames:
                base, ext = os.path.splitext(fn)
                if ext == '.json':
                    cfg = CourseConfig(os.path.join(dirpath, fn))
                    num = cfg['course_number']
                    self[num] = cfg

    def get_config(self, course):
        if course not in self.keys():
            raise MucsError('Course not recognized', reason=course)
        return self[course]
