#include "FolderEditWidget.h"
#include "AbstractPluginServices.h"
#include "CustomWebPage.h"
#include <QFile>
#include <QVBoxLayout>
#include <QWebEngineView>


#define NOTEBOOKTAG     "NB://"
#define kFolderString   "%folder%"
#define kBlockString    "%block%"
#define kFolderFile     "Folder.png"
#define kPageFile       "Page.png"
#define kStyleFile      ":/files/style.css"
#define kTemplateFile   ":/files/template.html"
#define kUrlPath        "qrc:/files/"


//--------------------------------------------------------------------------
FolderEditWidget::FolderEditWidget(AbstractPluginServices *pluginServices, QWidget *parent)
    : pluginServices(pluginServices), QWidget(parent)
{
    setObjectName("FolderEditWidget");

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    webView = new QWebEngineView(this);

    webPage = new CustomWebPage(this);
    webView->setPage(webPage);

    layout->addWidget(webView);
}

//--------------------------------------------------------------------------
void FolderEditWidget::initialize()
{
    // Retrieve CSS contents
    styleDef = readFile(kStyleFile);
    htmlTemplate = readFile(kTemplateFile);

    // Set connections
    connect(webPage, SIGNAL(linkClicked(const QUrl&)), this, SLOT(OnLinkClicked(const QUrl&)));
}

//--------------------------------------------------------------------------
void FolderEditWidget::PopulateView()
{
    ENTITY_ID   curPageId = pluginServices->currentPageId();
    QString     folderName = pluginServices->GetPageTitle(curPageId);
    PAGE_HASH   children = pluginServices->GetChildren(curPageId);
    QStringList childrenTitles;

    QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	it(children);

    while (it.hasNext())
    {
        it.next();
        SHARED_CPAGEDATA	pageData = it.value();

        QString     linkStr = QString("<tr><td class=\"image\"><img $imageattrs$></td><td>$link$</td></tr>");
        QString     linkText;
        int         imageWidth;
        QString     fileName;
        QString     imageAttrString("src=\"%1\" style=\"width:%2px;height:16px;\"");


        if (pageData.data()->m_pageType == kPageFolder)
        {
            imageWidth = 20;
            fileName = kFolderFile;
            linkText = QString("<i>%1</i>").arg(pageData.data()->m_title);
        }
        else
        {
            imageWidth = 16;
            fileName = kPageFile;
            linkText = QString("%1").arg(pageData.data()->m_title);
        }

        linkStr.replace("$imageattrs$",
                        QString("src=\"%1\" style=\"width:%2px;height:16px;\"")
                            .arg(fileName)
                            .arg(imageWidth));

        linkStr.replace("$link$",
                        QString("<a href=\"%1page%2\">%3</a>")
                            .arg(NOTEBOOKTAG)
                            .arg(pageData.data()->m_pageId)
                            .arg(linkText));

        childrenTitles << linkStr;
    }

    QString     childrenString = childrenTitles.join("");
    QString     contentString;

    // Construct content string
    contentString.append(styleDef);
    contentString.append(htmlTemplate);

    // Replace marker strings in the HTML template with
    // the text contents
    contentString.replace(kFolderString, folderName);
    contentString.replace(kBlockString, childrenString);

    webView->setHtml(contentString, QUrl(kUrlPath));
}

//--------------------------------------------------------------------------
QString FolderEditWidget::readFile(QString fileName)
{
    QFile   file(fileName);
    QString contents;

    if (file.open(QIODevice::ReadOnly))
    {
        contents = file.readAll();
        file.close();
    }

    return contents;
}

//--------------------------------------------------------------------------
void FolderEditWidget::OnLinkClicked(const QUrl &url)
{
    QString     linkStr = url.toString();
    QRegExp     pageIdRx("page(\\d+)");
    int         pos = 0;

    if ((pos = pageIdRx.indexIn(linkStr, 0)) != -1)
    {
        ENTITY_ID   pageId = kInvalidPageId;
        QString		pageIdStr = pageIdRx.cap(1);

        pageId = pageIdStr.toUInt();

        if (pageId != kInvalidPageId)
        {
            emit GotoPage(pageId);
        }
    }
}

//--------------------------------------------------------------------------
void FolderEditWidget::setPageContents(QString contents, QStringList imageNames)
{
    Q_UNUSED(contents);
    Q_UNUSED(imageNames);

    PopulateView();
}
