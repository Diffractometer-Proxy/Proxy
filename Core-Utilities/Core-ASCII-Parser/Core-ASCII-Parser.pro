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
TARGET = Core-ASCII-Parser
TEMPLATE = lib
QT += core

#Read Version from VERSION.h
VERSION = $$system(grep VERSION VERSION.h | cut -d \' \' -f 3 | tr -d \'\"\')
message(compile Core-Utilities_ASCII-Parser version: $$VERSION)

#Lib compile infos
INCLUDEPATH += ../Core-Debug

HEADERS += stdafx.h \
    Core-ASCII-Parser.h \
    Core-ASCII-Parser_Global.h
SOURCES += stdafx.cpp \
    Core-ASCII-Parser.cpp

# built-in install
target.path = $${INSTALL_ROOT}/$${INSTDIR}/lib
INSTALLS += target
