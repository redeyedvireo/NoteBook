#ifndef RICHTEXTEDITORPLUGIN_H
#define RICHTEXTEDITORPLUGIN_H

#include "richtexteditor_global.h"
#include <QObject>
#include "../NoteBook/NoteBook/PluginInterface.h"

class CRichTextEdit;
class CStyleManager;
class AbstractPluginServices;

class RICHTEXTEDITORSHARED_EXPORT RichTextEditorPlugin : public QObject, public CPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "richtexteditorplugin.json")
    Q_INTERFACES(CPluginInterface)

public:
    RichTextEditorPlugin();

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
    CRichTextEdit       *richTextEditor;
    CStyleManager       *styleManager;
    AbstractPluginServices  *pluginServices;

    void LoadStyles(const QString &styleFilePath);
    void SaveStyles(const QString &styleFilePath);
};

#endif // RICHTEXTEDITORPLUGIN_H
