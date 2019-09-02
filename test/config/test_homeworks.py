import unittest

from mucs.config import Homeworks
from mucs.consts import *
from mucs.exc import MucsError
from test_utils import *


class TestHomeworks(unittest.TestCase):

    def setUp(self):
        self.data = {
            'course_number': str(random_number(4)),
            'admin_hash': random_string(n=32, hex=True)
        }

    def test_nonobj_homeworks(self):
        self.data['homeworks'] = random_number()
        mock_config = MockCourseConfig(self.data)

        msg_re = r'^Config expected type object for key "homeworks": .*$'
        self.assertRaisesRegexp(MucsError, msg_re, Homeworks, mock_config)

    def test_empty_homeworks(self):
        self.data['homeworks'] = {}
        mock_config = MockCourseConfig(self.data)

        hws = Homeworks(mock_config)
        self.assertDictEqual(hws, {})

    def test_nonstr_duedate(self):
        self.data['homeworks'] = {'hw1': random_number()}
        mock_config = MockCourseConfig(self.data)

        msg_re = r'^Homework entries must be of type string: .*$'
        self.assertRaisesRegexp(MucsError, msg_re, Homeworks, mock_config)

    def test_invalid_duedate(self):
        self.data['homeworks'] = {'hw1': random_string()}
        mock_config = MockCourseConfig(self.data)

        msg_re = r'^Homework entries must be in the format ' \
                 r'"yyyy-mm-dd hh:mm:ss": .*$'
        self.assertRaisesRegexp(MucsError, msg_re, Homeworks, mock_config)

    def test_valid_homeworks(self):
        self.data['homeworks'] = {'hw1': NOW.strftime(HOMEWORK_FMT)}
        mock_config = MockCourseConfig(self.data)

        hws = Homeworks(mock_config)
