TEMPLATE = app
TARGET = NoteBook
QT += core gui sql xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += qt

unix:LIBS += -lcrypto

win32 {
    debug {
        LIBS += c:/OpenSSL-Win64/lib/VC/libeay32MTd.lib
    } else {
        LIBS += c:/OpenSSL-Win64/lib/VC/libeay32MT.lib
    }

    INCLUDEPATH += "C:/OpenSSL-Win64/include"
}

OBJECTS_DIR = ../generated_files
MOC_DIR = ../generated_files
UI_DIR = ../generated_files
RCC_DIR = ../generated_files

INSTALLS += target

unix {
DEFINES += LINUX
}

win32 {
DEFINES += WIN32
}

HEADERS += ./AboutDialog.h \
    ./ClickableLabel.h \
    ./Database.h \
    ./DateTree.h \
    ./DateWidgetItem.h \
    ./Encrypter.h \
    ./ExistingDbEntryDialog.h \
    ./ExportDlg.h \
    ./Exporter.h \
    ./FavoritesWidget.h \
    ./Importer.h \
    ./ImportFileDlg.h \
    ./NavigationPane.h \
    ./notebook.h \
    ./PageCache.h \
    ./PageData.h \
    ./PageHistoryWidget.h \
    ./PageInfoDlg.h \
    ./PageTitleList.h \
    ./PageTree.h \
    ./PluginInterface.h \
    ./Preferences.h \
    ./PrefsDialog.h \
    ./SearchWidget.h \
    ./SetPassword.h \
    ./stdafx.h \
    ./TagCache.h \
    ./TagList.h \
    ./uidef.h \
    CAbstractPageInterface.h \
    CEditorManager.h \
    AbstractPluginServices.h \
    PluginServices.h \
    TitleLabelWidget.h \
    PageItemModel.h
SOURCES += ./AboutDialog.cpp \
    ./ClickableLabel.cpp \
    ./Database.cpp \
    ./DateTree.cpp \
    ./DateWidgetItem.cpp \
    ./Encrypter.cpp \
    ./ExistingDbEntryDialog.cpp \
    ./ExportDlg.cpp \
    ./Exporter.cpp \
    ./FavoritesWidget.cpp \
    ./Importer.cpp \
    ./ImportFileDlg.cpp \
    ./main.cpp \
    ./NavigationPane.cpp \
    ./notebook.cpp \
    ./PageCache.cpp \
    ./PageData.cpp \
    ./PageHistoryWidget.cpp \
    ./PageInfoDlg.cpp \
    ./PageTitleList.cpp \
    ./PageTree.cpp \
    ./Preferences.cpp \
    ./PrefsDialog.cpp \
    ./SearchWidget.cpp \
    ./SetPassword.cpp \
    ./stdafx.cpp \
    ./TagCache.cpp \
    ./TagList.cpp \
    CEditorManager.cpp \
    PluginServices.cpp \
    TitleLabelWidget.cpp \
    PageItemModel.cpp
FORMS += ./AboutDialog.ui \
    ./ExistingDbEntryDialog.ui \
    ./ExportDlg.ui \
    ./FavoritesWidget.ui \
    ./ImportFileDlg.ui \
    ./notebook.ui \
    ./PageInfoDlg.ui \
    ./PrefsDialog.ui \
    ./SearchWidget.ui \
    ./SetPassword.ui \
    TitleLabelWidget.ui
RESOURCES += notebook.qrc
RC_FILE = NoteBook.rc
