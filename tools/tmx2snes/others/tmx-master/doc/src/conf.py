# -*- coding: utf-8 -*-
# For ReadTheDocs.org
import sys
import os
import shlex
from datetime import date
import re
import fileinput

extensions = ['sphinx_tabs.tabs']
source_suffix = '.rst'
master_doc = 'index'

project = u'libTMX'
copyright = u'2019-' + str(date.today().year) + u', libTMX documentation authors'

version = 'dev'
release = 'dev'
pattern = re.compile(r"^project ?\(\w+ +VERSION +([.0-9]+)(?: .*)\)")
with fileinput.input(files=('../../CMakeLists.txt')) as f:
	for line in f:
		match = pattern.match(line)
		if (match):
			release = match.group(1)
			print("Found project version: " + release)
			break

if (release is 'dev'):
	print('Could not find project version')
else:
	release_split = release.split('.')
	version = release_split[0] + '.' + release_split[1];
	print('Short version is: ' + version);

language = None
pygments_style = 'sphinx'
todo_include_todos = False
html_theme = 'sphinx_rtd_theme'
