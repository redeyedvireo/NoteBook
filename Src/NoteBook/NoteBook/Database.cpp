#include "stdafx.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QStringList>
#include <QMessageBox>
#include <QDebug>
#include "Database.h"


const int	kError = -1;


//--------------------------------------------------------------------------
CDatabase::CDatabase(QObject *parent)
	: QObject(parent)
{
}

//--------------------------------------------------------------------------
CDatabase::~CDatabase()
{
}

//--------------------------------------------------------------------------
bool CDatabase::OpenDatabase(const QString& databasePathname)
{
	if (m_db.isOpen())
		return true;

	// Determine if database file exists
	bool		bReturn = false;
	bool		bDbFileExists;
	QFileInfo	dbFileInfo(databasePathname);

	bDbFileExists = dbFileInfo.exists();

	m_db = QSqlDatabase::addDatabase("QSQLITE", "NotebookDatabase");
	m_db.setDatabaseName(databasePathname);

	if (m_db.open())
	{
		if (! bDbFileExists)
		{
			// Database file was not present.  It has just been created.  Create the tables.
			CreateTables();
		}
		else
		{
			// Update if necessary
			UpdateDatabase();
		}

		bReturn = true;
	}
	else
	{
		QMessageBox::critical(NULL, "Database Error", "Could not open database");

		QSqlError	dbErr = m_db.lastError();
		QMessageBox::information(NULL, "Driver text", dbErr.driverText());
		QMessageBox::information(NULL, "Database text", dbErr.databaseText());
	}

	return bReturn;
}

//--------------------------------------------------------------------------
void CDatabase::CloseDatabase()
{
	m_db.close();
	m_encrypterObj.ClearPassword();
}

//--------------------------------------------------------------------------
void CDatabase::CreateTables()
{
	QSqlQuery	queryObj(m_db);
	QString		createStr;

	// Create globals table, to hold database-specific data
	// This will be a series of key/value pairs.  Note that there are
	// several columns, each with a different type.  Only one of
	// these will be used, for any given key/value pair.
	createStr = "create table globals (";
	createStr += "key text UNIQUE, ";
	createStr += "datatype int, ";
	createStr += "intval int, ";
	createStr += "stringval text, ";
	createStr += "blobval blob";
	createStr += ")";

	queryObj.prepare(createStr);
	queryObj.exec();

	// Initialize global data
	createStr = "insert into globals (key, blobval) values (\"encrpw\", \"\")";
	queryObj.prepare(createStr);
	queryObj.exec();

	// Create pages table - table to hold main text of page entries
	createStr = "create table pages (";
	createStr += "pageid integer UNIQUE, ";			// Unique Page ID (must not be 0)
	createStr += "parentid integer, ";				// Page ID of this page's parent page
	createStr += "created integer, ";				// Date and time the page was created, as a time_t
	createStr += "lastmodified integer, ";			// Date and time page was last modified, as a time_t
	createStr += "nummodifications integer, ";		// Number of modifications made to the page
	createStr += "pagetype integer, ";				// Type of page (0=user text, 1=folder, 2=HTML, 3=Javascript)
	createStr += "pagetitle blob, ";				// Title of page.  Must be blob to hold encrypted data
	createStr += "contents blob, ";					// Must be blob to hold encrypted data
	createStr += "tags blob, ";						// Must be blob to hold encrypted data
	createStr += "additionalitems text, ";			// Additional items needed by this page.  A string of comma-separated values
	createStr += "isfavorite integer default 0 ";	// 1 if the page is a "favorite", 0 if not
	createStr += ")";

	queryObj.prepare(createStr);
	queryObj.exec();

	// Create the Additional Data table.  This table holds supplementary data
	// for pages, such as images, audio clips and video clips.
    createStr = QString("create table %1 (").arg(kAdditionalDataTable);
    createStr += "itemid text UNIQUE, ";		// Unique ID for this data item (will be a UUID)
    createStr += "type integer, ";				// Type of data (1=image, 2=audio clip, 3=video clip)
	createStr += "contents blob, ";				// Data contents.  Can be anything.
	createStr += "parentid integer ";			// ID of its containing content data entry (not sure if this is really needed)
	createStr += ")";

	queryObj.prepare(createStr);
	queryObj.exec();
}

//--------------------------------------------------------------------------
void CDatabase::UpdateDatabase()
{
	// Read database version (stored as a global value).  If there is no database
	// version in the globals, then it is version 1.
	// If the version of database is less than kCurrentDatabaseVersion, then
	// continue with this function.  Otherwise, just return.

	QVariant		databaseVersionVar;
	int				storedDatabaseVersion = 1;

	if (GetGlobalValue(kDatabaseVersionId, databaseVersionVar))
	{
		storedDatabaseVersion = databaseVersionVar.toInt();
	}

	for (int versionNum = storedDatabaseVersion; versionNum < kCurrentDatabaseVersion; versionNum++)
	{
		switch (versionNum)
		{
		case 1:
			UpdateDbVersion1To2();
			break;

		case 2:
			UpdateDbVersion2To3();
			break;

        case 3:
            UpdateDbVersion3To4();
            break;

		default:
			break;
		}
	}
}

//--------------------------------------------------------------------------
void CDatabase::UpdateDbVersion1To2()
{
	// The only difference between version 1 and version 2 databases is
	// that the globals table in version 2 has a datatype column.
	AddColumnToTable("globals", "datatype int");

	// Update database version value
	SetGlobalValue(kDatabaseVersionId, 2);
}

//--------------------------------------------------------------------------
void CDatabase::UpdateDbVersion2To3()
{
	// Add isfavorite column to pages table
	AddColumnToTable("pages", "isfavorite integer default 0");

	// Update database version value
	SetGlobalValue(kDatabaseVersionId, 3);
}

//--------------------------------------------------------------------------
void CDatabase::UpdateDbVersion3To4()
{
    // Add itemid column to additionaldata table.  The existing dataid column will
    // remain (it has never been used up to this point).
    AddColumnToTable("additionaldata", "itemid text");

    // Update database version value
    SetGlobalValue(kDatabaseVersionId, 4);
}

//--------------------------------------------------------------------------
bool CDatabase::AddColumnToTable(const QString& tableName, const QString columnNameAndDef)
{
	// Add the columns to the feed table
	QSqlQuery	queryObj(m_db);
	QString		queryString;
	bool		bReturn = true;

	queryString = QString("alter table %1 add column %2").arg(tableName).arg(columnNameAndDef);

	queryObj.prepare(queryString);

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to add column " << columnNameAndDef << ": " << sqlErr.text();
		bReturn = false;
	}

	return bReturn;
}

//--------------------------------------------------------------------------
ENTITY_ID CDatabase::NextImageId()
{
	QSqlQuery	queryObj(m_db);
	QString		createStr;
	int			nextId = kInvalidPageId;

	createStr = "select max(dataid) as maxdataid from additionaldata";

	queryObj.prepare(createStr);
	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		return kInvalidPageId;
	}

	int fieldNo = queryObj.record().indexOf("maxdataid");
	if (queryObj.next())
	{
		nextId = queryObj.value(fieldNo).toInt();
	}

	return nextId + 1;
}

//--------------------------------------------------------------------------
ENTITY_ID CDatabase::NextPageId()
{
	QSqlQuery	queryObj(m_db);
	QString		createStr;
	ENTITY_ID	nextId = kInvalidPageId;

	createStr = "select max(pageid) as maxpageid from pages";

	queryObj.prepare(createStr);
	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to retrieve the next page ID: " << sqlErr.text();

		return kInvalidPageId;
	}

	int fieldNo = queryObj.record().indexOf("maxpageid");
	if (queryObj.next())
	{
		nextId = queryObj.value(fieldNo).toInt();
	}

	return nextId + 1;
}

//--------------------------------------------------------------------------
void CDatabase::SetPasswordInMemory(const QString& password)
{
	m_encrypterObj.SetPassword(password);
}

//--------------------------------------------------------------------------
bool CDatabase::StorePassword(const QString& pw)
{
	QSqlQuery		queryObj(m_db);
	bool			bRetVal = false;

	m_encrypterObj.SetPassword(pw);

	queryObj.prepare("update globals set blobval=? where key=\"encrpw\"");

	queryObj.bindValue(0, QVariant(m_encrypterObj.HashedPassword()));

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to update the encrypted password: " << sqlErr.text();
		bRetVal = false;
	}
	else
	{
		bRetVal = true;
	}

	return bRetVal;
}

//--------------------------------------------------------------------------
bool CDatabase::IsPasswordProtected()
{
	QSqlQuery	queryObj(m_db);
	bool		bIsProtected = false;

	queryObj.prepare("select blobval from globals where key=\"encrpw\"");

	queryObj.exec();

	if (queryObj.next())
	{
		int pwField = queryObj.record().indexOf("blobval");

		QByteArray pwData = queryObj.value(pwField).toByteArray();

		if (pwData.size() > 0)
		{
			bIsProtected = true;
		}
	}

	return bIsProtected;
}

//--------------------------------------------------------------------------
bool CDatabase::PasswordMatch(const QString& pw)
{
	QSqlQuery	queryObj(m_db);
	QByteArray	hashedPw;
	bool		bMatch = false;

	hashedPw = m_encrypterObj.HashPassword(pw);

	queryObj.prepare("select blobval from globals where key=\"encrpw\"");

	queryObj.exec();

	if (queryObj.next())
	{
		int pwField = queryObj.record().indexOf("blobval");

		QByteArray pwData = queryObj.value(pwField).toByteArray();

		if (pwData == hashedPw)
		{
			bMatch = true;
		}
	}

	return bMatch;
}

//--------------------------------------------------------------------------
bool CDatabase::BeginTransaction()
{
	QSqlQuery	queryObj(m_db);

	queryObj.prepare("BEGIN TRANSACTION");
	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to begin transaction: " << sqlErr.text();
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------
bool CDatabase::EndTransaction()
{
	QSqlQuery	queryObj(m_db);

	queryObj.prepare("END TRANSACTION");
	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to end transaction: " << sqlErr.text();
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------
bool CDatabase::SetGlobalValue( const QString& key, QVariant value )
{
	QSqlQuery	queryObj(m_db);
	QString		createStr;
	int			dataType;

	// See if the key exists
	queryObj.prepare("select datatype from globals where key = ?");
	queryObj.addBindValue(QVariant(key));

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to determine if a global value exists: " << sqlErr.text();
		return false;
	}

	if (queryObj.next())
	{
		// Key exists - update its value
		if (value.type() == QVariant::Int || value.type() == QVariant::UInt)
		{
			createStr = "update globals set intval=? where key=?";
		}
		else if (value.type() == QVariant::String)
		{
			createStr = "update globals set stringval=? where key=?";
		}
		else if (value.type() == QVariant::ByteArray)
		{
			createStr = "update globals set blobval=? where key=?";
		}
		else
		{
			// Unknown data type
			Q_ASSERT(false);
			return false;
		}

		queryObj.prepare(createStr);

		queryObj.addBindValue(value);
		queryObj.addBindValue(QVariant(key));
	}
	else
	{
		// Key does not exist - add the key and value

		if (value.type() == QVariant::Int || value.type() == QVariant::UInt)
		{
			createStr = "insert into globals (key, datatype, intval) values (?, ?, ?)";
			dataType = kDataTypeInteger;
		}
		else if (value.type() == QVariant::String)
		{
			createStr = "insert into globals (key, datatype, stringval) values (?, ?, ?)";
			dataType = kDataTypeString;
		}
		else if (value.type() == QVariant::ByteArray)
		{
			createStr = "insert into globals (key, datatype, blobval) values (?, ?, ?)";
			dataType = kDataTypeBlob;
		}
		else
		{
			// Unknown data type
			Q_ASSERT(false);
			return false;
		}

		queryObj.prepare(createStr);

		queryObj.addBindValue(QVariant(key));
		queryObj.addBindValue(QVariant(dataType));
		queryObj.addBindValue(value);
	}

	queryObj.exec();

	// Check for errors
	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to set a global value: " << sqlErr.text();
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------
bool CDatabase::GetGlobalValue( const QString& key, QVariant& value )
{
	QSqlQuery	queryObj(m_db);
	QString		createStr;
	int			dataType;

	queryObj.prepare("select datatype from globals where key = ?");
	queryObj.bindValue(0, QVariant(key));

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to retrieve a global value: " << sqlErr.text();
		return false;
	}

	if (queryObj.next())
	{
		int typeField = queryObj.record().indexOf("datatype");

		dataType = queryObj.value(typeField).toInt();
	}
	else
	{
		// key not found
		return false;
	}


	if (dataType == kDataTypeInteger)
	{
		createStr = "select intval from globals where key=?";
	}
	else if (dataType == kDataTypeString)
	{
		createStr = "select stringval from globals where key=?";
	}
	else if (dataType == kDataTypeBlob)
	{
		createStr = "select blobval from globals where key=?";
	}
	else
	{
		// Unknown data type
		Q_ASSERT(false);
		return false;
	}

	queryObj.prepare(createStr);

	queryObj.bindValue(0, QVariant(key));

	queryObj.exec();

	// Check for errors
	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to retrieve a page: " << sqlErr.text();
		return false;
	}

	if (queryObj.next())
	{
		int valueField;

		if (dataType == kDataTypeInteger)
		{
			valueField = queryObj.record().indexOf("intval");
		}
		else if (dataType == kDataTypeString)
		{
			valueField = queryObj.record().indexOf("stringval");
		}
		else if (dataType == kDataTypeBlob)
		{
			valueField = queryObj.record().indexOf("blobval");
		}

		value = queryObj.value(valueField);
	}

	return true;
}

//--------------------------------------------------------------------------
bool CDatabase::SetPageHistory( const QString& pageHistoryStr )
{
	return SetGlobalValue(kPageHistoryKey, pageHistoryStr);
}

//--------------------------------------------------------------------------
bool CDatabase::GetPageHistory( QString& pageHistoryStr )
{
	bool		bReturn = false;
	QVariant	pageHistoryVar;

	if (GetGlobalValue(kPageHistoryKey, pageHistoryVar))
	{
		pageHistoryStr = pageHistoryVar.toString();
		bReturn = true;
	}

	return bReturn;
}

//--------------------------------------------------------------------------
bool CDatabase::SetPageOrder(const QString& pageOrderStr)
{
	return SetGlobalValue(kPageOrderKey, pageOrderStr);
}

//--------------------------------------------------------------------------
bool CDatabase::GetPageOrder(QString& pageOrderStr)
{
	bool		bReturn = false;
	QVariant	pageOrderVar;

	if (GetGlobalValue(kPageOrderKey, pageOrderVar))
	{
		pageOrderStr = pageOrderVar.toString();
		bReturn = true;
	}

	return bReturn;
}

//--------------------------------------------------------------------------
int CDatabase::AddNewFolder(CPageData& pageData)
{
	// This operation is the same as AddNewBlankPage.  I am keeping
	// this function as a wrapper in case it needs to be expanded
	// in the future.
	return AddNewBlankPage(pageData);
}

//--------------------------------------------------------------------------
bool CDatabase::PageExists(ENTITY_ID pageId)
{
	QSqlQuery	queryObj(m_db);

	queryObj.prepare("select pageid from pages where pageid=?");

	queryObj.bindValue(0, QVariant(pageId));

	queryObj.exec();

	return (queryObj.next());
}

//--------------------------------------------------------------------------
int CDatabase::AddNewPage(CPageData& pageData)
{
	QSqlQuery	queryObj(m_db);
	QByteArray	contentData, titleData, tagData;
	int			pageId = kInvalidPageId;

	pageId = NextPageId();

	if (pageId == kInvalidPageId)
	{
		return kInvalidPageId;
	}

	if (m_encrypterObj.HasPassword())
	{
		contentData = m_encrypterObj.EncryptString(pageData.m_contentString);
		tagData = m_encrypterObj.EncryptString(pageData.m_tags);
		titleData = m_encrypterObj.EncryptString(pageData.m_title);
	}
	else
	{
		contentData = pageData.m_contentString.toUtf8();
		tagData = pageData.m_tags.toUtf8();
		titleData = pageData.m_title.toUtf8();
	}

	queryObj.prepare("insert into pages (pageid, parentid, created, lastModified, numModifications, pagetype, pagetitle, contents, tags, additionalitems, isfavorite) "
						"values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

	queryObj.addBindValue(QVariant(pageId));
	queryObj.addBindValue(QVariant(pageData.m_parentId));
	queryObj.addBindValue(QVariant(pageData.CreatedTimeAsTimeT()));
	queryObj.addBindValue(QVariant(pageData.CreatedTimeAsTimeT()));		// Last modified date and time is same as created date/time for a new page
	queryObj.addBindValue(QVariant(pageData.m_numModifications));
	queryObj.addBindValue(QVariant(pageData.m_pageType));
	queryObj.addBindValue(QVariant(titleData));
	queryObj.addBindValue(QVariant(contentData));
	queryObj.addBindValue(QVariant(tagData));
	queryObj.addBindValue(QVariant(pageData.AdditionalItemsAsCommasSeparatedList()));
	queryObj.addBindValue(QVariant(pageData.m_bIsFavorite ? 1 : 0));

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to store a new page: " << sqlErr.text();
		return kInvalidPageId;
	}

	return pageId;
}

//--------------------------------------------------------------------------
int CDatabase::AddNewBlankPage(CPageData& pageData)
{
	QSqlQuery	queryObj(m_db);
	QByteArray	contentData, titleData, tagData;
	int			numModifications = 0;		// Don't count this as a modification
	int			pageId = kInvalidPageId;

	pageId = NextPageId();

	if (pageId == kInvalidPageId)
	{
		return kInvalidPageId;
	}

	if (m_encrypterObj.HasPassword())
	{
		titleData = m_encrypterObj.EncryptString(pageData.m_title);
	}
	else
	{
		titleData = pageData.m_title.toUtf8();
	}

	queryObj.prepare("insert into pages (pageid, parentid, created, lastModified, numModifications, pagetype, pagetitle) "
		"values (?, ?, ?, ?, ?, ?, ?)");

	queryObj.addBindValue(QVariant(pageId));
	queryObj.addBindValue(QVariant(pageData.m_parentId));
	queryObj.addBindValue(QVariant(pageData.CreatedTimeAsTimeT()));
	queryObj.addBindValue(QVariant(pageData.CreatedTimeAsTimeT()));		// Last modified date and time is same as created date/time for a new page
	queryObj.addBindValue(QVariant(numModifications));
	queryObj.addBindValue(QVariant(pageData.m_pageType));
	queryObj.addBindValue(QVariant(titleData));


	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to store a new blank page: " << sqlErr.text();
		return kInvalidPageId;
	}

	return pageId;
}

//--------------------------------------------------------------------------
bool CDatabase::AppendPage( ENTITY_ID existingPageId, CPageData& pageData )
{
	bool			bRetVal = false;
	CPageData		existingPage;

	if (GetPage(existingPageId, existingPage))
	{
		QTextDocument	tempDoc;

		tempDoc.setHtml(existingPage.m_contentString);

		QTextCursor		textCursor(&tempDoc);

		textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
		textCursor.insertHtml("<br><hr /><br>");
		textCursor.insertHtml(pageData.m_contentString);

		existingPage.m_contentString = tempDoc.toHtml();

		// Append tags
		existingPage.AppendTags(pageData.m_tags);

		// Append Additional Items
		existingPage.AppendAdditionalItems(pageData.AdditionalItemsAsCommasSeparatedList());

		bRetVal = UpdatePage(existingPage);
	}

	return bRetVal;

}

//--------------------------------------------------------------------------
bool CDatabase::UpdatePage(CPageData& pageData)
{
	QSqlQuery		queryObj(m_db);
	CPageData		existingPage;
	bool			bRetVal = false;

	if (GetPage(pageData.m_pageId, existingPage))
	{
		QByteArray	contentData;
		QByteArray	tagData;
		QByteArray	titleData;

		// Update modification data
		existingPage.m_numModifications++;
		existingPage.m_modifiedDateTime = QDateTime::currentDateTime();

		// Encrypt content data
		if (m_encrypterObj.HasPassword())
		{
			contentData = m_encrypterObj.EncryptString(pageData.m_contentString);
			tagData = m_encrypterObj.EncryptString(pageData.m_tags);
			titleData = m_encrypterObj.EncryptString(pageData.m_title);
		}
		else
		{
			contentData = pageData.m_contentString.toUtf8();
			tagData = pageData.m_tags.toUtf8();
			titleData = pageData.m_title.toUtf8();
		}

		queryObj.prepare("update pages set contents=?, pagetitle=?, tags=?, lastmodified=?, nummodifications=?, additionalitems=?, isfavorite=? where pageid=?");

		queryObj.addBindValue(QVariant(contentData));
		queryObj.addBindValue(QVariant(titleData));
		queryObj.addBindValue(QVariant(tagData));
		queryObj.addBindValue(QVariant(existingPage.ModifiedTimeAsTimeT()));
		queryObj.addBindValue(QVariant(existingPage.m_numModifications));
		queryObj.addBindValue(QVariant(pageData.AdditionalItemsAsCommasSeparatedList()));
		queryObj.addBindValue(QVariant(pageData.m_bIsFavorite ? 1 : 0));
		queryObj.addBindValue(QVariant(pageData.m_pageId));

		queryObj.exec();

		// Check for errors
		QSqlError	sqlErr;

		sqlErr = m_db.lastError();
		if (sqlErr.type() != QSqlError::NoError)
		{
			// Should probably log this error
			qDebug() << "Error when attempting to update a page: " << sqlErr.text();
			bRetVal = false;
		}
		else
		{
			bRetVal = true;
		}
	}

	return bRetVal;
}

//--------------------------------------------------------------------------
bool CDatabase::UpdatePageParent( ENTITY_ID pageId, ENTITY_ID newParentId )
{
	QSqlQuery		queryObj(m_db);
	CPageData		existingPage;
	bool			bRetVal = false;

	if (PageExists(pageId))
	{
		queryObj.prepare("update pages set parentid=? where pageid=?");

		queryObj.addBindValue(QVariant(newParentId));
		queryObj.addBindValue(QVariant(pageId));

		queryObj.exec();

		// Check for errors
		QSqlError	sqlErr;

		sqlErr = m_db.lastError();
		if (sqlErr.type() != QSqlError::NoError)
		{
			// Should probably log this error
			qDebug() << "Error when attempting to update a page's parent ID: " << sqlErr.text();
			bRetVal = false;
		}
		else
		{
			bRetVal = true;
		}
	}

	return bRetVal;
}

//--------------------------------------------------------------------------
bool CDatabase::ChangePageTitle( ENTITY_ID pageId, const QString& newTitle, bool bIsModification )
{
	QSqlQuery		queryObj(m_db);
	QByteArray		titleData;
	bool			bRetVal = false;

	// Encrypt title, if necessary
	if (m_encrypterObj.HasPassword())
	{
		titleData = m_encrypterObj.EncryptString(newTitle);
	}
	else
	{
		titleData = newTitle.toUtf8();
	}

	if (bIsModification)
	{
		CPageData		existingPage;

		// This is considered to be a modification, so need to get the current
		// value of nummodifications
		if (! GetPartialPage(pageId, existingPage, PageNumModifications))
		{
			// Error retrieving number of modifications
			return false;
		}

		// Update modification data
		existingPage.m_numModifications++;
		existingPage.m_modifiedDateTime = QDateTime::currentDateTime();


		queryObj.prepare("update pages set pagetitle=?, lastmodified=?, nummodifications=? where pageid=?");

		queryObj.addBindValue(QVariant(titleData));
		queryObj.addBindValue(QVariant(existingPage.ModifiedTimeAsTimeT()));
		queryObj.addBindValue(QVariant(existingPage.m_numModifications));
		queryObj.addBindValue(QVariant(pageId));
	}
	else
	{
		// This is not considered a "modification", so just change the title
		queryObj.prepare("update pages set pagetitle=?, where pageid=?");

		queryObj.addBindValue(QVariant(titleData));
		queryObj.addBindValue(QVariant(pageId));
	}

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to update page title: " << sqlErr.text();
		bRetVal = false;
	}
	else
	{
		bRetVal = true;
	}

	return bRetVal;
}

//--------------------------------------------------------------------------
bool CDatabase::GetPageTitle( ENTITY_ID pageId, QString& pageTitle )
{
	QSqlQuery		queryObj(m_db);

	queryObj.prepare("select pagetitle from pages where pageid=?");

	queryObj.bindValue(0, QVariant(pageId));

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to retrieve a page title: " << sqlErr.text();
		return false;
	}

	if (queryObj.next())
	{
		int pageTitleField = queryObj.record().indexOf("pagetitle");

		QByteArray titleData = queryObj.value(pageTitleField).toByteArray();
		pageTitle = titleData;

		return true;
	}

	return false;
}

//--------------------------------------------------------------------------
bool CDatabase::DeletePage( int pageId )
{
	QSqlQuery	queryObj(m_db);

	queryObj.prepare("delete from pages where pageid=?");

	queryObj.bindValue(0, QVariant(pageId));

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to delete a page: " << sqlErr.text();
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------
bool CDatabase::GetPage(ENTITY_ID pageId, CPageData& pageData)
{
	QSqlQuery	queryObj(m_db);

	queryObj.prepare("select pagetype, parentid, contents, pagetitle, tags, created, lastmodified, nummodifications, additionalitems, isfavorite from pages where pageid=?");

	queryObj.bindValue(0, QVariant(pageId));

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to retrieve a page: " << sqlErr.text();
		return false;
	}

	if (queryObj.next())
	{
		int pageTypeField = queryObj.record().indexOf("pagetype");
		int parentIdField = queryObj.record().indexOf("parentid");
		int contentsField = queryObj.record().indexOf("contents");
		int pageTitleField = queryObj.record().indexOf("pagetitle");
		int tagsField = queryObj.record().indexOf("tags");
		int createdField = queryObj.record().indexOf("created");
		int lastModifiedDateField = queryObj.record().indexOf("lastmodified");
		int numModificationsField = queryObj.record().indexOf("nummodifications");
		int additionalItemsField = queryObj.record().indexOf("additionalitems");
		int isFavoriteField = queryObj.record().indexOf("isfavorite");

		int pageTypeInt = queryObj.value(pageTypeField).toInt();
		ENTITY_ID parentId = queryObj.value(parentIdField).toUInt();
		QByteArray contentsData = queryObj.value(contentsField).toByteArray();
		QByteArray titleData = queryObj.value(pageTitleField).toByteArray();
		QByteArray tagData = queryObj.value(tagsField).toByteArray();
		int lastModifiedTime_t = queryObj.value(lastModifiedDateField).toInt();
		int createdTime_t = queryObj.value(createdField).toInt();
		QString additionalItemsStr = queryObj.value(additionalItemsField).toString();
		bool bIsFavorite = queryObj.value(isFavoriteField).toUInt() != 0;

		if (m_encrypterObj.HasPassword())
		{
			pageData.m_contentString = m_encrypterObj.DecryptString(contentsData);
			pageData.m_title = m_encrypterObj.DecryptString(titleData);
			pageData.m_tags = m_encrypterObj.DecryptString(tagData);
		}
		else
		{
			// No password
			pageData.m_contentString = QString::fromUtf8(contentsData);
			pageData.m_title = QString::fromUtf8(titleData);
			pageData.m_tags = QString::fromUtf8(tagData);
		}

		pageData.m_pageId = pageId;
		pageData.m_pageType = PAGE_TYPE(pageTypeInt);
		pageData.m_parentId = parentId;
		pageData.m_modifiedDateTime = QDateTime::fromTime_t(lastModifiedTime_t);
		pageData.m_createdDateTime = QDateTime::fromTime_t(createdTime_t);
		pageData.m_numModifications = queryObj.value(numModificationsField).toInt();
		pageData.SetAdditionalItemsFromCommaSeparatedList(additionalItemsStr);
		pageData.m_bIsFavorite = bIsFavorite;
	}
	else
		return false;		// No entry found

	return true;
}

//--------------------------------------------------------------------------
bool CDatabase::GetPartialPage( ENTITY_ID pageId, CPageData& pageData, PageFields fields )
{
	bool		bRetVal = false;
	QSqlQuery	queryObj(m_db);
	QString		requestString;
	QStringList	requestItems;

	if (fields.testFlag(ParentId))
	{
		requestItems << "parentid";
	}

	if (fields.testFlag(PageTitle))
	{
		requestItems << "pagetitle";
	}

	if (fields.testFlag(PageType))
	{
		requestItems << "pagetype";
	}

	if (fields.testFlag(PageContent))
	{
		requestItems << "contents";
	}

	if (fields.testFlag(PageTags))
	{
		requestItems << "tags";
	}

	if (fields.testFlag(PageModification))
	{
		requestItems << "lastmodified";
	}

	if (fields.testFlag(PageCreation))
	{
		requestItems << "created";
	}

	if (fields.testFlag(PageNumModifications))
	{
		requestItems << "nummodifications";
	}

	if (fields.testFlag(PageAdditionalItems))
	{
		requestItems << "additionalitems";
	}

	requestString = QString("select %1 from pages where pageid=?").arg(requestItems.join(", "));

	queryObj.prepare(requestString);
	queryObj.bindValue(0, QVariant(pageId));

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "CDatabase::GetPartialPage Error when retrieving page data: " << sqlErr.text();
		return false;
	}

	if (queryObj.next())
	{
		bRetVal = true;
		pageData.m_pageId = pageId;

		if (fields.testFlag(ParentId))
		{
			ENTITY_ID parentId = queryObj.value(queryObj.record().indexOf("parentid")).toUInt();
			pageData.m_parentId = parentId;
		}

		if (fields.testFlag(PageTitle))
		{
			QByteArray titleData = queryObj.value(queryObj.record().indexOf("pagetitle")).toByteArray();
			if (m_encrypterObj.HasPassword())
			{
				pageData.m_title = m_encrypterObj.DecryptString(titleData);
			}
			else
			{
				// No password
				pageData.m_title = QString::fromUtf8(titleData);
			}
		}

		if (fields.testFlag(PageType))
		{
			int pageTypeInt = queryObj.value(queryObj.record().indexOf("pagetype")).toInt();
			pageData.m_pageType = PAGE_TYPE(pageTypeInt);
		}

		if (fields.testFlag(PageContent))
		{
			QByteArray contentsData = queryObj.value(queryObj.record().indexOf("contents")).toByteArray();
			if (m_encrypterObj.HasPassword())
			{
				pageData.m_contentString = m_encrypterObj.DecryptString(contentsData);
			}
			else
			{
				// No password
				pageData.m_contentString = QString::fromUtf8(contentsData);
			}
		}

		if (fields.testFlag(PageTags))
		{
			QByteArray tagData = queryObj.value(queryObj.record().indexOf("tags")).toByteArray();
			if (m_encrypterObj.HasPassword())
			{
				pageData.m_tags = m_encrypterObj.DecryptString(tagData);
			}
			else
			{
				// No password
				pageData.m_tags = QString::fromUtf8(tagData);
			}
		}

		if (fields.testFlag(PageModification))
		{
			int lastModifiedTime_t = queryObj.value(queryObj.record().indexOf("lastmodified")).toInt();
			pageData.m_modifiedDateTime = QDateTime::fromTime_t(lastModifiedTime_t);
		}

		if (fields.testFlag(PageCreation))
		{
			int createdTime_t = queryObj.value(queryObj.record().indexOf("created")).toInt();
			pageData.m_createdDateTime = QDateTime::fromTime_t(createdTime_t);
		}

		if (fields.testFlag(PageNumModifications))
		{
			pageData.m_numModifications = queryObj.value(queryObj.record().indexOf("nummodifications")).toInt();
		}

		if (fields.testFlag(PageAdditionalItems))
		{
			QString additionalItemsStr = queryObj.value(queryObj.record().indexOf("additionalitems")).toString();
			pageData.SetAdditionalItemsFromCommaSeparatedList(additionalItemsStr);
		}
	}

	return bRetVal;
}

//--------------------------------------------------------------------------
bool CDatabase::GetPageList(PAGE_HASH& pageHash)
{
	QSqlQuery	queryObj(m_db);

    queryObj.prepare("select pageid, parentid, pagetitle, lastmodified, pagetype from pages order by pagetitle asc");

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to retrieve page IDs and titles: " << sqlErr.text();
		return false;
	}

	pageHash.clear();

	while (queryObj.next())
	{
		int	pageIdField = queryObj.record().indexOf("pageid");
		int parentIdField = queryObj.record().indexOf("parentid");
		int pageTitleField = queryObj.record().indexOf("pagetitle");
		int lastModifiedField = queryObj.record().indexOf("lastmodified");
		int pageTypeField = queryObj.record().indexOf("pagetype");

		ENTITY_ID pageId = queryObj.value(pageIdField).toUInt();
		QByteArray titleData = queryObj.value(pageTitleField).toByteArray();
		QString		pageTitle;

		if (m_encrypterObj.HasPassword())
		{
			pageTitle = m_encrypterObj.DecryptString(titleData);
		}
		else
		{
			pageTitle = QString(titleData);
		}

		SHARED_CPAGEDATA	newPageDataObj = SHARED_CPAGEDATA(new (std::nothrow) CPageData);

		if (! newPageDataObj.isNull())
		{
			newPageDataObj.data()->m_pageId = pageId;
			newPageDataObj.data()->m_parentId = queryObj.value(parentIdField).toUInt();
			newPageDataObj.data()->m_modifiedDateTime = QDateTime::fromTime_t(queryObj.value(lastModifiedField).toInt());
			newPageDataObj.data()->m_title = pageTitle;
			newPageDataObj.data()->m_pageType = (PAGE_TYPE) queryObj.value(pageTypeField).toInt();

			pageHash.insert(pageId, newPageDataObj);
		}
	}

    return true;
}

//--------------------------------------------------------------------------
bool CDatabase::GetChildren(PAGE_HASH &pageHash, ENTITY_ID parentId)
{
    QSqlQuery	queryObj(m_db);

    queryObj.prepare("select pageid, parentid, pagetitle, lastmodified, pagetype from pages where parentid=?");

    queryObj.bindValue(0, QVariant(parentId));

    queryObj.exec();

    // Check for errors
    QSqlError	sqlErr;

    sqlErr = m_db.lastError();
    if (sqlErr.type() != QSqlError::NoError)
    {
        // Should probably log this error
        qDebug() << "Error when attempting to retrieve page IDs and titles: " << sqlErr.text();
        return false;
    }

    pageHash.clear();

    while (queryObj.next())
    {
        int	pageIdField = queryObj.record().indexOf("pageid");
        int parentIdField = queryObj.record().indexOf("parentid");
        int pageTitleField = queryObj.record().indexOf("pagetitle");
        int lastModifiedField = queryObj.record().indexOf("lastmodified");
        int pageTypeField = queryObj.record().indexOf("pagetype");

        ENTITY_ID pageId = queryObj.value(pageIdField).toUInt();
        QByteArray titleData = queryObj.value(pageTitleField).toByteArray();
        QString		pageTitle;

        if (m_encrypterObj.HasPassword())
        {
            pageTitle = m_encrypterObj.DecryptString(titleData);
        }
        else
        {
            pageTitle = QString(titleData);
        }

        SHARED_CPAGEDATA	newPageDataObj = SHARED_CPAGEDATA(new (std::nothrow) CPageData);

        if (! newPageDataObj.isNull())
        {
            newPageDataObj.data()->m_pageId = pageId;
            newPageDataObj.data()->m_parentId = queryObj.value(parentIdField).toUInt();
            newPageDataObj.data()->m_modifiedDateTime = QDateTime::fromTime_t(queryObj.value(lastModifiedField).toInt());
            newPageDataObj.data()->m_title = pageTitle;
            newPageDataObj.data()->m_pageType = (PAGE_TYPE) queryObj.value(pageTypeField).toInt();

            pageHash.insert(pageId, newPageDataObj);
        }
    }

    return true;
}

//--------------------------------------------------------------------------
bool CDatabase::GetTagList( PAGE_ID_TAG_CACHE& pageIdTagHash )
{
	QSqlQuery	queryObj(m_db);

	queryObj.prepare("select pageid, tags from pages");

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to retrieve page IDs and tags: " << sqlErr.text();
		return false;
	}

	pageIdTagHash.clear();

	while (queryObj.next())
	{
		int	pageIdField = queryObj.record().indexOf("pageid");
		int tagsField = queryObj.record().indexOf("tags");

		int	pageId = queryObj.value(pageIdField).toUInt();
		QByteArray	tagData = queryObj.value(tagsField).toByteArray();
		QString		tagsStr;

		if (m_encrypterObj.HasPassword())
		{
			tagsStr = m_encrypterObj.DecryptString(tagData);
		}
		else
		{
			tagsStr = QString(tagData);
		}

		TAG_LIST	tagList;

		tagList = CPageData::TagStringToStringList(tagsStr);

		foreach(QString tagStrItem, tagList)
		{
			pageIdTagHash.insert(pageId, tagStrItem);
		}
	}

	return true;
}

//--------------------------------------------------------------------------
bool CDatabase::GetFavoritePages( ENTITY_LIST& pageIdList )
{
	QSqlQuery	queryObj(m_db);

	queryObj.prepare("select pageid from pages where isfavorite=1");

	queryObj.exec();

	// Check for errors
	QSqlError	sqlErr;

	sqlErr = m_db.lastError();
	if (sqlErr.type() != QSqlError::NoError)
	{
		// Should probably log this error
		qDebug() << "Error when attempting to retrieve favorite pages: " << sqlErr.text();
		return false;
	}

	pageIdList.clear();

	while (queryObj.next())
	{
		int	pageIdField = queryObj.record().indexOf("pageid");

		int	pageId = queryObj.value(pageIdField).toUInt();

		pageIdList << pageId;
	}

	return true;
}

//--------------------------------------------------------------------------
bool CDatabase::SetPageFavoriteStatus( ENTITY_ID pageId, bool bIsFavorite )
{
	QSqlQuery	queryObj(m_db);
	bool		bRetVal = false;

	if (PageExists(pageId))
	{
		queryObj.prepare("update pages set isfavorite=? where pageid=?");

		queryObj.addBindValue(QVariant(bIsFavorite ? 1 : 0));
		queryObj.addBindValue(QVariant(pageId));

		queryObj.exec();

		// Check for errors
		QSqlError	sqlErr;

		sqlErr = m_db.lastError();
		if (sqlErr.type() != QSqlError::NoError)
		{
			// Should probably log this error
			qDebug() << "Error when attempting to update a page's favorite status: " << sqlErr.text();
			bRetVal = false;
		}
		else
		{
			bRetVal = true;
		}
	}

	return bRetVal;
}


/**
 * Additional Data Table Accessor Functions
 */

bool CDatabase::AddImage(QString imageName, QPixmap pixmap, ENTITY_ID parentPageId)
{
    bool        bReturn = true;
    QSqlQuery	queryObj(m_db);
    QByteArray	imageData;

    // Convert pixmap to a QByteArray, for storage
    QBuffer     buffer(&imageData);

    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    buffer.close();

    QString     commandStr = QString("insert into %1 (itemid, type, contents, parentid)").arg(kAdditionalDataTable);
                commandStr += " values (?, ?, ?, ?);";

    queryObj.prepare(commandStr);

    queryObj.addBindValue(QVariant(imageName));
    queryObj.addBindValue(QVariant(kImageData));
    queryObj.addBindValue(QVariant(imageData));
    queryObj.addBindValue(QVariant(parentPageId));

    queryObj.exec();

    // Check for errors
    QSqlError	sqlErr;

    sqlErr = m_db.lastError();
    if (sqlErr.type() != QSqlError::NoError)
    {
        // TODO: Log database errors
        qDebug() << "Error when attempting to save an image: " << sqlErr.text();
        bReturn = false;
    }

    return bReturn;
}

//--------------------------------------------------------------------------
bool CDatabase::GetImage(QString imageName, QPixmap &pixmap)
{
    bool        bReturn = false;
    QSqlQuery	queryObj(m_db);
    QByteArray	imageData;

    QString     commandStr = QString("select contents from %1 where itemid=?").arg(kAdditionalDataTable);

    queryObj.prepare(commandStr);
    queryObj.addBindValue(QVariant(imageName));

    queryObj.exec();

    // Check for errors
    QSqlError	sqlErr;

    sqlErr = m_db.lastError();
    if (sqlErr.type() != QSqlError::NoError)
    {
        // TODO: Log database errors
        qDebug() << "Error when attempting to retrieve an image: " << sqlErr.text();
        bReturn = false;
    }
    else
    {
        if (queryObj.next())
        {
            int	contentsField = queryObj.record().indexOf("contents");

            QByteArray contentsData = queryObj.value(contentsField).toByteArray();
            if (pixmap.loadFromData(contentsData))
            {
                bReturn = true;
            }
        }
    }

    return bReturn;
}

//--------------------------------------------------------------------------
bool CDatabase::GetImageNamesForPage(ENTITY_ID pageId, QStringList& nameList)
{
    bool        bReturn = false;
    QSqlQuery	queryObj(m_db);
    QByteArray	imageData;

    QString     commandStr = QString("select itemid from %1 where parentid=?").arg(kAdditionalDataTable);

    queryObj.prepare(commandStr);
    queryObj.addBindValue(QVariant(pageId));

    queryObj.exec();

    // Check for errors
    QSqlError	sqlErr;

    sqlErr = m_db.lastError();
    if (sqlErr.type() != QSqlError::NoError)
    {
        // Should probably log this error
        qDebug() << "Error when attempting to retrieve image names: " << sqlErr.text();
        bReturn = false;
    }
    else
    {
        nameList.clear();

        while (queryObj.next())
        {
            int	nameField = queryObj.record().indexOf("itemid");

            QString imageName = queryObj.value(nameField).toString();

            nameList << imageName;
        }

        bReturn = true;
    }

    return bReturn;
}

//--------------------------------------------------------------------------
bool CDatabase::DeleteAllImagesForPage(ENTITY_ID pageId)
{
    bool        bReturn = false;
    QSqlQuery	queryObj(m_db);

    QString     commandStr = QString("delete from %1 where parentid=?").arg(kAdditionalDataTable);

    queryObj.prepare(commandStr);
    queryObj.addBindValue(QVariant(pageId));

    queryObj.exec();

    // Check for errors
    QSqlError	sqlErr;

    sqlErr = m_db.lastError();
    if (sqlErr.type() != QSqlError::NoError)
    {
        // Should probably log this error
        qDebug() << "Error when attempting to delete images: " << sqlErr.text();
        bReturn = false;
    }

    return bReturn;
}
