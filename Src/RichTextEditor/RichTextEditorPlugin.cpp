#include "RichTextEditorPlugin.h"
#include "RichTextEdit.h"
#include "StyleManager.h"
#include "PageData.h"
#include <QtCore/QtPlugin>
#include <QDir>

// Defaults for when there is no settings file
#define kDefaultFontSize    10
#define kDefaultFontFamily  "Verdana"

#define kStyleDefFile	"Styles.xml"

//--------------------------------------------------------------------------
RichTextEditorPlugin::RichTextEditorPlugin()
{
    richTextEditor = NULL;
}

//--------------------------------------------------------------------------
CPluginInterface::Capabilities RichTextEditorPlugin::capabilities() const
{
    return CPluginInterface::Edit;
}

//--------------------------------------------------------------------------
PAGE_LIST RichTextEditorPlugin::DoImport(QObject *pParent, QString &importFilePath)
{
    return PAGE_LIST();
}

//--------------------------------------------------------------------------
QString RichTextEditorPlugin::MenuItemString() const
{
    return QString();
}

//--------------------------------------------------------------------------
CAbstractPageInterface *RichTextEditorPlugin::getPageInterface() const
{
    return richTextEditor;
}

//--------------------------------------------------------------------------
int RichTextEditorPlugin::getPageType() const
{
    return kPageTypeUserText;
}

//--------------------------------------------------------------------------
void RichTextEditorPlugin::initialize(QWidget *parent, AbstractPluginServices *pluginServices)
{
    this->pluginServices = pluginServices;
    styleManager = new CStyleManager();

    LoadStyles(pluginServices->GetSettingsDirectory(false));

    richTextEditor = new CRichTextEdit(styleManager, pluginServices, parent);

    richTextEditor->initialize();
}

//--------------------------------------------------------------------------
void RichTextEditorPlugin::shutdown()
{
    SaveStyles(pluginServices->GetSettingsDirectory(true));
}

//--------------------------------------------------------------------------
void RichTextEditorPlugin::loadSettings(QSettings &settingsObj)
{
    int     defaultFontSize;
    QString defaultFontFamily;

    settingsObj.beginGroup("editor");

    defaultFontSize = settingsObj.value("defaultfontsize", kDefaultFontSize).toInt();
    defaultFontFamily = settingsObj.value("defaultfontfamily", kDefaultFontFamily).toString();

    settingsObj.endGroup();

    richTextEditor->SetDefaultFont(defaultFontFamily, defaultFontSize);
}

//--------------------------------------------------------------------------
void RichTextEditorPlugin::saveSettings(QSettings &settingsObj)
{
    int fontSize = richTextEditor->getDefaultFontSize();
    QString fontFamily = richTextEditor->getDefaultFontFamily();

    settingsObj.beginGroup("editor");
    settingsObj.setValue("defaultfontsize", fontSize);
    settingsObj.setValue("defaultfontfamily", fontFamily);
    settingsObj.endGroup();
}

//--------------------------------------------------------------------------
void RichTextEditorPlugin::LoadStyles(const QString& styleFilePath)
{
    if (! styleFilePath.isEmpty())
    {
        QDir    styleDirectory(styleFilePath);

        styleManager->LoadStyleDefs(styleDirectory.absoluteFilePath(kStyleDefFile));
    }
}

//--------------------------------------------------------------------------
void RichTextEditorPlugin::SaveStyles( const QString& styleFilePath )
{
    QDir    styleDirectory(styleFilePath);

    styleManager->SaveStyleDefs(styleDirectory.absoluteFilePath(kStyleDefFile));
}
