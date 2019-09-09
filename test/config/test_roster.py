import unittest

from mucs.config import Roster
from mucs.exc import MucsError
from test_utils import *


class TestRoster(unittest.TestCase):

    def setUp(self):
        self.data = {
            'course_number': str(random_number(4)),
            'admin_hash': random_string(n=32, hex=True)
        }

    def test_nonobj_roster(self):
        self.data['roster'] = random_number()
        mock_config = MockCourseConfig(self.data)

        msg_re = r'^Config expected type object for key "roster": .*$'
        self.assertRaisesRegexp(MucsError, msg_re, Roster, mock_config, [])

    def test_nonstr_letter(self):
        self.data['roster'] = {random_string(n=6): random_number()}
        mock_config = MockCourseConfig(self.data)

        msg_re = r'^Roster entries must be of type string: .*$'
        self.assertRaisesRegexp(MucsError, msg_re, Roster, mock_config, [])

    def test_unrecognized_letter(self):
        self.data['roster'] = {random_string(n=6): random_char()}
        mock_config = MockCourseConfig(self.data)

        msg_re = r'^Lab session letter not recognized: .*$'
        self.assertRaisesRegexp(MucsError, msg_re, Roster, mock_config, [])

    def test_valid_roster(self):
        user = random_string(n=6)
        letter = random_char().upper()
        self.data['roster'] = {user: letter}
        mock_config = MockCourseConfig(self.data)

        roster = Roster(mock_config, [letter])

        self.assertIn(user, roster)
        self.assertEqual(roster[user], letter)
