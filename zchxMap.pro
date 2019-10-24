#-------------------------------------------------
#
# Project created by QtCreator 2018-12-03T13:46:36
#
#-------------------------------------------------

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = zchx_ecdis app

QMAKE_CXXFLAGS_RELEASE *= $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE *= $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
