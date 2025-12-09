#!/usr/bin/env python3

# Copyright 2020 Piotr Wiszowaty
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import argparse
import datetime
import os
import re
import sys


class FileNotFoundException(Exception):
    def __init__(self, fname):
        self.fname = fname

    def __str__(self):
        return f"file not found: {self.fname}"


parser = argparse.ArgumentParser()
parser.add_argument("file", metavar="FILE", help="file to send")
parser.add_argument("-c", "--skip-comments", action="store_true", default=False, help="skip sending comments")
parser.add_argument("-e", "--skip-empty-lines", action="store_true", default=False, help="skip sending empty lines")
parser.add_argument("-I", "--include-path", metavar="DIR", action="append", default=["."], help="append directory to include paths")
parser.add_argument("-s", "--print-statistics", action="store_true", default=False, help="print transfer statistics")
args = parser.parse_args()

included = []

rc = re.compile(r"\s+(\\ .*)")  # remove \ comment at end of line
rc2 = re.compile(r" \( .+ \)")  # remove ( ) comment in line


def xfr(parent_fname, parent_lineno, fname):
    lineno = 0
    n_bytes_sent = 0
    try:
        try:
            paths = filter(lambda p: os.path.exists(p), map(lambda d: os.path.join(d, fname), args.include_path))
            fpath = next(paths)
        except StopIteration:
            raise FileNotFoundException(fname)
        for line in open(fpath, "rt"):
            lineno += 1
            if line.startswith("\\ #include ") or line.startswith("#require ") or line.startswith("#include "):
                if line.startswith("#require "):
                    fn = line[9:].strip()
                elif line.startswith("#include "):
                    fn = line[9:].strip()
                else:
                    fn = line[11:].strip()

                if fn not in included:
                    sys.stderr.write(f"*** Including {fn} ***\n")
                    included.append(fn)
                    n_bytes_sent += xfr(fname, lineno, fn)
                else:
                    sys.stderr.write(f"*** {fn}: skipped as already included ***\n")
            elif args.skip_comments and re.match(r"^\s*\\\s.*", line):
                continue
            elif args.skip_empty_lines and re.match(r"^\s*$", line):
                continue
            else:
                if args.skip_comments:
                    line = rc.sub('', line)
                    line = rc2.sub('', line)
                sys.stdout.write(line)
                n_bytes_sent += len(line)
                response = sys.stdin.readline()
                sys.stderr.write(response)
                if not (response.endswith("ok.\n") or response.endswith("ok'\x1B[0m\n") or response.endswith("ok.\x1B[0m\n")):
                    sys.stderr.write(f"*** {fname}({lineno}): compilation error ***\n")
                    sys.exit(1)
        return n_bytes_sent
    except (OSError, FileNotFoundException) as e:
        if parent_fname is not None:
            sys.stderr.write(f"*** {parent_fname}({parent_lineno}): {e} ***\n")
        else:
            sys.stderr.write(f"*** {fname}: {e} ***\n")
        sys.exit(1)


t0 = datetime.datetime.now()
n = xfr(None, 0, args.file)
elapsed_time = datetime.datetime.now() - t0
speed = int(n / elapsed_time.total_seconds())
if args.print_statistics:
    sys.stderr.write(f"*** elapsed time: {elapsed_time} ({speed} bytes/s) ***\n")
