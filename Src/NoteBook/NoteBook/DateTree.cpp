#include "stdafx.h"
#include "DateTree.h"
#include "notebook.h"
#include <QDate>
#include <QDebug>


//--------------------------------------------------------------------------
CDateTree::CDateTree(QWidget *parent)
	: QTreeWidget(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	InitMenus();
}

//--------------------------------------------------------------------------
CDateTree::~CDateTree()
{

}

//--------------------------------------------------------------------------
void CDateTree::InitMenus()
{
	m_contextMenu.addAction("Expand All", this, SLOT(expandAll()));
	m_contextMenu.addAction("Collapse All", this, SLOT(collapseAll()));
}

//--------------------------------------------------------------------------
void CDateTree::Initialize( CNoteBook *parent )
{
	SetConnections(parent);
}

//--------------------------------------------------------------------------
void CDateTree::SetConnections(CNoteBook *parent)
{
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));

	connect(parent, SIGNAL(MW_NewBlankPageCreated(CPageData)), this, SLOT(OnNewBlankPageCreated(CPageData)));
	connect(parent, SIGNAL(MW_PageSaved(CPageData)), this, SLOT(OnPageSaved(CPageData)));
	connect(parent, SIGNAL(MW_PageTitleChanged(CPageData)), this, SLOT(OnPageTitleChanged(CPageData)));
	connect(parent, SIGNAL(MW_PageDeleted(ENTITY_ID)), this, SLOT(OnPageDeleted(ENTITY_ID)));
	connect(parent, SIGNAL(MW_PageImported(CPageData)), this, SLOT(OnPageImported(CPageData)));
	connect(parent, SIGNAL(MW_PageUpdatedByImport(CPageData)), this, SLOT(OnPageUpdatedByImport(CPageData)));
	connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(OnItemClicked(QTreeWidgetItem*, int)));
	connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(OnItemChanged(QTreeWidgetItem*, int)));
}

//--------------------------------------------------------------------------
ENTITY_ID CDateTree::GetItemPageId(QTreeWidgetItem* pItem)
{
	bool			bOk;
	QVariant		varData = pItem->data(0, Qt::UserRole);
	ENTITY_ID		pageId;

	pageId = varData.toUInt(&bOk);
	Q_ASSERT(bOk);

	return pageId;
}

//--------------------------------------------------------------------------
void CDateTree::AddItem( const CPageData& pageData )
{
	// Find the date under which this page should be created
	CDateWidgetItem* pDateItem = FindDate(pageData.m_modifiedDateTime.date());

	if (pDateItem == NULL)
	{
		pDateItem = AddDate(pageData.m_modifiedDateTime.date());
	}

	// Add to the list
	QTreeWidgetItem*	pNewItem = new (std::nothrow) QTreeWidgetItem();

	if (pNewItem != NULL)
	{
		pNewItem->setText(0, pageData.m_title);
		pNewItem->setData(0, Qt::UserRole, QVariant(pageData.m_pageId));
		pDateItem->addChild(pNewItem);
	}
}

//--------------------------------------------------------------------------
void CDateTree::AddItems( const PAGE_HASH& pageHash )
{
	clear();

	QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	it(pageHash);

	setSortingEnabled(false);		// Turn sorting off while inserting many items

	while (it.hasNext())
	{
		it.next();

		SHARED_CPAGEDATA	pageData = it.value();
		AddItem(*pageData);
	}

//	setSortingEnabled(true);

	SortDateItems();
	SortPageTitles();
}

//--------------------------------------------------------------------------
QTreeWidgetItem* CDateTree::FindPageItem(ENTITY_ID pageId)
{
	if (pageId == kInvalidPageId)
		return NULL;		// No item will have this page ID
	else
	{
		// Iterate over the children of the date items.  The date items are the 
		// first level of children in the tree.
		int		tliCount = topLevelItemCount();
		for (int dateItemNum = 0; dateItemNum < tliCount; dateItemNum++)
		{
			QTreeWidgetItem*	pDateItem = topLevelItem(dateItemNum);

			for (int i = 0; i < pDateItem->childCount(); i++)
			{
				QTreeWidgetItem*	pTreeItem = pDateItem->child(i);

				if (pTreeItem && GetItemPageId(pTreeItem) == pageId)
				{
					return pTreeItem;
				}
			}
		}

		return NULL;
	}
}

//--------------------------------------------------------------------------
CDateWidgetItem* CDateTree::PageItemParent(QTreeWidgetItem* pPageItem)
{
	QTreeWidgetItem*	pParent = pPageItem->parent();
	return dynamic_cast<CDateWidgetItem*>(pParent);
}

//--------------------------------------------------------------------------
CDateWidgetItem* CDateTree::FindDate(const QDate& inDate)
{
	int		tliCount = topLevelItemCount();
	for (int i = 0; i < tliCount; i++)
	{
		QTreeWidgetItem*	pItem = topLevelItem(i);
		CDateWidgetItem*	pDateItem = dynamic_cast<CDateWidgetItem*>(pItem);

		if (pDateItem != NULL)
		{
			if (*pDateItem == inDate)
				return pDateItem;
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------
CDateWidgetItem* CDateTree::AddDate(const QDate& inDate)
{
	CDateWidgetItem*	pDateItem = new (std::nothrow) CDateWidgetItem(0, inDate);

	if (pDateItem != NULL)
	{
		addTopLevelItem(pDateItem);
	}

	return pDateItem;
}

//--------------------------------------------------------------------------
void CDateTree::SortTitlesWithinDate(QTreeWidgetItem* pItem)
{
	pItem->sortChildren(0, Qt::AscendingOrder);
}

//--------------------------------------------------------------------------
void CDateTree::SortPageTitles()
{
	// Since the dates are sorted in descending order, it is necessary
	// to resort the page titles in ascending order.
	int		tliCount = topLevelItemCount();
	for (int i = 0; i < tliCount; i++)
	{
		QTreeWidgetItem*	pItem = topLevelItem(i);
		SortTitlesWithinDate(pItem);
	}
}

//--------------------------------------------------------------------------
void CDateTree::SortDateItems()
{
	// This will end up sorting all of column 0, which would include
	// the page titles.  The page titles will have to be sorted individually
	// after this function is called.

	QTreeWidgetItem		*pRootItem = invisibleRootItem();
	pRootItem->sortChildren(0, Qt::DescendingOrder);
}


/************************************************************************/
/* Slots                                                                */
/************************************************************************/

void CDateTree::OnContextMenu( const QPoint& pos )
{
	// Since the context menu contains only "Expand All" and "Collapse All",
	// the menu can be invoked by clicking anywhere in the control.  In the
	// future, if items are added that apply only to a specific date, then
	// this function will need to check that an item is being clicked on,
	// as is done for the Page Tree.
	m_contextMenu.popup(mapToGlobal(pos));
}

//--------------------------------------------------------------------------
void CDateTree::OnItemChanged( QTreeWidgetItem* pItem, int column )
{
	// Since the dates are sorted in descending order, it is necessary
	// to resort the page titles in ascending order, whenever an item
	// is changed.  Whenever the tree is touched, Qt will resort the tree,
	// thus, it is necessary to resort the titles within each date.
	SortPageTitles();
}

//--------------------------------------------------------------------------
void CDateTree::OnItemClicked( QTreeWidgetItem* item, int column )
{
	if (item->type() == QTreeWidgetItem::Type)
	{
		ENTITY_ID		pageId = GetItemPageId(item);

		emit PageSelected(pageId);
	}
}

//--------------------------------------------------------------------------
void CDateTree::OnNewBlankPageCreated(CPageData pageData)
{
	AddItem(pageData);

	SortPageTitles();
}

//--------------------------------------------------------------------------
void CDateTree::OnPageSaved(CPageData pageData)
{
	// This slot is only called when an existing page is saved.
	// Determine if the page needs to be moved to a different date parent.
	QTreeWidgetItem*	pItem = FindPageItem(pageData.m_pageId);

	if (pItem != NULL)
	{
		CDateWidgetItem*	pDateItem = PageItemParent(pItem);
		int					childIndex = pDateItem->indexOfChild(pItem);

		if (pDateItem != NULL)
		{
			if (*pDateItem != pageData.m_modifiedDateTime.date())
			{
				// The page now has a different modification date.  It needs to be moved.
				// Find the date under which this page should be created
				CDateWidgetItem* pNewDateItem = FindDate(pageData.m_modifiedDateTime.date());

				if (pNewDateItem == NULL)
				{
					// Date does not exist, so add it
					pNewDateItem = AddDate(pageData.m_modifiedDateTime.date());

					// Must re-sort the date items, since a new date was added
					SortDateItems();

					// Sorting the date items will require all page titles to be resorted
					SortPageTitles();
				}

				// For Debug, verify that the original item is still at the same childindex
				int		childIndexVerify = pDateItem->indexOfChild(pItem);
				Q_ASSERT(childIndex == childIndexVerify);
				// End Debug

				// Remove page item from its current date, and add to the new date
				QTreeWidgetItem*	pRemovedItem = pDateItem->takeChild(childIndex);
				qDebug() << "Taking child: " << pRemovedItem->text(0);
				pNewDateItem->addChild(pRemovedItem);

				// If the old date has no more children, remove it
				if (pDateItem->childCount() == 0)
				{
					int  index = indexOfTopLevelItem(pDateItem);
					takeTopLevelItem(index);
				}

				SortTitlesWithinDate(pNewDateItem);
			}
		}
	}
}

//--------------------------------------------------------------------------
void CDateTree::OnPageTitleChanged( CPageData pageData )
{
	QTreeWidgetItem*	pItem = FindPageItem(pageData.m_pageId);

	if (pItem != NULL)
	{
		CDateWidgetItem*	pItemDate = PageItemParent(pItem);
		pItem->setText(0, pageData.m_title);
		
		SortTitlesWithinDate(pItemDate);	// Maintain proper sort order in the date

		// Changing the title constitutes a "change"
		OnPageSaved(pageData);
	}
}

//--------------------------------------------------------------------------
void CDateTree::OnPageDeleted( ENTITY_ID pageId )
{
	QTreeWidgetItem*	pItem = FindPageItem(pageId);

	if (pItem != NULL)
	{
		CDateWidgetItem*	pItemDate = PageItemParent(pItem);
		pItemDate->removeChild(pItem);

		// If the date widget item has no more children, remove the date widget item
		if (pItemDate->childCount() == 0)
		{
			int index = indexOfTopLevelItem(pItemDate);
			takeTopLevelItem(index);
			delete pItemDate;
		}
	}
}

//--------------------------------------------------------------------------
void CDateTree::OnPageImported( CPageData pageData )
{
	AddItem(pageData);

	SortPageTitles();
}

//--------------------------------------------------------------------------
void CDateTree::OnPageUpdatedByImport( CPageData pageData )
{
	OnPageSaved(pageData);
}
