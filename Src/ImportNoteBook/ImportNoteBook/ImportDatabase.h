#ifndef IMPORTDATABASE_H
#define IMPORTDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include "../../NoteBook/NoteBook/PageData.h"
#include "../../NoteBook/NoteBook/Encrypter.h"


// This should work with any version of the database, since
// all we are retrieving fields that have been around since
// version 1.

class CImportDatabase : public QObject
{
	Q_OBJECT

public:
	CImportDatabase(QObject *parent);
	~CImportDatabase();

	bool OpenDatabase(const QString& databasePathname);
	void CloseDatabase();

	bool IsDatabaseOpen()	{ return m_db.isOpen(); }

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

	/*
	 *	Retrieves the specified page from the database.
	 *	Note that the content is returned as a string.  The caller must decide how to
	 *		use this string, based on the pageType.
	 *	@return Returns true if successful, false otherwise.
	 */
	bool GetPage(ENTITY_ID pageId, CPageData& pageData);

private:
	QSqlDatabase				m_db;				// Page database
	CEncrypter					m_encrypterObj;		// Encrypter object
};

#endif // IMPORTDATABASE_H
