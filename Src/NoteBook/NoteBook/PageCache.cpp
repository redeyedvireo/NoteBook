#include "stdafx.h"
#include "PageCache.h"
#include <QHashIterator>

// CPageCache is a singleton
CPageCache* CPageCache::m_pInstance = NULL;


//--------------------------------------------------------------------------
CPageCache::CPageCache(void)
{
}

//--------------------------------------------------------------------------
CPageCache::~CPageCache(void)
{
}

//--------------------------------------------------------------------------
CPageCache* CPageCache::Instance()
{
	if (! m_pInstance)
	{
		m_pInstance = new (std::nothrow) CPageCache();
	}

	return m_pInstance;
}

//--------------------------------------------------------------------------
void CPageCache::AddPage( ENTITY_ID pageId, const QString& pageTitle )
{
	if (! m_pageCache.contains(pageId))
	{
		// Not found
		m_pageCache.insert(pageId, pageTitle);
	}
}

//--------------------------------------------------------------------------
void CPageCache::AddPages( const PAGE_HASH& pageHash )
{
	QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	it(pageHash);

	while (it.hasNext())
	{
		it.next();

		SHARED_CPAGEDATA	pageData = it.value();
		m_pageCache.insert(it.key(), pageData.data()->m_title);
	}
}

//--------------------------------------------------------------------------
QString CPageCache::PageTitle( ENTITY_ID pageId )
{
	QString		strResult;

	if (m_pageCache.contains(pageId))
	{
		strResult = m_pageCache.value(pageId);
	}

	return strResult;
}

//--------------------------------------------------------------------------
ENTITY_ID CPageCache::PageId( const QString& pageTitle )
{
	QHashIterator<ENTITY_ID, QString>	it(m_pageCache);

	while (it.hasNext())
	{
		it.next();

		if (it.value() == pageTitle)
		{
			return it.key();
		}
	}

	// Not found
	return kInvalidPageId;
}

//--------------------------------------------------------------------------
void CPageCache::ChangeTitle( ENTITY_ID pageId, const QString& newTitle )
{
	if (m_pageCache.contains(pageId))
	{
		m_pageCache[pageId] = newTitle;
	}
}

//--------------------------------------------------------------------------
void CPageCache::DeletePage( ENTITY_ID pageId )
{
	m_pageCache.remove(pageId);
}

//--------------------------------------------------------------------------
bool CPageCache::Contains( ENTITY_ID pageId )
{
	return m_pageCache.contains(pageId);
}

//--------------------------------------------------------------------------
bool CPageCache::Contains( const QString& pageTitle )
{
	QList<QString>	allValues = m_pageCache.values();

	return allValues.contains(pageTitle);
}
