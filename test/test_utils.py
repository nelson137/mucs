import datetime
import random
import string
import tempfile

NOW = datetime.datetime.now()


def random_string(n=10, hex=False):
    if hex:
        chars = string.ascii_lowercase[:6] + string.digits
    else:
        chars = string.ascii_lowercase + string.digits
    return ''.join(random.choice(chars) for i in range(n))


def random_number(digits=4):
    s = str(random.randint(1, 9))
    for n in range(digits - 1):
        s += str(random.randint(0, 9))
    return int(s)


class MockCourseConfig(dict):
    def __init__(self, data, fn=None):
        super().__init__(data)
        if fn is None:
            self.filename = random_string(n=4) + '.json'
        else:
            self.filename = fn


class TempFile:
    def __init__(self):
        self._f = tempfile.NamedTemporaryFile(mode='w', suffix='.mucs')
        self.name = self._f.name

    def __enter__(self):
        self._f.__enter__()
        return self

    def __exit__(self, *args, **kwargs):
        self._f.__exit__(*args, **kwargs)

    def write(self, data):
        self._f.write(data)
        self._f.flush()
