#pragma once

#include "PageData.h"
#include <QHash>


typedef QHash<ENTITY_ID, QString>	PAGE_CACHE;


class CPageCache
{
public:
	static CPageCache* Instance();
	~CPageCache(void);

	void AddPage(ENTITY_ID pageId, const QString& pageTitle);
	void AddPages(const PAGE_HASH& pageHash);

	QString PageTitle(ENTITY_ID pageId);
	ENTITY_ID PageId(const QString& pageTitle);

	/*
	 *	Changes the title of a page in the cache.
	 */
	void ChangeTitle(ENTITY_ID pageId, const QString& newTitle);

	/*
	 *	Deletes a page from the cache.
	 */
	void DeletePage(ENTITY_ID pageId);

	/*
	 *	Determines if a page exists in the cache.
	 */
	bool Contains(ENTITY_ID pageId);

	/*
	 *	Overload that determines if a page title exists in the cache.
	 */
	bool Contains(const QString& pageTitle);

private:
	CPageCache(void);

private:
	static CPageCache*	m_pInstance;

	PAGE_CACHE			m_pageCache;
};
