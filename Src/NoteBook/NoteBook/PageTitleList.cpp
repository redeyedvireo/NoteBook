#include "stdafx.h"
#include "PageTitleList.h"
#include "notebook.h"


//--------------------------------------------------------------------------
CPageTitleList::CPageTitleList(QWidget *parent)
	: QListWidget(parent)
{
}

//--------------------------------------------------------------------------
CPageTitleList::~CPageTitleList()
{

}

//--------------------------------------------------------------------------
void CPageTitleList::Initialize( CNoteBook *parent )
{
	SetConnections(parent);
}

//--------------------------------------------------------------------------
void CPageTitleList::SetConnections(CNoteBook *parent)
{
	connect(parent, SIGNAL(MW_NewBlankPageCreated(CPageData)), this, SLOT(OnNewBlankPageCreated(CPageData)));
	connect(parent, SIGNAL(MW_PageTitleChanged(CPageData)), this, SLOT(OnPageTitleChanged(CPageData)));
	connect(parent, SIGNAL(MW_PageDeleted(ENTITY_ID)), this, SLOT(OnPageDeleted(ENTITY_ID)));
	connect(parent, SIGNAL(MW_PageImported(CPageData)), this, SLOT(OnPageImported(CPageData)));
	connect(parent, SIGNAL(MW_PageUpdatedByImport(CPageData)), this, SLOT(OnPageUpdatedByImport(CPageData)));
	connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(OnItemClicked(QListWidgetItem*)));
}

//--------------------------------------------------------------------------
QListWidgetItem* CPageTitleList::FindItem( ENTITY_ID pageId )
{
	for (int i = 0; i < count(); i++)
	{
		QListWidgetItem*	pItem = item(i);

		if (pItem->data(Qt::UserRole).toUInt() == pageId)
		{
			return pItem;
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------
void CPageTitleList::AddItem( ENTITY_ID pageId, const QString& pageTitle )
{
	// Add to the list
	QListWidgetItem*	pNewItem = new (std::nothrow) QListWidgetItem(pageTitle);

	if (pNewItem != NULL)
	{
		pNewItem->setData(Qt::UserRole, QVariant(pageId));
		addItem(pNewItem);
	}
}


//--------------------------------------------------------------------------
void CPageTitleList::AddItems( const PAGE_HASH& pageHash )
{
	clear();

	QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	it(pageHash);

	while (it.hasNext())
	{
		it.next();

		SHARED_CPAGEDATA	pageData = it.value();
		AddItem(it.key(), pageData.data()->m_title);
	}
}


/************************************************************************/
/* Slots                                                                */
/************************************************************************/

void CPageTitleList::OnNewBlankPageCreated( CPageData pageData )
{
	AddItem(pageData.m_pageId, pageData.m_title);
}

//--------------------------------------------------------------------------
void CPageTitleList::OnItemClicked( QListWidgetItem* item )
{
	if (item != NULL)
	{
		QVariant	theData = item->data(Qt::UserRole);

		emit PTL_PageSelected(theData.toUInt());
	}
}

//--------------------------------------------------------------------------
void CPageTitleList::OnPageTitleChanged( CPageData pageData )
{
	QListWidgetItem*	pItem = FindItem(pageData.m_pageId);

	if (pItem != NULL)
	{
		pItem->setText(pageData.m_title);
	}
}

//--------------------------------------------------------------------------
void CPageTitleList::OnPageDeleted(ENTITY_ID pageId)
{
	QListWidgetItem*	pItem = FindItem(pageId);

	if (pItem != NULL)
	{
		int rowNum = row(pItem);
		QListWidgetItem*	pRemovedItem = takeItem(rowNum);
		delete pRemovedItem;
	}
}

//--------------------------------------------------------------------------
void CPageTitleList::OnPageImported( CPageData pageData )
{
	AddItem(pageData.m_pageId, pageData.m_title);
}

//--------------------------------------------------------------------------
void CPageTitleList::OnPageUpdatedByImport( CPageData pageData )
{
	QListWidgetItem*	pItem = FindItem(pageData.m_pageId);

	if (pItem != NULL)
	{
		pItem->setText(pageData.m_title);
	}
}
