#ifndef FOLDEREDITORPLUGIN_H
#define FOLDEREDITORPLUGIN_H

#include "foldereditor_global.h"
#include <QObject>
#include "../NoteBook/NoteBook/PluginInterface.h"

class FolderEditWidget;
class AbstractPluginServices;

class FOLDEREDITORSHARED_EXPORT FolderEditorPlugin : public QObject, public CPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "foldereditorplugin.json")
    Q_INTERFACES(CPluginInterface)

public:
    FolderEditorPlugin();

    virtual CPluginInterface::Capabilities capabilities() const;
    virtual PAGE_LIST DoImport(QObject *pParent, QString &importFilePath);
    virtual QString MenuItemString() const;
    virtual CAbstractPageInterface *getPageInterface() const;
    virtual int getPageType() const;
    virtual void initialize(QWidget *parent, AbstractPluginServices *pluginServices);
    virtual void shutdown();
    virtual void loadSettings(QSettings& settingsObj) { Q_UNUSED(settingsObj) }
    virtual void saveSettings(QSettings& settingsObj) { Q_UNUSED(settingsObj) }

private:
    FolderEditWidget        *folderEditor;
};

#endif // FOLDEREDITORPLUGIN_H
