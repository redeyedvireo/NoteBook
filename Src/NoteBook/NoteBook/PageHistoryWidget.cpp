#include "stdafx.h"
#include "PageHistoryWidget.h"
#include "notebook.h"

//--------------------------------------------------------------------------
CPageHistoryWidget::CPageHistoryWidget(QWidget *parent)
	: QListWidget(parent), m_maxHistory(kDefaultMaxHistory)
{
	setSelectionMode(QAbstractItemView::NoSelection);
}

//--------------------------------------------------------------------------
CPageHistoryWidget::~CPageHistoryWidget()
{

}

//--------------------------------------------------------------------------
void CPageHistoryWidget::Initialize(CNoteBook *parent , CDatabase *db)
{
    m_db = db;
	SetConnections(parent);
}

//--------------------------------------------------------------------------
void CPageHistoryWidget::SetConnections(CNoteBook *parent)
{
	connect(parent, SIGNAL(MW_PageTitleChanged(CPageData)), this, SLOT(OnPageTitleChanged(CPageData)));
	connect(parent, SIGNAL(MW_PageDeleted(ENTITY_ID)), this, SLOT(OnPageDeleted(ENTITY_ID)));
	connect(parent, SIGNAL(MW_PageImported(CPageData)), this, SLOT(OnPageImported(CPageData)));
	connect(parent, SIGNAL(MW_PageUpdatedByImport(CPageData)), this, SLOT(OnPageUpdatedByImport(CPageData)));
	connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(OnItemClicked(QListWidgetItem*)));
}

//--------------------------------------------------------------------------
QString CPageHistoryWidget::GetPageHistory()
{
	QString			pageHistoryStr;
	int				numRows = count();
	ENTITY_LIST		pageIdList;

	for (int curRow = 0; curRow < numRows; curRow++)
	{
		QListWidgetItem		*pCurItem = item(curRow);

		if (pCurItem != NULL)
		{
			QVariant	theData = pCurItem->data(Qt::UserRole);
			int			curPageId = theData.toUInt();

			pageIdList << curPageId;
		}
	}

	pageHistoryStr = CPageData::EntityListToCommaSeparatedList(pageIdList);

	return pageHistoryStr;
}

//--------------------------------------------------------------------------
void CPageHistoryWidget::SetPageHistory( const QString& pageHistoryStr )
{
	ENTITY_LIST		pageIdList;

	clear();

	CPageData::CommaSeparatedListToEntityList(pageHistoryStr, pageIdList);

	foreach(ENTITY_ID curPageId, pageIdList)
	{
		QString		pageTitle;

        if (m_db->GetPageTitle(curPageId, pageTitle))
		{
			AddItem(curPageId, pageTitle, true);		// Add item at the end
		}
	}
}

//--------------------------------------------------------------------------
ENTITY_ID CPageHistoryWidget::GetMostRecentlyViewedPage()
{
	ENTITY_ID	pageId = kInvalidPageId;

	if (count() > 0)
	{
		// First item
		pageId = item(0)->data(Qt::UserRole).toUInt();
	}

	return pageId;
}

//--------------------------------------------------------------------------
QListWidgetItem* CPageHistoryWidget::NewItem(ENTITY_ID pageId, const QString& pageTitle)
{
	QListWidgetItem*	pNewItem = new (std::nothrow) QListWidgetItem(pageTitle);

	if (pNewItem != NULL)
	{
		pNewItem->setData(Qt::UserRole, QVariant(pageId));
	}

	return pNewItem;
}

//--------------------------------------------------------------------------
void CPageHistoryWidget::AddItem( ENTITY_ID pageId, const QString& pageTitle, bool bAddAtEnd )
{
	QListWidgetItem*	pFoundItem = FindItem(pageId);

	if (pFoundItem != NULL)
	{
		// This page already exists.  If it is not the first element,
		// remove it, and place it at the top.  If it is the first element,
		// there is nothing to do.
		int		itemRow = row(pFoundItem);

		if (itemRow > 0)
		{
			takeItem(itemRow);		// Remove the item
		}
		else
			return;		// The item is already at the top, so we're done
	}

	// Add item
	// If the list is already at its maximum, remove the last entry
	if (count() == m_maxHistory)
	{
		// Remove last item
		takeItem(count() - 1);
	}

	QListWidgetItem	*pNewItem = NewItem(pageId, pageTitle);

	if (bAddAtEnd)
	{
		addItem(pNewItem);
	}
	else
	{
		insertItem(0, pNewItem);
	}
}

//--------------------------------------------------------------------------
QListWidgetItem* CPageHistoryWidget::FindItem( ENTITY_ID pageId )
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


/************************************************************************/
/* Slots                                                                */
/************************************************************************/

void CPageHistoryWidget::OnPageTitleChanged( CPageData pageData )
{
	QListWidgetItem*	pItem = FindItem(pageData.m_pageId);

	if (pItem != NULL)
	{
		pItem->setText(pageData.m_title);
	}
}

//--------------------------------------------------------------------------
void CPageHistoryWidget::OnPageDeleted(ENTITY_ID pageId)
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
void CPageHistoryWidget::OnPageImported( CPageData pageData )
{
	AddItem(pageData.m_pageId, pageData.m_title);
}

//--------------------------------------------------------------------------
void CPageHistoryWidget::OnPageUpdatedByImport( CPageData pageData )
{
	QListWidgetItem*	pItem = FindItem(pageData.m_pageId);

	if (pItem != NULL)
	{
		pItem->setText(pageData.m_title);
	}
}

//--------------------------------------------------------------------------
void CPageHistoryWidget::OnItemClicked( QListWidgetItem* item )
{
	if (item != NULL)
	{
		QVariant	theData = item->data(Qt::UserRole);

		emit PHW_PageSelected(theData.toUInt());
	}
}
