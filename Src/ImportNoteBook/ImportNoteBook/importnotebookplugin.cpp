#include "importnotebook.h"
#include "importnotebookplugin.h"
#include <QtCore/QtPlugin>
#include <QStringList>

#if 0
ImportNoteBookPlugin::ImportNoteBookPlugin(QObject *parent)
	: QObject(parent)
{
}
#endif

//--------------------------------------------------------------------------
CPluginInterface::Capabilities ImportNoteBookPlugin::capabilities() const
{
    return CPluginInterface::Import;
}

//--------------------------------------------------------------------------
PAGE_LIST ImportNoteBookPlugin::DoImport( QObject *pParent, QString& importFilePath )
{
    Q_UNUSED(pParent)
	ImportNoteBook		inb;

	return inb.Import(importFilePath);
}

//--------------------------------------------------------------------------
QString ImportNoteBookPlugin::MenuItemString() const
{
    return QString("Import NoteBook");
}

//--------------------------------------------------------------------------
CAbstractPageInterface *ImportNoteBookPlugin::getPageInterface() const
{
    return NULL;
}

//--------------------------------------------------------------------------
int ImportNoteBookPlugin::getPageType() const
{
    return -1;
}

//--------------------------------------------------------------------------
void ImportNoteBookPlugin::initialize(QWidget *parent, AbstractPluginServices *pluginServices)
{
    Q_UNUSED(parent);
    Q_UNUSED(pluginServices)
}

//--------------------------------------------------------------------------
void ImportNoteBookPlugin::shutdown()
{
}

#if (QT_VERSION < 0x050000)
Q_EXPORT_PLUGIN2(importnotebook, ImportNoteBookPlugin);
#endif
