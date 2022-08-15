#!/usr/bin/env python

import codecs, errno, os, re
from datetime import datetime

dir_root = os.path.normpath(os.path.join(os.path.dirname(__file__), "../"))


def _readLines(infile):
	buffer = codecs.open(infile, "r", "utf-8")
	lines = buffer.read().replace("\r\n", "\n").replace("\r", "\n").split("\n")
	buffer.close()

	return lines


def _writeLines(outfile, l_new, l_old=None):
	if l_new != l_old:
		buffer = codecs.open(outfile, "w", "utf-8")
		buffer.write("\n".join(l_new))
		buffer.close()

		print("Updated copyright in {}".format(outfile))


# def updateLicense():
def _updateFile(filepath):
	#file_lic = os.path.join(dir_root, "LICENSE.txt")
	if not os.path.isfile(filepath):
		print("ERROR: License text not found: {}".format(filepath))
		return errno.ENOENT

	old_lines = _readLines(filepath)

	new_lines = list(old_lines)
	for idx in range(len(new_lines)):
		line = new_lines[idx]
		if re.search(r"Copyright (?:©|\([cC]\)?) .*? ", line):
			line = re.sub(r"Copyright (?:©|\([cC]\)?) (.*?) (.*$)", r"Copyright © 2010-{} \2".format(datetime.now().year), line)

			new_lines[idx] = line

			# files shouldn't have more than one copyright line
			break

	_writeLines(filepath, new_lines, old_lines)


def main():
	file_lic = os.path.join(dir_root, "LICENSE.txt")
	if os.path.isfile(file_lic):
		_updateFile(file_lic)

	dir_include = os.path.join(dir_root, "include")
	dir_src = os.path.join(dir_root, "src")

	for ROOT, DIRS, FILES in os.walk(dir_include):
		for f in FILES:
			filepath = os.path.join(ROOT, f)
			if filepath.endswith(".h") and os.path.isfile(filepath):
				_updateFile(filepath)

	for ROOT, DIRS, FILES in os.walk(dir_src):
		for f in FILES:
			filepath = os.path.join(ROOT, f)
			if filepath.endswith(".cpp") and os.path.isfile(filepath):
				_updateFile(filepath)


if __name__ == "__main__":
	main()
