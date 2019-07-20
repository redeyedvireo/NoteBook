#include "ImportDatabase.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

//--------------------------------------------------------------------------
CImportDatabase::CImportDatabase(QObject *parent)
	: QObject(parent)
{

}

//--------------------------------------------------------------------------
CImportDatabase::~CImportDatabase()
{

}

//--------------------------------------------------------------------------
bool CImportDatabase::OpenDatabase(const QString& databasePathname)
{
	if (m_db.isOpen())
		return true;

	// Don't need to determine if database file exists, as that
	// will be done by the calling function.
	bool		bReturn = false;

	m_db = QSqlDatabase::addDatabase("QSQLITE", "ImportDatabase");
	m_db.setDatabaseName(databasePathname);

	if (m_db.open())
	{
		bReturn = true;
	}
	else
	{
		QSqlError	dbErr = m_db.lastError();
		QString		errMessage = QString("Could not open %1\n"
			"Driver text: %2\nDatabase text: %3")
			.arg(databasePathname)
			.arg(dbErr.driverText())
			.arg(dbErr.databaseText());

		QMessageBox::critical(NULL, "Import NoteBook - Database Error", errMessage);
		bReturn = false;
	}

	return bReturn;
}

//--------------------------------------------------------------------------
void CImportDatabase::CloseDatabase()
{
	m_db.close();
	m_encrypterObj.ClearPassword();
}

//--------------------------------------------------------------------------
bool CImportDatabase::IsPasswordProtected()
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
bool CImportDatabase::PasswordMatch(const QString& pw)
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
bool CImportDatabase::GetPageList(PAGE_HASH& pageHash)
{
	QSqlQuery	queryObj(m_db);

	queryObj.prepare("select pageid, parentid, pagetitle, lastmodified, pagetype  from pages");

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
bool CImportDatabase::GetPage(ENTITY_ID pageId, CPageData& pageData)
{
	QSqlQuery	queryObj(m_db);

	queryObj.prepare("select pagetype, parentid, contents, pagetitle, tags, created, lastmodified, nummodifications, additionalitems from pages where pageid=?");

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

		int pageTypeInt = queryObj.value(pageTypeField).toInt();
		ENTITY_ID parentId = queryObj.value(parentIdField).toUInt();
		QByteArray contentsData = queryObj.value(contentsField).toByteArray();
		QByteArray titleData = queryObj.value(pageTitleField).toByteArray();
		QByteArray tagData = queryObj.value(tagsField).toByteArray();
		int lastModifiedTime_t = queryObj.value(lastModifiedDateField).toInt();
		int createdTime_t = queryObj.value(createdField).toInt();
		QString additionalItemsStr = queryObj.value(additionalItemsField).toString();

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
		pageData.m_bIsFavorite = false;
	}
	else
		return false;		// No entry found

	return true;
}
