#include "PluginServices.h"
#include "Database.h"
#include "notebook.h"
#include <QPixmap>

//--------------------------------------------------------------------------
PluginServices::PluginServices(CNoteBook *notebook, CDatabase *database)
{
    this->notebook = notebook;
    this->database = database;
}

//--------------------------------------------------------------------------
QString PluginServices::GetSettingsDirectory(bool bCreate)
{
    return notebook->GetSettingsDirectory(bCreate);
}

//--------------------------------------------------------------------------
ENTITY_ID PluginServices::currentPageId()
{
    if (notebook != NULL)
    {
        return notebook->GetCurrentPageId();
    }
    else
    {
        return kInvalidPageId;
    }

}

//--------------------------------------------------------------------------
QString PluginServices::GetPageTitle(int pageId)
{
    QString     pageTitle;

    database->GetPageTitle(pageId, pageTitle);
    return pageTitle;
}

//--------------------------------------------------------------------------
bool PluginServices::GetPageList(PAGE_HASH &pageHash)
{
    return database->GetPageList(pageHash);
}

//--------------------------------------------------------------------------
PAGE_HASH PluginServices::GetChildren(ENTITY_ID parentId)
{
    PAGE_HASH       pageHash;

    database->GetChildren(pageHash, parentId);

    return pageHash;
}

//--------------------------------------------------------------------------
bool PluginServices::AddImageToCurrentPage(QString imageName, QPixmap pixmap)
{
    ENTITY_ID       curPageId = currentPageId();

    return database->AddImage(imageName, pixmap, curPageId);
}

//--------------------------------------------------------------------------
QPixmap PluginServices::GetImage(QString imageName)
{
    QPixmap     pixmap;

    database->GetImage(imageName, pixmap);

    // This will be an invalid pixmap if a database error occurs.
    return pixmap;
}
