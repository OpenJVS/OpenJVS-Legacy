from setuptools import setup

setup(
	name = 'JVSE',
	version = '0.1.1',
	description = 'Jamma Video System Emulator libraries for Python',
	author = 'Bobby Dilley',
	author_email = 'bobby@dilley.io',
	url = 'https://github.com/bobbydilley/JVSEmulator',
	packages = ['JVSE', 'JVSE.Modules'],
	entry_points = {
		'console_scripts': [
			'JVSE = JVSE.__main__:main',
			'jvse = JVSE.__main__:main'
		]
	},
	install_requires = ['pyserial', 'pygame']
)
