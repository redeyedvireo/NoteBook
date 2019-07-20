#ifndef IMPORTTIDDLYWIKI_H
#define IMPORTTIDDLYWIKI_H

#include <QObject>
#include "PageData.h"
#include "PluginInterface.h"

class AbstractPluginServices;

class CImportTiddlyWikiPlugin : public QObject, public CPluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "importtiddlywikiplugin.json")
	Q_INTERFACES(CPluginInterface)

public:
	//CImportTiddlyWikiPlugin(QObject *parent = 0);	// I don't think a constructor is needed
	CImportTiddlyWikiPlugin() {}
	~CImportTiddlyWikiPlugin() {}

    virtual CPluginInterface::Capabilities capabilities() const;		// Return "import"
    virtual PAGE_LIST DoImport(QObject *pParent, QString& importFilePath);
    virtual QString MenuItemString() const;
    virtual CAbstractPageInterface *getPageInterface() const;
    virtual int getPageType() const;
    virtual void initialize(QWidget *parent, AbstractPluginServices *pluginServices);
    virtual void shutdown();
    virtual void loadSettings(QSettings& settingsObj) { Q_UNUSED(settingsObj) }
    virtual void saveSettings(QSettings& settingsObj) { Q_UNUSED(settingsObj) }

signals:
	void ImportStarting();			// Mostly for debug
};

#endif // IMPORTTIDDLYWIKI_H
