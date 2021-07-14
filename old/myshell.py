import os
import subprocess
import sys

import argparse
parser = argparse.ArgumentParser()
# parser can have any arguments, whatever you want!

def parse_command(command):
    parsed, unknown = parser.parse_known_args([command]) # this is an 'internal' method
    # which returns 'parsed', the same as what parse_args() would return
    # and 'unknown', the remainder of that
    # the difference to parse_args() is that it does not exit when it finds redundant arguments

    for arg in unknown:
        print(arg)
        if arg.startswith(("-", "--")):
            # you can pass any arguments to add_argument
            parser.add_argument(arg.split('=')[0], type=str)

    args = parser.parse_args(command)

def run_command(command):
    if command == 'logout':
        exit(0)

    try:
        r = subprocess.run(command.split(' '), stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=os.environ)
        print(r.stdout.decode('utf-8'), end='')
        print(r.stderr.decode('utf-8'), file=sys.stderr, end='')
    except Exception as e:
        print(e)

while True:
    try:
        command = input()
    except EOFError:
        break
    run_command(command)