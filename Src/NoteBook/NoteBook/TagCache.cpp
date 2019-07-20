#include "stdafx.h"
#include "TagCache.h"
#include <QHashIterator>

// CTagCache is a singleton
CTagCache* CTagCache::m_pInstance = NULL;


//--------------------------------------------------------------------------
CTagCache::CTagCache(void)
{
}

//--------------------------------------------------------------------------
CTagCache::~CTagCache(void)
{
}

//--------------------------------------------------------------------------
CTagCache* CTagCache::Instance()
{
	if (! m_pInstance)
	{
		m_pInstance = new (std::nothrow) CTagCache();
	}

	return m_pInstance;
}

//--------------------------------------------------------------------------
void CTagCache::AddTagMap( const QString& tagStr, ENTITY_ID pageId )
{
	if (! m_tagCache.contains(tagStr, pageId))
	{
		// Not found

		m_tagCache.insert(tagStr, pageId);
	}
}

//--------------------------------------------------------------------------
void CTagCache::AddTags( const PAGE_ID_TAG_CACHE& pageIdTagCache )
{
	QHashIterator<ENTITY_ID, QString>	it(pageIdTagCache);

	while (it.hasNext())
	{
		it.next();

		// The key for m_tagCache is the tag string, hence the ordering
		// of the parameters below.
		m_tagCache.insert(it.value(), it.key());
	}
}

//--------------------------------------------------------------------------
ENTITY_LIST CTagCache::PagesUsingTag( const QString& tagStr )
{
	ENTITY_LIST		pagesList = m_tagCache.values(tagStr);

	return pagesList;
}

//--------------------------------------------------------------------------
void CTagCache::RemovePage( const QString& tagStr, ENTITY_ID pageId )
{
	m_tagCache.remove(tagStr, pageId);
}

//--------------------------------------------------------------------------
bool CTagCache::Contains( const QString& tagStr )
{
	return m_tagCache.contains(tagStr);
}
