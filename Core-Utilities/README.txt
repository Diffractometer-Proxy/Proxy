Dependencies:

	- Qt

The Core-Utilities needs only Qt. It was tested with Qt4 and Qt5.
The VS project files are currently configured for Qt5.

Environment  Variables:
	Linux:
		----
	Windows:
		QTDIR => Qt Path

Install:
	Windows:
		Please create the setup.exe from the VS project and use this setup file to install this lib at other pc's.
	Linux:
		Please edit the Core-Utilities_Build.pri file to change the installation folder. Use after the compilation make install to install the lib.
		Attention you will need superuser rights. 
