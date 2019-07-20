#ifndef CLOGDATABASE_H
#define CLOGDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QByteArray>
#include <QTextDocument>
#include <QFlags>
#include "PageData.h"
#include "Encrypter.h"


#define errNoDateFound		-1
#define errSqliteError		-2

#define kDataTypeInteger	0
#define kDataTypeString		1
#define kDataTypeBlob		2

/* Current database version.  Stored in the globals table.
 * Version history:
 *  - Version 1 (this was not stored in the globals table) - Initial version.
 *  - Version 2 - Added datatype column in the globals table.
 *	- Version 3 - Added isfavorite column to the pages table.
 *  - Version 4 - Add itemid column to the additionaldata table.  This is now the key field.
*/
#define	kCurrentDatabaseVersion		4

// Global keys
#define kPageHistoryKey		"pagehistory"
#define kDatabaseVersionId	"databaseversion"
#define kPageOrderKey		"pageorder"

// Table names
#define kAdditionalDataTable    "additionaldata"

// Additional data types (for the Additional Data table)
#define kImageData          1


class CDatabase : public QObject
{
	Q_OBJECT

public:
	enum PageField
	{
		PageNoFields = 0x0,
		PageId = 0x1,
		ParentId = 0x2,
		PageTitle = 0x4,
		PageType = 0x8,
		PageContent = 0x10,
		PageTags = 0x20,
		PageModification = 0x40,
		PageCreation = 0x80,
		PageNumModifications = 0x100,
		PageAdditionalItems = 0x200
	};
    Q_DECLARE_FLAGS(PageFields, PageField)

    CDatabase(QObject *parent);

	virtual ~CDatabase();

	void SetParent(QObject* pParent)	{ setParent(pParent); }

	bool OpenDatabase(const QString& databasePathname);
	void CloseDatabase();

	bool IsDatabaseOpen()	{ return m_db.isOpen(); }

	/*
	 *	Sets the password for the current database in memory;
	 *		does not store this in the database.  This is used
	 *		when opening a database.
	 */
	void SetPasswordInMemory(const QString& password);

	/*
	 *	Stores the log password in the database.
	 *	@param encryptedPw The password, unencrypted.
	 *	@return Returns true if successful, false otherwise.
	 */
	bool StorePassword(const QString& pw);

	/*
	 *	Determines if the database is protected by a password.  This
	 *	is determined by checking that the password field contains data.
	 *	If the password field is empty, the database is not protected.
	 *	@return Returns true if protected, false otherwise.
	 */
	bool IsPasswordProtected();

	/*
	 *	Determines if the password passed in matches the one stored in the database.
	 *	@param pw The unencrypted password.
	 *	@return Returns true if the passwords match, false otherwise.
	 */
	bool PasswordMatch(const QString& pw);

	/*
	 *	Begins a database transaction.  Database modifications that occur within
	 *	transactions generally occur faster.
	 */
	bool BeginTransaction();

	/*
	 *	Ends a database transaction.  Database modifications that occur within
	 *	transactions generally occur faster.
	 */
	bool EndTransaction();

	/**
	 *	Sets a value in the globals table.
	 */
	bool SetGlobalValue(const QString& key, QVariant value);

	/**
	 *	Retrieves a value from the globals table.
	 */
	bool GetGlobalValue(const QString& key, QVariant& value);

	/**
	 *	Sets the page history in the globals table.
	 */
	bool SetPageHistory(const QString& pageHistoryStr);

	/**
	 *	Retrieves the page history from the globals table.
	 */
	bool GetPageHistory(QString& pageHistoryStr);

	/*
	 *	Adds a new folder to the database.
	 *	@return Returns the page ID of the newly added page, or kInvalidPageId (0) if
	 *		an error occurred.
	 */
	int AddNewFolder(CPageData& pageData);

	/*
	 *	Determines if a page exists.
	 */
	bool PageExists(ENTITY_ID pageId);

	/*
	 *	Adds a new page to the database.
	 *	@return Returns the page ID of the newly added entry, or kInvalidPageId (0) if
	 *		an error occurred.
	 */
	int AddNewPage(CPageData& pageData);

	/*
	 *	Adds a new blank page to the database.
	 *	@param pageData The page's data.
	 *		Only the following fields will be used:
	 *		- pageType Type of page
	 *		- parentId Id of the new item's parent, or kInvalidPageId (0) to make it
	 *			a top-level page.
	 *		- created date
	 *		- modified date
	 *		- pageTitle Tile of the page.
	 *	@return Returns the page ID of the newly added page, or kInvalidPageId (0) if
	 *		an error occurred.
	 */
	int AddNewBlankPage(CPageData& pageData);

	/*
	 *	Appends page data to an existing page.  Only the body and tags are
	 *	appended.  (TODO: may have to update additional Items, if the second
	 *	page contains images.)
	 *	@return Returns true if successful, false otherwise.
	 */
	bool AppendPage(ENTITY_ID existingPageId, CPageData& pageData);

	/*
	 *	Updates an existing page.  Replaces the content data.  This updates the current
	 *	modification date/time.  Note that the page type does not change.
	 *	If new additional items have been added, those items should be stored first,
	 *	before calling this function, so that their IDs are known.
	 */
	bool UpdatePage(CPageData& pageData);

	/*
	 *	Update the parent ID of a page.
	 */
	bool UpdatePageParent(ENTITY_ID pageId, ENTITY_ID newParentId);

	/*
	 *	Changes the title of a page.
	 *	@param pageId Page ID of the page to update.
	 *	@param newTitle New title of the page
	 *	@param bIsModification If true, the change is considered to be a "modification",
	 *			and the lastmodified and nummodifications fields are updated.  Thus,
	 *			the caller decides whether changing the title is considered a "modification".
	 */
	bool ChangePageTitle(ENTITY_ID pageId, const QString& newTitle, bool bIsModification);

	/**
	 *	Retrieve the title for a given page.
	 *	@param pageId Page ID of the page to get.
	 *	@param pageTitle If successful, contains the title of the
	 *			page on return.
	 *	@return Returns true if successful, false otherwise.
	 */
	bool GetPageTitle(ENTITY_ID pageId, QString& pageTitle);

	/*
	 *	Deletes a page.
	 */
	bool DeletePage(int pageId);

	/*
	 *	Retrieves the specified page from the database.
	 *	Note that the content is returned as a string.  The caller must decide how to
	 *		use this string, based on the pageType.
	 *	@return Returns true if successful, false otherwise.
	 */
	bool GetPage(ENTITY_ID pageId, CPageData& pageData);

	/*
	 *	Retrieves the requested fields of a page.
	 */
	bool GetPartialPage(ENTITY_ID pageId, CPageData& pageData, PageFields fields);

	/*
	 *	Gets a list of page IDs and their page titles.  This is used mainly
	 *	for populating the page cache when a database is opened.
	 *	@param pageHash On return, contains page IDs and their
	 *			associated CPageData object.  The CPageData object in this
	 *			case only contains the following items:
	 *			- parent ID
	 *			- page title
	 *			- page modification date
	 */
	bool GetPageList(PAGE_HASH& pageHash);

    /**
     * Retrieves the children of the given page ID (which must be a folder).
     *
     * @param pageHash  On return, contains page IDs and their
     *          associated CPageData object.  The CPageData object in this
     *          case only contains the following items:
     *          - parent ID
     *          - page title
     *          - page modification date
     * @param parentId  ID of parent
     * @return Returns true if successful, false otherwise.
     */
    bool GetChildren(PAGE_HASH& pageHash, ENTITY_ID parentId);

	/*
	 *	Gets a list of page IDs and their tags.  This is used mainly
	 *	for populating the tag cache when a database is opened.
	 *	@param
	 */
	bool GetTagList(PAGE_ID_TAG_CACHE& pageIdTagHash);

	/**
	 *	Gets the list of favorite pages.
	 *	@param pageIdList On return, contains the page IDs of the favorite pages.
	 */
	bool GetFavoritePages(ENTITY_LIST& pageIdList);

	/**
	 *	Sets the favorite status for the page.
	 *	@param pageId The page to set.
	 *	@param bIsFavorite True if the page is to be a favorite, false otherwise.
	 */
	bool SetPageFavoriteStatus(ENTITY_ID pageId, bool bIsFavorite);

	/*
	 *	Sets the page order for the notebook.  The page order is a string
	 *	that lists the order of all pages in the notebook, traversed in a
	 *	depth-first order.  Folders are terminated by an ID of 0.
	 */
	bool SetPageOrder(const QString& pageOrderStr);

	/*
	 *	Retrieves the page order string for the notebook.
	 */
	bool GetPageOrder(QString& pageOrderStr);

    /**
     * @brief Adds an image to the additionaldata table.
     * @param imageName UUID representing the image name.
     * @param pixmap
     * @param parentPageId ID of page where the image appears.
     * @return True if successful, false otherwise.
     */
    bool AddImage(QString imageName, QPixmap pixmap, ENTITY_ID parentPageId);

    /**
     * @brief Retrieves the specified image.
     * @param imageName UUID representing the image name.
     * @param pixmap On return, the pixmap (if no error).
     * @return True if successful, false otherwise.
     */
    bool GetImage(QString imageName, QPixmap& pixmap);

    /**
     * @brief Returns a list of image names (UUIDs) for all the images that are present in the
     * given page.
     * @param pageId
     * @param nameList a string list of the names of all images contained
     *          in the given page.
     * @return True if successful, false otherwise.
     */
    bool GetImageNamesForPage(ENTITY_ID pageId, QStringList &nameList);

    /**
     * @brief Deletes all images contained in the given page.
     * @param pageId
     * @return True if successful, false otherwise.
     */
    bool DeleteAllImagesForPage(ENTITY_ID pageId);

private:
    CDatabase(CDatabase const&) : QObject(NULL) {}                // Copy constructor is private
    CDatabase& operator=(CDatabase const&) { return *this; }		// Assignment operator is private

	void CreateTables();

	ENTITY_ID NextPageId();
	ENTITY_ID NextImageId();

	int AddNewContent(const QTextDocument* contentData, int parentLogIndex);

	void UpdateDatabase();
	void UpdateDbVersion1To2();
	void UpdateDbVersion2To3();
    void UpdateDbVersion3To4();

	bool AddColumnToTable(const QString& tableName, const QString columnNameAndDef);

private:
	QSqlDatabase				m_db;				// Page database
	CEncrypter					m_encrypterObj;		// Encrypter object
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CDatabase::PageFields)

#endif // CLOGDATABASE_H
