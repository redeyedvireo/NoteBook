#-------------------------------------------------
#
# Project created by QtCreator 2015-12-29T09:15:47
#
#-------------------------------------------------

QT       += core gui
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

TARGET = FolderEditor
TEMPLATE = lib

DEFINES += FOLDEREDITOR_LIBRARY

OBJECTS_DIR = ../objects
MOC_DIR = ../generated_files
UI_DIR = ../generated_files
RCC_DIR = ../generated_files

SOURCES += FolderEditorPlugin.cpp \
    FolderEditWidget.cpp \
    CustomWebPage.cpp

HEADERS += FolderEditorPlugin.h\
        foldereditor_global.h \
    ../NoteBook/NoteBook/AbstractPluginServices.h \
    ../NoteBook/NoteBook/CAbstractPageInterface.h \
    ../NoteBook/NoteBook/PageData.h \
    FolderEditWidget.h \
    CustomWebPage.h

INCLUDEPATH += ../NoteBook/NoteBook

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    foldereditorplugin.json \
    style.css \
    template.html

RESOURCES += \
    FolderEditor.qrc
