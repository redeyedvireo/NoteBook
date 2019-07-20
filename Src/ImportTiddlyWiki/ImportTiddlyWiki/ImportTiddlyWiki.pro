QT       += core gui sql
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets

    greaterThan(QT_MINOR_VERSION, 5) {
        QT += webengine
        QT += webenginewidgets
    } else {
        QT += webkitwidgets
    }
} else {
    QT += webkit
}
CONFIG += plugin

TARGET = ImportTiddlyWiki
TEMPLATE = lib

DEFINES += IMPORTTIDDLYWIKI_LIBRARY

OBJECTS_DIR = ../generated_files
MOC_DIR = ../generated_files
UI_DIR = ../generated_files
RCC_DIR = ../generated_files

SOURCES += ImportTiddlyWiki.cpp \
           ImportPageData.cpp \
           importtiddlywikiplugin.cpp \
           PreviewImportDialog.cpp \
           WikiTextRenderer.cpp \
           ../../NoteBook/NoteBook/PageData.cpp

HEADERS += ImportTiddlyWiki.h \
           ImportPageData.h \
           importtiddlywikiplugin.h \
           PreviewImportDialog.h \
           WikiTextRenderer.h \
           PageData.h

FORMS += PreviewImportDialog.ui

INCLUDEPATH += ../../NoteBook/NoteBook

unix {
    target.path = /usr/lib
    INSTALLS += target
}
