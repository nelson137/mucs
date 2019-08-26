import sys


class Printer:
    __init_blank = False
    __contexts = 0

    def __enter__(self):
        if not Printer.__init_blank:
            Printer.__init_blank = True
            print()
        Printer.__contexts += 1

    def __exit__(self, *args):
        if Printer.__contexts == 1:
            print()
        Printer.__contexts -= 1


def printline(*args, **kwargs):
    with Printer():
        print(*args, **kwargs)
