#-------------------------------------------------
#
# Project created by QtCreator 2012-09-05T22:04:59
#
#-------------------------------------------------

QT       += core gui network script

TARGET = basicEditor
TEMPLATE = app

INCLUDEPATH += oscpack

SOURCES += main.cpp\
        mainwindow.cpp \
    oscpack/osc/OscTypes.cpp \
    oscpack/osc/OscReceivedElements.cpp \
    oscpack/osc/OscPrintReceivedElements.cpp \
    oscpack/osc/OscOutboundPacketStream.cpp \
    codeedit.cpp \
    revision.cpp \
    process.cpp \
    revtree.cpp \
    mw_macros.cpp \
    codearea.cpp \
    engine.cpp \
    superwordhighlighter.cpp \
    revtreeitem.cpp \
    proctreeitem.cpp

HEADERS  += mainwindow.h \
    oscpack/ip/TimerListener.h \
    oscpack/ip/PacketListener.h \
    oscpack/ip/IpEndpointName.h \
    oscpack/osc/OscTypes.h \
    oscpack/osc/OscReceivedElements.h \
    oscpack/osc/OscPrintReceivedElements.h \
    oscpack/osc/OscPacketListener.h \
    oscpack/osc/OscOutboundPacketStream.h \
    oscpack/osc/OscHostEndianness.h \
    oscpack/osc/OscException.h \
    oscpack/osc/MessageMappingOscPacketListener.h \
    codeedit.h \
    revision.h \
    revui.h \
    process.h \
    revtree.h \
    codearea.h \
    engine.h \
    superwordhighlighter.h \
    revtreeitem.h \
    proctreeitem.h

FORMS    += mainwindow.ui

#LIBS += Ws2_32.lib WINMM.LIB



release {
    install_it.path = $$OUT_PWD/release
}
install_it.files += "c:\\eula.1028.txt"

#QTBINPATH = "c:\\Qt\\4.8.6\\bin"
QTBINPATH = "C:\\QtSDK\\Desktop\\Qt\\4.8.0\\msvc2008\\bin"
dllset.path = $$OUT_PWD/release
dllset.files += $$QTBINPATH/QtGui4.dll
dllset.files += $$QTBINPATH/QtScript4.dll
dllset.files += $$QTBINPATH/QtCore4.dll
dllset.files += $$QTBINPATH/libwinpthread-1.dll
#dllset.files += $$QTBINPATH/libstdc++-6.dll
dllset.files += $$QTBINPATH/QtNetwork4.dll

#copy files after build
INSTALLS += install_it dllset

OTHER_FILES += \
    macros.qs editor.org

