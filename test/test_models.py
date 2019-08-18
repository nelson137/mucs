import unittest


class TestHomeworks(unittest.TestCase):

    def test_parse_no_name(self):
        with TempFile() as tf:
            json.dump([{'duedate': NOW}], tf)

            re = r'^Homework objects must specify a name:.*$'
            try:
                with self.assertRaisesRegex(ValidationError, re):
                    Homeworks(tf.name)
            except ValidationError:
                pass
            except Exception:
                self.fail('a ValidationError for no name should have been '
                          'raised')

    def test_parse_no_duedate(self):
        with TempFile() as tf:
            json.dump([{'name': random_string()}], tf)

            re = r'^Homework objects must specify a duedate:.*$'
            try:
                with self.assertRaisesRegex(ValidationError, re):
                    Homeworks(tf.name)
            except ValidationError:
                pass
            except Exception:
                self.fail('a ValidationError for no duedate should have been '
                          'raised')

    def test_parse_duplicate_name(self):
        with TempFile() as tf:
            name = random_string()
            json.dump([
                {'name': name, 'duedate': NOW},
                {'name': name, 'duedate': NOW}
            ], tf)

            re = r'^Homework object names must be unique:.*$'
            try:
                self.assertRaisesRegex(ValidationError, re, Homeworks, tf.name)
            except ValidationError:
                pass
            except Exception:
                self.fail('a ValidationError for duplicate names should have '
                          'been raised')

    def test_parse_good(self):
        with TempFile() as tf:
            name = random_string()
            duedate = NOW
            data = [{'name': name, 'duedate': NOW}]

            json.dump(data, tf)

            try:
                hws = Homeworks(tf.name)
            except Exception:
                self.fail('no exception should have been thrown')

            expected_hws = {hw['name']: parse_date(hw['duedate'])
                            for hw in data}
            self.assertDictEqual(hws, expected_hws)

    def test_dump(self):
        with TempFile() as tf:
            name = random_string()
            data = [{'name': name, 'duedate': NOW}]

            json.dump(data, tf)

            try:
                hws = Homeworks(tf.name)
            except Exception:
                self.fail('no exception should have been thrown')

            expected_dump = "{'%s': '%s'}" % (name, str(NOW))
            self.assertEqual(str(hws), expected_dump)


class TestLabSessions(unittest.TestCase):

    def test_parse_(self):
        # TODO
        self.assertTrue(True)
