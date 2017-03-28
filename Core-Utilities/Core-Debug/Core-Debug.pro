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
PRECOMPILED_HEADER = StdAfx.h

#Lib specific configuration
TARGET = Core-Debug
TEMPLATE = lib
QT += core

#Read Version from VERSION.h
VERSION = $$system(grep VERSION VERSION.h | cut -d \' \' -f 3 | tr -d \'\"\')
message(compile Core-Utilities_Debug version: $$VERSION)

#Lib compile infos
DEFINES += COREDEBUG_LIB

HEADERS += ./stdafx.h \
	./Core-Debug.h \
    	./Core-Debug_Global.h

SOURCES += ./stdafx.cpp \
    	./Core-Debug.cpp

# built-in install
target.path = $${INSTALL_ROOT}/$${INSTDIR}/lib
INSTALLS += target
