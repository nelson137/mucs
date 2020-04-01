import datetime
import unittest

from mucs.config import LabSesh
from test_utils import NOW


class TestLabSesh(unittest.TestCase):

    def setUp(self):
        self.labsesh = LabSesh(
            weekday=NOW.weekday(),
            start=datetime.time(),
            end=datetime.time(23, 59, 59, 999999))

    def test_is_active_true(self):
        self.assertTrue(self.labsesh._is_active())

    def test_is_active_false(self):
        # If current hour is after noon
        if NOW.time().hour > 12:
            # Move end time hour to noon
            self.labsesh = self.labsesh._replace(
                end=self.labsesh.end.replace(hour=12))
        # If current hour is noon or before
        else:
            # Move start time to after noon
            self.labsesh = self.labsesh._replace(
                start=self.labsesh.start.replace(hour=13))

        self.assertFalse(self.labsesh._is_active())
