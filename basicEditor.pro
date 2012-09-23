#-------------------------------------------------
#
# Project created by QtCreator 2012-09-05T22:04:59
#
#-------------------------------------------------

QT       += core gui network script

TARGET = basicEditor
TEMPLATE = app

INCLUDEPATH += C:\Users\Graham\Desktop\chuck_today\oscpack

SOURCES += main.cpp\
        mainwindow.cpp \
    ../oscpack/osc/OscTypes.cpp \
    ../oscpack/osc/OscReceivedElements.cpp \
    ../oscpack/osc/OscPrintReceivedElements.cpp \
    ../oscpack/osc/OscOutboundPacketStream.cpp \
    codeedit.cpp \
    revision.cpp

HEADERS  += mainwindow.h \
    ../oscpack/ip/TimerListener.h \
    ../oscpack/ip/PacketListener.h \
    ../oscpack/ip/IpEndpointName.h \
    ../oscpack/osc/OscTypes.h \
    ../oscpack/osc/OscReceivedElements.h \
    ../oscpack/osc/OscPrintReceivedElements.h \
    ../oscpack/osc/OscPacketListener.h \
    ../oscpack/osc/OscOutboundPacketStream.h \
    ../oscpack/osc/OscHostEndianness.h \
    ../oscpack/osc/OscException.h \
    ../oscpack/osc/MessageMappingOscPacketListener.h \
    codeedit.h \
    revision.h

FORMS    += mainwindow.ui

#LIBS += Ws2_32.lib WINMM.LIB
