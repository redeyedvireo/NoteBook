#ifndef ABSTRACTPLUGINSERVICES_H
#define ABSTRACTPLUGINSERVICES_H

#include "PageData.h"
#include <QString>

/**
 * Miscellaneous functions provided to plug-ins.
 */

class AbstractPluginServices
{
public:
    /**
     * Returns the path of the application's settings directory.
     * @param bCreate If true, the settings directory will be created, if
     *        it does not exist.
     * @return Returns the settings directory.
     */
    virtual QString GetSettingsDirectory(bool bCreate) = 0;

    /**
     * Returns the page ID of the page currently being edited.
     * @return Page ID
     */
    virtual ENTITY_ID currentPageId() = 0;

    /**
     * Returns the title of the page of the given page ID.
     * @return Page title, or empty string if not successful.
     */
    virtual QString GetPageTitle(int pageId) = 0;

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
    virtual bool GetPageList(PAGE_HASH& pageHash) = 0;

    /**
     * Returns a list of all the children pages of the given page, which must be a folder.
     * If an error occurs, the returned list will be empty.
     *
     * @param parentId  Page ID of the folder
     * @return Returns a page hash containing the child pages.
     */
    virtual PAGE_HASH GetChildren(ENTITY_ID parentId) = 0;

    /**
     * Adds an image to the image cache of the current page.
     * @return True if successful, false otherwise.
     */
    virtual bool AddImageToCurrentPage(QString imageName, QPixmap pixmap) = 0;

    /**
     * Retrieves an image from the database.
     * @return Pixmap.  This will be invalid in the event of an error.
     */
    virtual QPixmap GetImage(QString imageName) = 0;
};

#endif // ABSTRACTPLUGINSERVICES_H
