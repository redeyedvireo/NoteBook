#pragma once

#include "PageData.h"
#include <QMultiHash>

/*
 *	The TAG_CACHE maps tag strings with a list of ENTITY_IDs corresponding
 *	to the IDs of pages that contain those tag strings.
 */
typedef QMultiHash<QString, ENTITY_ID>		TAG_CACHE;


class CTagCache
{
public:
	static CTagCache* Instance();
	~CTagCache(void);

private:
	CTagCache(void);

public:
	/*
	 *	Adds a tag to the cache
	 */
	void AddTagMap(const QString& tagStr, ENTITY_ID pageId);

	/*
	 *	Adds multiple items to the cache.  Note that this function
	 *	is generally called when a NoteBook file is opened, and the
	 *	cache is empty.  As such, this function does not check
	 *	if the items exist in the cache before inserting.
	 */
	void AddTags(const PAGE_ID_TAG_CACHE& pageIdTagCache);

	void RemovePage(const QString& tagStr, ENTITY_ID pageId);

	/*
	 *	Determines if a tag exists in the cache.
	 */
	bool Contains(const QString& tagStr);

	ENTITY_LIST PagesUsingTag(const QString& tagStr);

private:
	static CTagCache*	m_pInstance;

	TAG_CACHE			m_tagCache;
};
