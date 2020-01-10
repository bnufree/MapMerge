include(../common.pri)

message("new destination:"+$${IDE_APP_PATH})

defineReplace(qtLibraryName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
              else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

#根据不同的编译清空生成不同的输出路径
CONFIG(release, debug|release) {
  DEFINES *= RELEASE _RELEASE NDEBUG QT_MESSAGELOGCONTEXT
  CONFIG_NAME = Release
} else {
  DEFINES *= DEBUG _DEBUG
  CONFIG_NAME = Debug
  EXT_NAME = d
}

include($$ZCHX_ECDIS_3RDPARTY/protobuf/protobuf.pri)
include($$ZCHX_ECDIS_3RDPARTY/ZeroMQ/zmq.pri)


QT += core
QT -= gui

CONFIG += c++11

TARGET = zchxReceiver
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ZCHXRadar.pb.cc \
    zchxradardatachange.cpp \
    zchxradarechothread.cpp \
    zchxradarpointthread.cpp \
    zchxradarrectthread.cpp \
    ZCHXRadarVideo.pb.cc \
    profiles.cpp

DISTFILES += \
    ZCHXRadar.proto \
    ZCHXRadarVideo.proto

HEADERS += \
    ZCHXRadar.pb.h \
    zchxradardatachange.h \
    zchxradarechothread.h \
    zchxradarpointthread.h \
    zchxradarrectthread.h \
    zchxradarutils.h \
    ZCHXRadarVideo.pb.h \
    profiles.h
