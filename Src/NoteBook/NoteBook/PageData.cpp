#include "stdafx.h"
#include "PageData.h"
#include <QSet>


bool dateLessThan( const CPageData& s1, const CPageData& s2 )
{
	return s1.m_modifiedDateTime < s2.m_modifiedDateTime;
}

//--------------------------------------------------------------------------
CPageData::CPageData(void)
{
	m_pageId = kInvalidPageId;
	m_parentId = kInvalidPageId;
	m_pageType = kPageTypeUserText;
	m_numModifications = 0;
	m_bIsFavorite = false;

	m_createdDateTime = QDateTime::currentDateTime();
	m_modifiedDateTime = QDateTime::currentDateTime();
}

//--------------------------------------------------------------------------
QString CPageData::EntityListToCommaSeparatedList(ENTITY_LIST items)
{
	QString		itemStr;

	// Create additionalItemsStr
	foreach (int item, items)
	{
		itemStr.append(QString("%1,").arg(item));
	}

    if (itemStr.endsWith(","))
	{
		// Remove the trailing comma
		itemStr.chop(1);
	}

	return itemStr;
}

//--------------------------------------------------------------------------
void CPageData::CommaSeparatedListToEntityList( const QString& itemStr, ENTITY_LIST& entityList )
{
	QStringList		itemStrList = itemStr.split(",");

	entityList.clear();

	foreach (QString itemStr, itemStrList)
	{
		entityList << itemStr.toInt();
	}
}

//--------------------------------------------------------------------------
void CPageData::SetAdditionalItemsFromCommaSeparatedList( const QString& commaList )
{
	CPageData::CommaSeparatedListToEntityList(commaList, m_additionalDataItems);
}

//--------------------------------------------------------------------------
QString CPageData::AdditionalItemsAsCommasSeparatedList()
{
	return CPageData::EntityListToCommaSeparatedList(m_additionalDataItems);
}

//--------------------------------------------------------------------------
void CPageData::AppendAdditionalItems( const QString& commaList )
{
	ENTITY_LIST		newItems;
	CommaSeparatedListToEntityList(commaList, newItems);

	m_additionalDataItems << newItems;

	// Remove duplicates
	QSet<ENTITY_ID>		uniqueItems = QSet<ENTITY_ID>::fromList(newItems);

	m_additionalDataItems = QList<ENTITY_ID>::fromSet(uniqueItems);
}

//--------------------------------------------------------------------------
TAG_LIST CPageData::TagStringToStringList( const QString& tagString )
{
	TAG_LIST	tagList;

	// First check if list is comma-separated
	if (tagString.contains(","))
	{
		tagList = tagString.split(",", QString::SkipEmptyParts);
	}
	else
	{
		// No commas - assume it is space-separated
		tagList = tagString.split(" ", QString::SkipEmptyParts);
	}

	return tagList;
}

//--------------------------------------------------------------------------
QString CPageData::StringListToTagString( const TAG_LIST& tagList )
{
	return tagList.join(",");
}

//--------------------------------------------------------------------------
TAG_LIST CPageData::GetTagList()
{
	return CPageData::TagStringToStringList(m_tags);
}

//--------------------------------------------------------------------------
void CPageData::AppendTags( const QString& tagString )
{
	TAG_LIST	existingTags = GetTagList();
	TAG_LIST	newTags = CPageData::TagStringToStringList(tagString);

	existingTags << newTags;
	existingTags.removeDuplicates();

	m_tags = StringListToTagString(existingTags);
}

//--------------------------------------------------------------------------
TAG_LIST CPageData::GetPreviousTagList()
{
	return CPageData::TagStringToStringList(m_previousTags);
}
