#-------------------------------------------------
#
# Project created by QtCreator 2015-12-28T12:25:07
#
#-------------------------------------------------

QT       += core gui sql xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += plugin

TARGET = RichTextEditor
TEMPLATE = lib

DEFINES += RICHTEXTEDITOR_LIBRARY

OBJECTS_DIR = ../generated_files
MOC_DIR = ../generated_files
UI_DIR = ../generated_files
RCC_DIR = ../generated_files

SOURCES += RichTextEditorPlugin.cpp \
    CCustomTextEdit.cpp \
    ColorButton.cpp \
    CRichTextEditorSettingsWidget.cpp \
    CTextImage.cpp \
    CTextTable.cpp \
    RichTextEdit.cpp \
    SelectStyleDlg.cpp \
    StyleDef.cpp \
    StyleDlg.cpp \
    StyleManager.cpp \
    TableFormatDlg.cpp \
    AddWebLinkDlg.cpp \
    ChoosePageToLinkDlg.cpp

HEADERS += RichTextEditorPlugin.h\
        richtexteditor_global.h \
    CCustomTextEdit.h \
    ColorButton.h \
    CRichTextEditorSettingsWidget.h \
    CTextImage.h \
    CTextTable.h \
    RichTextEdit.h \
    SelectStyleDlg.h \
    StyleDef.h \
    StyleDlg.h \
    StyleManager.h \
    TableFormatDlg.h \
    TextTableDefs.h \
    ../NoteBook/NoteBook/CAbstractPageInterface.h \
    ../NoteBook/NoteBook/PageData.h \
    ../NoteBook/NoteBook/PluginInterface.h \
    AddWebLinkDlg.h \
    ../NoteBook/NoteBook/AbstractPluginServices.h \
    ../NoteBook/NoteBook/GlobalData.h \
    ChoosePageToLinkDlg.h

FORMS += \
    RichTextEdit.ui \
    RichTextEditorSettingsWidget.ui \
    SelectStyleDlg.ui \
    StyleDlg.ui \
    TableFormatDlg.ui \
    AddWebLinkDlg.ui \
    ChoosePageToLinkDlg.ui

INCLUDEPATH += ../NoteBook/NoteBook

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    richtexteditorplugin.json

RESOURCES += \
    RichTextEditor.qrc
