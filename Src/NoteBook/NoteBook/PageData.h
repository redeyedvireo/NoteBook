#pragma once

#ifndef PageData_h__
#define PageData_h__

#include <QStringList>
#include <QDateTime>
#include <QList>
#include <QMultiHash>
#include <QSharedPointer>

#define kInvalidPageId		0


typedef enum {
	kPageTypeUserText = 0,				// User-entered text
	kPageFolder,						// Folder page (simply a placeholder that has children)
    kPageTypeToDoList,                  // To Do list
	kPageTypeHtml,						// Specific HTML code
    kPageTypeJavascript                 // Javascript
} PAGE_TYPE;

typedef enum {
    kAdditionalDataTypeImage = 1,		// Image data
	kAdditionalDataTypeAudio,			// Audio data
	kAdditionalDataTypeVideo			// Video data
} ADDITIONAL_DATA_TYPE;

typedef enum {
	kAbort = 0,
	kCreateNewEntry,
	kAppendEntry,
	kOverwriteEntry,
	kSkipEntry
} DecisionType;

typedef unsigned int						ENTITY_ID;
typedef QList<ENTITY_ID>					ENTITY_LIST;
typedef QStringList							TAG_LIST;


class CPageData
{
public:
	CPageData(void);
    ~CPageData(void) {}

	/*
	 *	Sets the additional items field from a comma-separated list.
	 */
	void SetAdditionalItemsFromCommaSeparatedList(const QString& commaList);

	/*
	 *	Retrieves the additional items as a comma-separated list.
	 */
	QString AdditionalItemsAsCommasSeparatedList();

	/*
	 *	Appends additional items to the current list of additional items.
	 *	@param A string consisting of comma-separated items.
	 */
	void AppendAdditionalItems(const QString& commaList);

	/*
	 *	Retrieves the tag list.
	 */
	TAG_LIST GetTagList();

	/*
	 *	Appends tags to the tag list.
	 */
	void AppendTags(const QString& tagString);

	/*
	 *	Retrieves the previous tag list
	 */
	TAG_LIST GetPreviousTagList();

	/*
	 *	Returns the created time as a time_t.
	 */
	uint CreatedTimeAsTimeT()	{ return m_createdDateTime.toTime_t(); }

	/*
	 *	Returns the created time as a string.
	 */
	QString CreatedTimeAsString()	{ return m_createdDateTime.toString(Qt::SystemLocaleLongDate); }

	/*
	 *	Returns the modified time as a string.
	 */
	QString ModifiedTimeAsString()	{ return m_modifiedDateTime.toString(Qt::SystemLocaleLongDate); }

	/*
	 *	Returns the modification time as a time_t.
	 */
	uint ModifiedTimeAsTimeT()	{ return m_modifiedDateTime.toTime_t(); }

	bool IsFavorite()		{ return m_bIsFavorite; }

	/*
	 *	Converts an ENTITY_LIST to a comma-separated list. [static]
	 */
	static QString EntityListToCommaSeparatedList(ENTITY_LIST items);

	/*
	 *	Converts a comma-separated list to an ENTITY_LIST.  [static]
	 */
	static void CommaSeparatedListToEntityList(const QString& itemStr, ENTITY_LIST& entityList);

	/*
	 *	Converts a tag string to a TAG_LIST.  The tagString can be either
	 *	comma-separated or space-separated.
	 */
	static TAG_LIST TagStringToStringList(const QString& tagString);

	/*
	 *	Converts a TAG_LIST to a string of comma-separated tags.
	 */
	static QString StringListToTagString(const TAG_LIST& tagList);

	// Public data
	ENTITY_ID			m_pageId;
	ENTITY_ID			m_parentId;
	QString				m_title;
	PAGE_TYPE			m_pageType;
	QString				m_contentString;
	QString				m_tags;
	QString				m_previousTags;			// Used when updating a page
	QDateTime			m_modifiedDateTime;		// When last modified
	QDateTime			m_createdDateTime;
	int					m_numModifications;
	ENTITY_LIST			m_additionalDataItems;
	bool				m_bIsFavorite;			// True if the page is a "favorite" page
};

typedef QSharedPointer<CPageData>			SHARED_CPAGEDATA;	// CPageData pointer wrapped in a QSharedPointer
typedef QHash<ENTITY_ID, SHARED_CPAGEDATA>	PAGE_HASH;			// Used when loading a new Notebook
typedef QMultiHash<ENTITY_ID, QString>		PAGE_ID_TAG_CACHE;	// Used when loading a new Notebook
typedef QList<CPageData>					PAGE_LIST;			// Used mainly when importing


bool dateLessThan(const CPageData& s1, const CPageData& s2);

#endif // PageData_h__
