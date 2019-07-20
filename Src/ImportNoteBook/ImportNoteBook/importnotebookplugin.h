#ifndef IMPORTNOTEBOOKPLUGIN_H
#define IMPORTNOTEBOOKPLUGIN_H

#include <QObject>
#include "../../NoteBook/NoteBook/PageData.h"
#include "../../NoteBook/NoteBook/PluginInterface.h"

class AbstractPluginServices;

class ImportNoteBookPlugin : public QObject, public CPluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "importnotebookplugin.json")
	Q_INTERFACES(CPluginInterface)

public:
	//ImportNoteBookPlugin(QObject *parent = 0);	// I don't think a constructor is needed
	~ImportNoteBookPlugin() {}

    virtual CPluginInterface::Capabilities capabilities() const;
    virtual PAGE_LIST DoImport(QObject *pParent, QString &importFilePath);
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

#endif // IMPORTNOTEBOOKPLUGIN_H
