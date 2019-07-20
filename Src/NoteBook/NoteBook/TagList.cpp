#include "stdafx.h"
#include "TagList.h"
#include "TagCache.h"
#include "PageCache.h"
#include "notebook.h"
#include <QAction>
#include <QSet>


//--------------------------------------------------------------------------
CTagList::CTagList(QWidget *parent)
	: QListWidget(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
}

//--------------------------------------------------------------------------
CTagList::~CTagList()
{

}

//--------------------------------------------------------------------------
void CTagList::Initialize( CNoteBook *parent )
{
	SetConnections(parent);
}

//--------------------------------------------------------------------------
void CTagList::SetConnections( CNoteBook *parent )
{
	connect(parent, SIGNAL(MW_PageSaved(CPageData)), this, SLOT(OnPageSaved(CPageData)));
	connect(parent, SIGNAL(MW_PageImported(CPageData)), this, SLOT(OnPageImported(CPageData)));
	connect(parent, SIGNAL(MW_PageUpdatedByImport(CPageData)), this, SLOT(OnPageUpdatedByImport(CPageData)));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));
	connect(&m_signalMapper, SIGNAL(mapped(int)), this, SLOT(OnPageSelected(int)));
}

//--------------------------------------------------------------------------
void CTagList::AddItem(const QString& tagStr)
{
	QListWidgetItem*	pNewItem = new (std::nothrow) QListWidgetItem(tagStr);

	if (pNewItem != NULL)
	{
		addItem(pNewItem);
	}
}

//--------------------------------------------------------------------------
void CTagList::AddItems( const PAGE_ID_TAG_CACHE& pageIdTagCache )
{
	QList<QString>	keyList;

	keyList = pageIdTagCache.values();

	QStringList		keyStrList(keyList);

	keyStrList.removeDuplicates();

	foreach (QString tagStr, keyStrList)
	{
		AddItem(tagStr);
	}
}

//--------------------------------------------------------------------------
QListWidgetItem* CTagList::FindItem(const QString& itemStr)
{
	QList<QListWidgetItem*>	foundItems = findItems(itemStr, Qt::MatchFixedString);

	if (foundItems.size() == 0)
	{
		return NULL;
	}

	// There really should only be 1 occurrence of this string
	return foundItems.at(0);
}

//--------------------------------------------------------------------------
void CTagList::RemoveItem(const QString& itemStr)
{
	QListWidgetItem*	pItem = FindItem(itemStr);

	if (pItem != NULL)
	{
		int		itemRow = row(pItem);

		if (itemRow != -1)
		{
			takeItem(itemRow);
		}
	}
}


/************************************************************************/
/* Slots                                                                */
/************************************************************************/

void CTagList::OnPageSaved( CPageData pageData )
{
	// Get list of tags from pageData.
	TAG_LIST	tagList = pageData.GetTagList();

	foreach (QString tagStr, tagList)
	{
		CTagCache::Instance()->AddTagMap(tagStr, pageData.m_pageId);

		QListWidgetItem*	pFoundItem = FindItem(tagStr);

		if (pFoundItem == NULL)
		{
			// This tag does not exist in the list - add it
			AddItem(tagStr);
		}
	}

	// Remove tags that the user has removed.
	QSet<QString>	prevTags;
	QSet<QString>	currentTags;

	prevTags = QSet<QString>::fromList(pageData.GetPreviousTagList());
	currentTags = QSet<QString>::fromList(pageData.GetTagList());

	// Remove the new tags.  Any tags remaining were deleted by the user.
	prevTags -= currentTags;

	foreach (QString tagStr, prevTags)
	{
		// Remove from tag cache
		CTagCache::Instance()->RemovePage(tagStr, pageData.m_pageId);

		// Now remove any of these tags from the control, if it is not used anymore
		if (! CTagCache::Instance()->Contains(tagStr))
		{
			RemoveItem(tagStr);
		}
	}
}

//--------------------------------------------------------------------------
void CTagList::OnContextMenu( const QPoint& pos )
{
	// Create a context menu with items for each page that contains the selected tag.
	m_contextMenu.clear();

	QListWidgetItem*	pItem = itemAt(pos);

	if (pItem != NULL)
	{
		QString		tagStr = pItem->text();

		ENTITY_LIST		pageList = CTagCache::Instance()->PagesUsingTag(tagStr);

		// Use a QMap in order to alphabetize the list
		QMap<QString, int>	sortedList;

		foreach (ENTITY_ID pageId, pageList)
		{
			sortedList.insert(CPageCache::Instance()->PageTitle(pageId), pageId);
		}

		QMapIterator<QString, int>	it(sortedList);
		while (it.hasNext())
		{
			it.next();
			QAction*	pAction;

			pAction = m_contextMenu.addAction(QString("%1").arg(it.key()));

			m_signalMapper.setMapping(pAction, it.value());
			connect(pAction, SIGNAL(triggered()), &m_signalMapper, SLOT(map()));
		}

		if (pageList.size() > 0)
		{
			m_contextMenu.popup(mapToGlobal(pos));
		}
	}
}

//--------------------------------------------------------------------------
void CTagList::OnPageSelected( int pageId )
{
	emit TL_PageSelected(pageId);
}

//--------------------------------------------------------------------------
void CTagList::OnPageImported( CPageData pageData )
{
	OnPageSaved(pageData);
}

//--------------------------------------------------------------------------
void CTagList::OnPageUpdatedByImport( CPageData pageData )
{
	OnPageSaved(pageData);
}
