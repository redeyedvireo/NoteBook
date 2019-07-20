#ifndef PLUGINSERVICES_H
#define PLUGINSERVICES_H

#include "AbstractPluginServices.h"

/*
 * Concrete subclass of AbstractPluginServices.  See AbstractPluginServices.h
 * for function documentation.
 */


class CNoteBook;
class CDatabase;

class PluginServices : public AbstractPluginServices
{
public:
    PluginServices(CNoteBook *notebook, CDatabase *database);

    /**
     * Returns the path of the application's settings directory.
     * @param bCreate If true, the settings directory will be created, if
     *        it does not exist.
     * @return Returns the settings directory.
     */
    virtual QString GetSettingsDirectory(bool bCreate);

    /**
     * Returns the page ID of the page currently being edited.
     * @return Page ID
     */
    virtual ENTITY_ID currentPageId();

    /**
     * Returns the title of the page of the given page ID.
     * @return Page title, or empty string if not successful.
     */
    virtual QString GetPageTitle(int pageId);

    /**
     *	Gets a list of page IDs and their page titles.  This is used mainly
     *	for populating the page cache when a database is opened.
     *
     *	@param pageHash On return, contains page IDs and their
     *			associated CPageData object.  The CPageData object in this
     *			case only contains the following items:
     *			- parent ID
     *			- page title
     *			- page modification date
     */
    virtual bool GetPageList(PAGE_HASH& pageHash);

    virtual PAGE_HASH GetChildren(ENTITY_ID parentId);

    /**
     * Adds an image to the image cache of the current page.
     * @return True if successful, false otherwise.
     */
    virtual bool AddImageToCurrentPage(QString imageName, QPixmap pixmap);

    /**
     * Retrieves an image from the database.
     * @return Pixmap.  This will be invalid in the event of an error.
     */
    virtual QPixmap GetImage(QString imageName);

private:
    CNoteBook       *notebook;
    CDatabase       *database;
};

#endif // PLUGINSERVICES_H
