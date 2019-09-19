import datetime
import unittest

from mucs.config import LabSessions
from mucs.consts import *
from mucs.exc import MucsError
from mucs.util import weekday_to_str
from test_utils import *


class TestLabSessions(unittest.TestCase):

    def setUp(self):
        self.data = {
            'course_number': str(random_number(4)),
            'admin_hash': random_string(n=32, hex=True)
        }

    def test_nonobj_labsessions(self):
        self.data['labs'] = random_number()
        mock_config = MockCourseConfig(self.data)

        msg_re = r'^Config expected type object for key "labs": .*$'
        self.assertRaisesRegexp(MucsError, msg_re, LabSessions, mock_config)

    def test_empty_labsessions(self):
        self.data['labs'] = {}
        mock_config = MockCourseConfig(self.data)

        labs = LabSessions(mock_config)
        self.assertDictEqual(labs, {})

    def test_nonstr_session_time(self):
        self.data['labs'] = {random_char(): random_number()}
        mock_config = MockCourseConfig(self.data)

        msg_re = r'^Lab entries must be of type string: .*$'
        self.assertRaisesRegexp(MucsError, msg_re, LabSessions, mock_config)

    def test_invalid_duedate(self):
        self.data['labs'] = {random_char(): random_string()}
        mock_config = MockCourseConfig(self.data)

        msg_re = r'^Lab entries must be in the format ' \
                 r'"<weekday> <start_time> - <end_time>": .*$'
        self.assertRaisesRegexp(MucsError, msg_re, LabSessions, mock_config)

    def test_valid_labsessions(self):
        letter = random_char().upper()
        time = NOW.time()
        weekday = NOW.weekday()
        weekday_f = weekday_to_str(weekday)
        start = time.replace(microsecond=0)
        start_f = start.strftime(TIME_FMT)
        end = time.replace(hour=(time.hour+1) % 24, microsecond=0)
        end_f = end.strftime(TIME_FMT)

        self.data['labs'] = {
            letter: '%s %s - %s' % (weekday_f, start_f, end_f)}
        mock_config = MockCourseConfig(self.data)

        labs = LabSessions(mock_config)

        self.assertIn(letter, labs)
        self.assertEqual(labs[letter].weekday, weekday)
        self.assertEqual(labs[letter].start, start)
        self.assertEqual(labs[letter].end, end)
