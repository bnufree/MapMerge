#-------------------------------------------------
#
# Project created by QtCreator 2018-12-03T13:46:36
#
#-------------------------------------------------

include(../common.pri)

#defineReplace(qtLibraryName) {
#   unset(LIBRARY_NAME)
#   LIBRARY_NAME = $$1
#   CONFIG(debug, debug|release) {
#      !debug_and_release|build_pass {
#          mac:RET = $$member(LIBRARY_NAME, 0)_debug
#              else:win32:RET = $$member(LIBRARY_NAME, 0)d
#      }
#   }
#   isEmpty(RET):RET = $$LIBRARY_NAME
#   return($$RET)
#}
##根据不同的编译清空生成不同的输出路径
#CONFIG(release, debug|release) {
#  DEFINES *= RELEASE _RELEASE _DEBUG QT_MESSAGELOGCONTEXT
#  CONFIG_NAME = Release
#} else {
#  DEFINES *= DEBUG _DEBUG
#  CONFIG_NAME = Debug
#  EXT_NAME = d
#}

#CONFIG += ordered
#SUBDIRS = Libs


#OPENCPN_3RD_PATH = $${PWD}/3rdparty
#INCLUDEPATH += $${OPENCPN_3RD_PATH}/include
#OPENCPN_3RD_STATIC_LIB_PATH = $${OPENCPN_3RD_PATH}/lib
#INCLUDEPATH += $${OPENCPN_3RD_INCLUDE_PATH}

#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}/$${CONFIG_NAME}\ -lgdal$${EXT_NAME}
#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}/$${CONFIG_NAME}\ -lnmea0183$${EXT_NAME}
#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}/$${CONFIG_NAME}\ -ltinyxml$${EXT_NAME}
#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}/$${CONFIG_NAME}\ -lsound$${EXT_NAME}
#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}/$${CONFIG_NAME}\ -liso8211$${EXT_NAME}
#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}/$${CONFIG_NAME}\ -lSQLiteCPP$${EXT_NAME}
#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}/$${CONFIG_NAME}\ -lssl_sha1$${EXT_NAME}
#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}/$${CONFIG_NAME}\ -ltexcmp$${EXT_NAME}
#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}/$${CONFIG_NAME}\ -llz4$${EXT_NAME}
#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}/$${CONFIG_NAME}\ -ltess2$${EXT_NAME}
#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}/$${CONFIG_NAME}\ -lmipmap$${EXT_NAME}
#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}\ -llzma
#LIBS += -L$${OPENCPN_3RD_STATIC_LIB_PATH}\ -lsqlite3
#LIBS += -lpsapi

LIBS += -L$${DESTDIR}\ -lgdal$${EXT_NAME}
LIBS += -L$${DESTDIR}\ -lnmea0183$${EXT_NAME}
LIBS += -L$${DESTDIR}\ -ltinyxml$${EXT_NAME}
LIBS += -L$${DESTDIR}\ -lsound$${EXT_NAME}
LIBS += -L$${DESTDIR}\ -liso8211$${EXT_NAME}
LIBS += -L$${DESTDIR}\ -lSQLiteCPP$${EXT_NAME}
LIBS += -L$${DESTDIR}\ -lssl_sha1$${EXT_NAME}
LIBS += -L$${DESTDIR}\ -ltexcmp$${EXT_NAME}
LIBS += -L$${DESTDIR}\ -llz4$${EXT_NAME}
LIBS += -L$${DESTDIR}\ -ltess2$${EXT_NAME}
LIBS += -L$${DESTDIR}\ -lmipmap$${EXT_NAME}
LIBS += -L$${DESTDIR}\ -llzma
LIBS += -L$${DESTDIR}\ -lsqlite3
LIBS += -lpsapi

INCLUDEPATH += ../include
INCLUDEPATH += $${PWD}/s57
INCLUDEPATH += $${PWD}/framework

DEFINES += ocpnUSE_GL TIXML_USE_STL __WXQT__ ocpnUSE_SVG


QT       += core gui network positioning core_private opengl svg xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = zchx_ecdis
TARGET = $$qtLibraryName($$TARGET)

#CONFIG(release, debug|release) {
#  CONFIG_NAME = Release
#} else {
#  CONFIG_NAME = Debug
#}

TargetRoot=$$dirname(PWD)
CONFIG += MyDLL

MyDLL{
    TEMPLATE = lib
    CONFIG += shared dll
#    BINARIES_PATH = $$TargetRoot/out/$$CONFIG_NAME
}else{
#    BINARIES_PATH = $$TargetRoot/bin
    TEMPLATE = app
}
#DESTDIR = $$BINARIES_PATH
!contains(CONFIG, MyDLL){
    DEFINES += MyTest
} else {
    DEFINES -= MyTest
}
warning("dest:" + $$DESTDIR)

TRANSLATIONS += $$PWD/translations/zchx_ecdis_zh_CN.ts
exists($$PWD/translations/zchx_ecdis_zh_CN.ts){
    mkpath($$BINARIES_PATH/translations)
    mkpath($$BINARIES_PATH/resources)
    system(lrelease $$PWD/translations/zchx_ecdis_zh_CN.ts -qm $$BINARIES_PATH/translations/zchx_ecdis_zh_CN.qm)
    system(rcc $$PWD/res/resources.qrc --binary -o $$BINARIES_PATH/resources/zchx_ecdis.rcc)
}

DEFINES *= ZCHX_ECDIS_PLUGIN


SOURCES += mainwindow.cpp \
    zchxtileimagethread.cpp \
    qhttpget.cpp \
    zchxmapwidget.cpp \
    zchxmaploadthread.cpp \
    zchxutils.cpp \
    zchxMapDataUtils.cpp \    
    profiles.cpp \
    framework/zchxmapframe.cpp \
    framework/zchxtilemapframework.cpp \
    framework/zchxvectormapframework.cpp \
    slider_ctrl.cpp \
    proxystyle.cpp \
    coastdatainfodialog.cpp \
    info_dialog.cpp \
    localmarkdlg.cpp \
    structureinfodialog.cpp \
    warningzoneparadialog.cpp \
    zchxdrawmultibeam.cpp \
    element/IDrawElement.cpp \
    element/drawelement.cpp \
    map_layer/zchxmaplayermgr.cpp \
    map_layer/zchxMapLayer.cpp \
    dialog/zchxcameralistdlg.cpp \
    element/aiselement.cpp \
    element/radarelement.cpp \
    zchxroutedatamgr.cpp \
    zchxshipplandatamgr.cpp \
    data_manager/zchxecdisdatamgr.cpp \
    data_manager/zchxaisdatamgr.cpp \
    data_manager/zchxradardatamgr.cpp \
    data_manager/zchxdatamgrfactory.cpp \
    data_manager/zchxcameradatamgr.cpp \
    data_manager/zchxroddatamgr.cpp \
    element/rodelement.cpp \
    element/ipcelement.cpp \
    dialog/zchxecdisdlgmgr.cpp \
    dialog/defenceinfodialog.cpp \
    element/videotargetelement.cpp \
    data_manager/zchxvideotargetdatamgr.cpp \
    data_manager/zchxcameraviewdatamgr.cpp \
    data_manager/zchxuserdefinesdatamgr.cpp \
    element/islandlineelement.cpp \
    element/shipalarmascendelement.cpp \
    element/structureelement.cpp \
    element/moveelement.cpp \
    element/patrolstationelement.cpp \
    draw_manager/zchxdrawareatool.cpp \
    draw_manager/zchxdrawtool.cpp \
    draw_manager/zchxdrawangletool.cpp \
    draw_manager/zchxdrawdistool.cpp \
    draw_manager/zchxdrawlocalmarktool.cpp \
    draw_manager/zchxdrawcameranetgridtool.cpp \
    data_manager/zchxradarvideomgr.cpp \
    element/radarvideoelement.cpp \
    dialog/radarfeaturezone.cpp \
    draw_manager/zchxdrawzonetool.cpp \
    dialog/channelinfodialog.cpp \
    dialog/mooringinfodialog.cpp \
    dialog/cardmouthinfodialog.cpp \
    draw_manager/zchxeditzonetool.cpp \
    element/zchxmoveelements.cpp \
    element/zchxfixelements.cpp \
    element/cameraelement.cpp \
    element/gridelement.cpp \
    data_manager/zchxnetgridmgr.cpp \
    dialog/statistcLineinfodialog.cpp \
    element/radarsiteelement.cpp \
    data_manager/zchxradarsitedatamgr.cpp \
    data_manager/zchxradarrectmgr.cpp \
    element/radarrectelement.cpp \
    s57/_def.cpp \
    s57/s52plib.cpp \
    s57/OCPNPlatform.cpp \
    s57/OCPNRegion.cpp \
    s57/viewport.cpp \
    s57/chartdb.cpp \
    s57/chartdbs.cpp \
    s57/chartimg.cpp \
    s57/chartsymbols.cpp \
    s57/chcanv.cpp \
    s57/gshhs.cpp \
    s57/bbox.cpp \
    s57/bitmap.cpp \
    s57/ocpndc.cpp \
    s57/TexFont.cpp \
    s57/zchxconfig.cpp \
    s57/styles.cpp \
    s57/FontMgr.cpp \
    s57/LLRegion.cpp \
    s57/georef.cpp \
    s57/cutil.cpp \
    s57/Quilt.cpp \
    s57/s57chart.cpp \
    s57/ocpn_pixel.cpp \
    s57/FlexHash.cpp \
    s57/ChartDataInputStream.cpp \
    s57/CanvasConfig.cpp \
    s57/pugixml.cpp \
    s57/s57obj.cpp \
    s57/ogrs57datasource.cpp \
    s57/s57reader.cpp \
    s57/s57RegistrarMgr.cpp \
    s57/s57classregistrar.cpp \
    s57/Osenc.cpp \
    s57/SencManager.cpp \
    s57/FontDesc.cpp \
    s57/mygeom.cpp \
    s57/s57featuredefns.cpp \
    s57/ogrs57layer.cpp \
    s57/s52utils.cpp \
    s57/s52cnsy.cpp \
    s57/vector2d.cpp \
    s57/glTexCache.cpp \
    s57/glTextureDescriptor.cpp \
    s57/glTextureManager.cpp \
    s57/glChartCanvas.cpp \
    s57/glwidget.cpp \
    s57/mbtiles.cpp \
    s57/GL/zchxopenglutil.cpp \
    dialog/zchxmapsourcedialog.cpp \
    zchxvectormapsettingwidget.cpp \
    zchxEcdisProgressWidget.cpp \
    zchxecdispopupwidget.cpp

HEADERS  += mainwindow.h \
    zchxtileimagethread.h \
    qhttpget.h \
    zchxmapwidget.h \
    zchxmaploadthread.h \
    zchx_ecdis_global.h \
    zchxutils.hpp \
    zchxMapDataUtils.h \
    element/IDrawElement.hpp \
    element/drawelement.hpp \
    profiles.h \
    framework/zchxmapframe.h \
    framework/zchxtilemapframework.h \
    framework/zchxvectormapframework.h \
    framework/zchxmapframework.h \
    slider_ctrl.hpp \
    proxystyle.hpp \
    coastdatainfodialog.h \
    info_dialog.hpp \
    localmarkdlg.h \
    structureinfodialog.h \
    warningzoneparadialog.h \
    zchxdrawmultibeam.h \
    map_layer/zchxmaplayermgr.h \
    map_layer/zchxmaplayer_p.h \
    map_layer/zchxMapLayer.h \
    dialog/zchxcameralistdlg.h \
    dialog/defenceinfodialog.h \
    element/aiselement.hpp \
    element/cameraelement.h \
    element/radarelement.h \
    zchxroutedatamgr.h \
    zchxshipplandatamgr.h \
    data_manager/zchxecdisdatamgr.h \
    data_manager/zchxaisdatamgr.h \
    data_manager/zchxradardatamgr.h \
    data_manager/zchxdatamgrfactory.h \
    data_manager/zchxdatamanagers.h \
    data_manager/zchxcameradatamgr.h \
    data_manager/zchxroddatamgr.h \
    element/rodelement.h \
    element/ipcelement.h \
    dialog/zchxecdisdlgmgr.h \
    element/videotargetelement.h \
    data_manager/zchxcameraviewdatamgr.h \
    data_manager/zchxvideotargetdatamgr.h \
    data_manager/zchxuserdefinesdatamgr.h \
    element/islandlineelement.h \
    element/shipalarmascendelement.h \
    element/structureelement.h \
    element/moveelement.h \
    element/patrolstationelement.h \
    data_manager/zchxtemplatedatamgr.h \
    draw_manager/zchxecdismousedefines.h \
    draw_manager/zchxdrawareatool.h \
    draw_manager/zchxdrawtool.h \
    draw_manager/zchxdrawangletool.h \
    draw_manager/zchxdrawdistool.h \
    draw_manager/zchxdrawlocalmarktool.h \
    draw_manager/zchxdrawcameranetgridtool.h \
    draw_manager/zchxdrawtoolutil.h \
    data_manager/zchxradarvideomgr.h \
    element/radarvideoelement.h \
    dialog/radarfeaturezone.h \
    draw_manager/zchxdrawzonetool.h \
    dialog/channelinfodialog.h \
    dialog/mooringinfodialog.h \
    dialog/cardmouthinfodialog.h \
    draw_manager/zchxeditzonetool.h \
    element/zchxmoveelements.h \
    element/fixelement.h \
    element/zchxfixelements.h \
    element/gridelement.h \
    data_manager/zchxnetgridmgr.h \
    dialog/statistcLineinfodialog.h \
    element/radarsiteelement.h \
    data_manager/zchxradarsitedatamgr.h \
    data_manager/zchxradarrectmgr.h \
    element/radarrectelement.h \
    s57/_def.h \
    s57/s52plib.h \
    s57/OCPNPlatform.h \
    s57/OCPNRegion.h \
    s57/viewport.h \
    s57/chartdb.h \
    s57/chartdbs.h \
    s57/chartimg.h \
    s57/chartsymbols.h \
    s57/chcanv.h \
    s57/gshhs.h \
    s57/bbox.h \
    s57/bitmap.h \
    s57/ocpndc.h \
    s57/TexFont.h \
    s57/zchxconfig.h \
    s57/styles.h \
    s57/FontMgr.h \
    s57/LLRegion.h \
    s57/georef.h \
    s57/cutil.h \
    s57/Quilt.h \
    s57/s57chart.h \
    s57/ocpn_pixel.h \
    s57/FlexHash.h \
    s57/ChartDataInputStream.h \
    s57/CanvasConfig.h \
    s57/ogr_s57.h \
    s57/pugixml.hpp \
    s57/s57.h \
    s57/s57RegistrarMgr.h \
    s57/S57ClassRegistrar.h \
    s57/Osenc.h \
    s57/SencManager.h \
    s57/FontDesc.h \
    s57/mygeom.h \
    s57/s52utils.h \
    s57/vector2D.h \
    s57/glTexCache.h \
    s57/glTextureDescriptor.h \
    s57/glTextureManager.h \
    s57/glChartCanvas.h \
    s57/glwidget.h \
    s57/mbtiles.h \
    s57/GL/zchxopenglutil.h \
    dialog/zchxmapsourcedialog.h \
    zchxvectormapsettingwidget.h \
    zchxEcdisProgressWidget.h \
    zchxecdispopupwidget.h

FORMS    += mainwindow.ui \
    coastdatainfodialog.ui \
    localmarkdlg.ui \
    structureinfodialog.ui \
    warningzoneparadialog.ui \
    dialog/radarfeaturezone.ui \
    dialog/defenceinfodialog.ui \
    dialog/channelinfodialog.ui \
    dialog/mooringinfodialog.ui \
    dialog/cardmouthinfodialog.ui \
    dialog/statistcLineinfodialog.ui \
    dialog/zchxmapsourcedialog.ui \
    zchxvectormapsettingwidget.ui

!MyDLL{
    SOURCES += main.cpp
}

RESOURCES += res/resources.qrc
DISTFILES += \
    res/element/camera1.png \
    res/element/camera2.png \
    configuration/maplayers.xml
RESOURCES += $$PWD/opengl/opengl.qrc

DESTDIRRoot = $$TargetRoot/app/3rdParty/zchx_ecdis/3.0.0
#DESTDIRRoot = H:/workspace/2018_WBS_V2/5.code/trunk/kakou_framework_new_ecdis/Tools/3rdParty/zchx_ecdis/3.0.0

qminstall.files = $$BINARIES_PATH/translations/zchx_ecdis_zh_CN.qm
qminstall.path = $$DESTDIRRoot/translations/
INSTALLS += qminstall

rccinstall.files = $$BINARIES_PATH/resources/zchx_ecdis.rcc
rccinstall.path = $$DESTDIRRoot/resources/
INSTALLS += rccinstall

LibFilesInclude.path = $$DESTDIRRoot/include/qt
LibFilesInclude.files = $$PWD/*.h $$PWD/*.hpp
INSTALLS += LibFilesInclude

LibFilesInclude1.path = $$DESTDIRRoot/include/qt/camera_mgr
LibFilesInclude1.files = $$PWD/camera_mgr/*.h $$PWD/camera_mgr/*.hpp
INSTALLS += LibFilesInclude1

LibFilesInclude2.path = $$DESTDIRRoot/include/qt/data_manager
LibFilesInclude2.files = $$PWD/data_manager/*.h $$PWD/data_manager/*.hpp
INSTALLS += LibFilesInclude2

LibFilesInclude3.path = $$DESTDIRRoot/include/qt/element
LibFilesInclude3.files = $$PWD/element/*.h $$PWD/element/*.hpp
INSTALLS += LibFilesInclude3

LibFilesInclude4.path = $$DESTDIRRoot/include/qt/map_layer
LibFilesInclude4.files = $$PWD/map_layer/*.h $$PWD/map_layer/*.hpp
INSTALLS += LibFilesInclude4

LibFilesInclude5.path = $$DESTDIRRoot/include/qt/draw_manager
LibFilesInclude5.files = $$PWD/draw_manager/*.h $$PWD/draw_manager/*.hpp
INSTALLS += LibFilesInclude5


LibFilesLib.path = $$DESTDIRRoot/lib/
LibFilesLib.files += $$DESTDIR/lib$${TARGET}.dll.a
INSTALLS += LibFilesLib

dlltarget.path = $$DESTDIRRoot/bin/$$CONFIG_NAME
dlltarget.files =  $$DESTDIR/$${TARGET}.dll
INSTALLS += dlltarget

layertarget.path = $$DESTDIRRoot/bin/mapdata
layertarget.files = $$PWD/configuration/*.*
INSTALLS += layertarget

#QMAKE_CXXFLAGS_RELEASE *= $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
#QMAKE_LFLAGS_RELEASE *= $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_CXXFLAGS_RELEASE += -O0 -g
QMAKE_CFLAGS_RELEASE += -O0 -g
DEFINES -= QT_NO_DEBUG_OUTPUT  #enable debug outpu

