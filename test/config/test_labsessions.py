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
        t = NOW.time()
        w = weekday_to_str(NOW.weekday())
        s = t.strftime(TIME_FMT)
        e = t.replace(hour=t.hour+2, minute=t.minute+30).strftime(TIME_FMT)
        self.data['labs'] = {random_char(): '%s %s - %s' % (w, s, e)}
        mock_config = MockCourseConfig(self.data)

        labs = LabSessions(mock_config)
