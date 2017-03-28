#######################################################
# written by Christian Randau
# Mail: christian.randau@frm2.tum.de
#######################################################

include(../Core-Utilities_Build.pri)

DESTDIR = ../$${COMPILEDIR}
CONFIG += $${PROJECT_CONFIG}

# Lib standard configuration
MOC_DIR = ./GeneratedFiles/release
UI_DIR = ./GeneratedFiles
RCC_DIR = ./GeneratedFiles
OBJECTS_DIR = ./obj

#Lib specific configuration
TARGET = Core-Utilities
TEMPLATE = lib

core-utilities_install-include.path    = $${INSTALL_ROOT}/$${INSTDIR}/include
core-utilities_install-include.files  += ../Core-ASCII-Parser/Core-ASCII-Parser.h
core-utilities_install-include.files  += ../Core-ASCII-Parser/Core-ASCII-Parser_Global.h 
core-utilities_install-include.files  += ../Core-Debug/Core-Debug.h
core-utilities_install-include.files  += ../Core-Debug/Core-Debug_Global.h
core-utilities_install-include.files  += ../Core-Persistence/Core-Persistence.h
INSTALLS += core-utilities_install-include

# create profile.d script for the environment variables
unix{
	system(mkdir -p $${DESTDIR}/script)
	system(rm -f $${DESTDIR}/script/Core-Utilities.sh)
	system(touch $${DESTDIR}/script/Core-Utilities.sh)
	system(sh -c "'echo echo create eviroment variable COREUTILITIESDIR >> $${DESTDIR}/script/Core-Utilities.sh'")
	system(sh -c "'echo echo put COREUTILITIESDIR/lib into the path variable >> $${DESTDIR}/script/Core-Utilities.sh'")
	system(sh -c "'echo export COREUTILITIESDIR=$${INSTDIR} >> $${DESTDIR}/script/Core-Utilities.sh'")
	system(sh -c "'echo export PATH=\\\$PATH:\\\$COREUTILITIESDIR/lib >> $${DESTDIR}/script/Core-Utilities.sh'")
	system(sh -c "'echo export LD_LIBRARY_PATH=\\\$LD_LIBRARY_PATH:\\\$COREUTILITIESDIR/lib >> $${DESTDIR}/script/Core-Utilities.sh'")
}

unix {
	core-utilities_install-profile.path = $${INSTALL_ROOT}/etc/profile.d
	core-utilities_install-profile.files += $${DESTDIR}/script/Core-Utilities.sh
}
macx {
	core-utilities_install-profile.path = $${INSTALL_ROOT}./
	core-utilities_install-profile.commands += @echo create eviroment variable COREUTILITIESDIR and put COREUTILITIESDIR/bin into the path variable.
}
INSTALLS += core-utilities_install-profile

