//#include "stdafx.h"
#include "importtiddlywikiplugin.h"
#include "ImportTiddlyWiki.h"
#include <QtCore/QtPlugin>
#include <QMessageBox>


#if 0
//--------------------------------------------------------------------------
CImportTiddlyWikiPlugin::CImportTiddlyWikiPlugin(QObject *parent)
	: QObject(parent)
{
}
#endif

//--------------------------------------------------------------------------
CPluginInterface::Capabilities CImportTiddlyWikiPlugin::capabilities() const
{
    return CPluginInterface::Import;
}

//--------------------------------------------------------------------------
PAGE_LIST CImportTiddlyWikiPlugin::DoImport( QObject *pParent, QString& importFilePath )
{
	CImportTiddlyWiki		itw(10);		// TODO: Get font size from dialog

	return itw.Import(importFilePath);
}

//--------------------------------------------------------------------------
QString CImportTiddlyWikiPlugin::MenuItemString() const
{
    return QString("Import TiddlyWiki");
}

//--------------------------------------------------------------------------
CAbstractPageInterface *CImportTiddlyWikiPlugin::getPageInterface() const
{
    return NULL;
}

//--------------------------------------------------------------------------
int CImportTiddlyWikiPlugin::getPageType() const
{
    return -1;
}

//--------------------------------------------------------------------------
void CImportTiddlyWikiPlugin::initialize(QWidget *parent, AbstractPluginServices *pluginServices)
{
    Q_UNUSED(parent);
    Q_UNUSED(pluginServices)
}

//--------------------------------------------------------------------------
void CImportTiddlyWikiPlugin::shutdown()
{
}

#if (QT_VERSION < 0x050000)
Q_EXPORT_PLUGIN2(importtiddlywiki, CImportTiddlyWikiPlugin);
#endif
