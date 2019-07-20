#include "FolderEditorPlugin.h"
#include "FolderEditWidget.h"
#include "PageData.h"
#include <QtCore/QtPlugin>


//--------------------------------------------------------------------------
FolderEditorPlugin::FolderEditorPlugin()
{
    folderEditor = NULL;
}

//--------------------------------------------------------------------------
CPluginInterface::Capabilities FolderEditorPlugin::capabilities() const
{
    return CPluginInterface::Edit;
}

//--------------------------------------------------------------------------
PAGE_LIST FolderEditorPlugin::DoImport(QObject *pParent, QString &importFilePath)
{
    return PAGE_LIST();
}

//--------------------------------------------------------------------------
QString FolderEditorPlugin::MenuItemString() const
{
    return QString();
}

//--------------------------------------------------------------------------
CAbstractPageInterface *FolderEditorPlugin::getPageInterface() const
{
    return folderEditor;
}

//--------------------------------------------------------------------------
int FolderEditorPlugin::getPageType() const
{
    return kPageFolder;
}

//--------------------------------------------------------------------------
void FolderEditorPlugin::initialize(QWidget *parent, AbstractPluginServices *pluginServices)
{
    Q_UNUSED(pluginServices)
    folderEditor = new FolderEditWidget(pluginServices, parent);
    folderEditor->initialize();
}

//--------------------------------------------------------------------------
void FolderEditorPlugin::shutdown()
{
}
