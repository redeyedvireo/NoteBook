#-------------------------------------------------
#
# Project created by QtCreator 2015-12-28T10:38:44
#
#-------------------------------------------------

QT       += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += plugin

TARGET = ToDoListEditor
TEMPLATE = lib

DEFINES += TODOLISTEDITOR_LIBRARY

OBJECTS_DIR = ../generated_files
MOC_DIR = ../generated_files
UI_DIR = ../generated_files
RCC_DIR = ../generated_files

SOURCES += ToDoListEditorPlugin.cpp \
        CTaskReader.cpp \
        CTaskWriter.cpp \
        CToDoEditWidget.cpp \
        CToDoItem.cpp \
        CToDoListEditorSettingsWidget.cpp \
        CPriorityDelegate.cpp

HEADERS += ToDoListEditorPlugin.h\
        todolisteditor_global.h \
        ../NoteBook/NoteBook/PluginInterface.h \
        CTaskReader.h \
        CTaskWriter.h \
        CToDoEditWidget.h \
        CToDoItem.h \
        CToDoListEditorSettingsWidget.h \
        TaskDefs.h \
        ../NoteBook/NoteBook/CAbstractPageInterface.h \
        CPriorityDelegate.h \
        ../NoteBook/NoteBook/PageData.h

FORMS += CToDoListEditorSettingsWidget.ui \
    ToDoEdit.ui

INCLUDEPATH += ../NoteBook/NoteBook

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    todolisteditorplugin.json
