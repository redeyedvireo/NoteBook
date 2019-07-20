#ifndef TODOLISTEDITORPLUGIN_H
#define TODOLISTEDITORPLUGIN_H

#include "todolisteditor_global.h"
#include <QObject>
#include "../NoteBook/NoteBook/PluginInterface.h"

class CToDoEditWidget;
class AbstractPluginServices;

class TODOLISTEDITORSHARED_EXPORT ToDoListEditorPlugin : public QObject, public CPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "todolisteditorplugin.json")
    Q_INTERFACES(CPluginInterface)

public:
    ToDoListEditorPlugin();

    virtual CPluginInterface::Capabilities capabilities() const;
    virtual PAGE_LIST DoImport(QObject *pParent, QString &importFilePath);
    virtual QString MenuItemString() const;
    virtual CAbstractPageInterface *getPageInterface() const;
    virtual int getPageType() const;
    virtual void initialize(QWidget *parent, AbstractPluginServices *pluginServices);
    virtual void shutdown();
    virtual void loadSettings(QSettings& settingsObj);
    virtual void saveSettings(QSettings& settingsObj);

private:
    CToDoEditWidget     *toDoEditor;
};

#endif // TODOLISTEDITORPLUGIN_H
