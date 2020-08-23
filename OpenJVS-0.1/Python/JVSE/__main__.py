import sys, os
from .JVSE import JVSE
from os.path import expanduser

def main():
	home = expanduser("~") + "/JVSE/Modules"

	path = os.path.dirname(os.path.abspath(__file__)) + '/Modules'
	jvse_path = os.path.dirname(os.path.abspath(__file__)) + '/JVSE.py'


	print "Jamma Video System Emulator"
	print "By Bobby Dilley"
	print "Version: Alpha"
	print "Config Location:", jvse_path

	if not os.path.exists(path):
		print "Error: Couldn't find internal modules"
	else:
		print "Modules Location:", path


if __name__ == '__main__':
	main()
