QT += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += plugin

TARGET = ImportNoteBook
TEMPLATE = lib

DEFINES += QT_LARGEFILE_SUPPORT QT_DLL QT_SQL_LIB

OBJECTS_DIR = ../generated_files
MOC_DIR = ../generated_files
UI_DIR = ../generated_files
RCC_DIR = ../generated_files

#DESTDIR = ../Output
#PRECOMPILED_HEADER = StdAfx.h

unix:LIBS += -lcrypto
win32 {
    debug {
        LIBS += c:/OpenSSL-Win64/lib/VC/libeay32MTd.lib
    } else {
        LIBS += c:/OpenSSL-Win64/lib/VC/libeay32MT.lib
    }
    INCLUDEPATH += "C:/OpenSSL-Win64/include"
}


unix {
DEFINES += LINUX
}

win32 {
DEFINES += WIN32
}

HEADERS += ../../NoteBook/NoteBook/Encrypter.h \
    ./ImportDatabase.h \
    ./importnotebook.h \
    ./importnotebookplugin.h \
    ../../NoteBook/NoteBook/PageData.h \
    ../../NoteBook/NoteBook/PluginInterface.h \
    ./PreviewImportDialog.h
SOURCES += ../../NoteBook/NoteBook/Encrypter.cpp \
    ./ImportDatabase.cpp \
    ./importnotebook.cpp \
    ./importnotebookplugin.cpp \
    ../../NoteBook/NoteBook/PageData.cpp \
    ./PreviewImportDialog.cpp
FORMS += ./PreviewImportDialog.ui
