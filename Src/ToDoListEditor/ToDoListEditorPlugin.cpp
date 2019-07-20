#include "ToDoListEditorPlugin.h"
#include "CToDoEditWidget.h"
#include "PageData.h"
#include <QtCore/QtPlugin>

#define kSettingsGroup          "todolist"
#define kAutoSaveSetting        "autosave"


//--------------------------------------------------------------------------
ToDoListEditorPlugin::ToDoListEditorPlugin()
{
    toDoEditor = NULL;
}

//--------------------------------------------------------------------------
CPluginInterface::Capabilities ToDoListEditorPlugin::capabilities() const
{
    return CPluginInterface::Edit;
}

//--------------------------------------------------------------------------
PAGE_LIST ToDoListEditorPlugin::DoImport(QObject *pParent, QString &importFilePath)
{
    Q_UNUSED(pParent)
    return PAGE_LIST();
}

//--------------------------------------------------------------------------
QString ToDoListEditorPlugin::MenuItemString() const
{
    return QString();
}

//--------------------------------------------------------------------------
CAbstractPageInterface *ToDoListEditorPlugin::getPageInterface() const
{
    return toDoEditor;
}

//--------------------------------------------------------------------------
int ToDoListEditorPlugin::getPageType() const
{
    return kPageTypeToDoList;
}

//--------------------------------------------------------------------------
void ToDoListEditorPlugin::initialize(QWidget *parent, AbstractPluginServices *pluginServices)
{
    Q_UNUSED(pluginServices)
    toDoEditor = new CToDoEditWidget(parent);
}

//--------------------------------------------------------------------------
void ToDoListEditorPlugin::shutdown()
{
}

//--------------------------------------------------------------------------
void ToDoListEditorPlugin::loadSettings(QSettings &settingsObj)
{
    settingsObj.beginGroup(kSettingsGroup);
    toDoEditor->setAutoSave(settingsObj.value(kAutoSaveSetting, true).toBool());
    settingsObj.endGroup();
}

//--------------------------------------------------------------------------
void ToDoListEditorPlugin::saveSettings(QSettings &settingsObj)
{
    settingsObj.beginGroup(kSettingsGroup);
    settingsObj.setValue(kAutoSaveSetting, toDoEditor->isAutoSaved());
    settingsObj.endGroup();
}
