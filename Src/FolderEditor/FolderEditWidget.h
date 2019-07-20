#ifndef FOLDEREDITWIDGET_H
#define FOLDEREDITWIDGET_H

#include <QWidget>
#include <QUrl>
#include "CAbstractPageInterface.h"

class QWebEngineView;
class AbstractPluginServices;
class CustomWebPage;


class FolderEditWidget : public QWidget, public CAbstractPageInterface
{
    Q_OBJECT
public:
    explicit FolderEditWidget(AbstractPluginServices *pluginServices, QWidget *parent = 0);

    void initialize();

    /*
     * CAbstractPageInterface overrides
     */
    virtual PAGE_TYPE documentType()    { return kPageFolder; }

    virtual QWidget *editorWidget() { return this; }

    virtual void newDocument()  {}

    virtual QString getPageContents() { return QString(); }

    virtual void setPageContents(QString contents, QStringList imageNames);

    virtual void setDocumentModified(bool bModified) { Q_UNUSED(bModified) }

    virtual bool isDocumentModified()   { return false; }

    virtual void enableEditing(bool bEnable) { Q_UNUSED(bEnable); }

    virtual QWidget *settingsWidget() { return NULL; }

    virtual QString settingsGroupName() { return QString(); }

    virtual void settingsDialogAccepted(QWidget* pSettingsWidget) { Q_UNUSED(pSettingsWidget) }

private slots:
    void OnLinkClicked(const QUrl &url);

signals:
    // Signal sent when user clicks a link to another page
    void GotoPage(ENTITY_ID pageId);

private:
    void PopulateView();
    QString readFile(QString fileName);

    AbstractPluginServices  *pluginServices;

    QWebEngineView  *webView;
    CustomWebPage   *webPage;
    QString         styleDef;       // CSS contents
    QString         htmlTemplate;   // HTML template of the page
};

#endif // FOLDEREDITWIDGET_H
