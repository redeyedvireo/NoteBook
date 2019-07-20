#include "stdafx.h"
#include "FavoritesWidget.h"
#include "Database.h"
#include "notebook.h"

//--------------------------------------------------------------------------
CFavoritesWidget::CFavoritesWidget(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
}

//--------------------------------------------------------------------------
CFavoritesWidget::~CFavoritesWidget()
{

}

//--------------------------------------------------------------------------
void CFavoritesWidget::Initialize( CNoteBook *parent, CDatabase *db )
{
    m_db = db;
	SetConnections(parent);
}

//--------------------------------------------------------------------------
void CFavoritesWidget::Clear()
{
	ui.favoritesListWidget->clear();
}

//--------------------------------------------------------------------------
void CFavoritesWidget::SetConnections( CNoteBook *parent )
{
	connect(parent, SIGNAL(MW_PageTitleChanged(CPageData)), this, SLOT(OnPageTitleChanged(CPageData)));
	connect(parent, SIGNAL(MW_PageDeleted(ENTITY_ID)), this, SLOT(OnPageDeleted(ENTITY_ID)));
	connect(parent, SIGNAL(MW_PageImported(CPageData)), this, SLOT(OnPageImported(CPageData)));
	connect(parent, SIGNAL(MW_PageUpdatedByImport(CPageData)), this, SLOT(OnPageUpdatedByImport(CPageData)));

	connect(ui.favoritesListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(OnItemClicked(QListWidgetItem*)));
}

//--------------------------------------------------------------------------
void CFavoritesWidget::AddPage( ENTITY_ID pageId )
{
	QString		pageTitle;

    if (m_db->GetPageTitle(pageId, pageTitle))
	{
		AddItem(pageId, pageTitle);
	}
}

//--------------------------------------------------------------------------
void CFavoritesWidget::RemovePage( ENTITY_ID pageId )
{
	QListWidgetItem		*pItem = FindItem(pageId);
	if (pItem != NULL)
	{
		QVariant	theData = pItem->data(Qt::UserRole);
		int			curPageId = theData.toUInt();

		ui.favoritesListWidget->takeItem(ui.favoritesListWidget->row(pItem));
	}
}

//--------------------------------------------------------------------------
void CFavoritesWidget::AddPages( ENTITY_LIST& pageIds )
{
	foreach (ENTITY_ID pageId, pageIds)
	{
		AddPage(pageId);
	}
}

//--------------------------------------------------------------------------
QListWidgetItem* CFavoritesWidget::NewItem( ENTITY_ID pageId, const QString& pageTitle )
{
	QListWidgetItem*	pNewItem = new (std::nothrow) QListWidgetItem(pageTitle);

	if (pNewItem != NULL)
	{
		pNewItem->setData(Qt::UserRole, QVariant(pageId));
	}

	return pNewItem;
}

//--------------------------------------------------------------------------
void CFavoritesWidget::AddItem( ENTITY_ID pageId, const QString& pageTitle )
{
	if (FindItem(pageId) == NULL)
	{
		QListWidgetItem	*pNewItem = NewItem(pageId, pageTitle);

		ui.favoritesListWidget->addItem(pNewItem);
	}
}

//--------------------------------------------------------------------------
QListWidgetItem* CFavoritesWidget::FindItem( ENTITY_ID pageId )
{
	for (int i = 0; i < ui.favoritesListWidget->count(); i++)
	{
		QListWidgetItem*	pItem = ui.favoritesListWidget->item(i);

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

void CFavoritesWidget::on_clearButton_clicked()
{
	// Update database
	int				numRows = ui.favoritesListWidget->count();
	ENTITY_LIST		pageIdList;

	for (int curRow = 0; curRow < numRows; curRow++)
	{
		QListWidgetItem		*pCurItem = ui.favoritesListWidget->item(curRow);

		if (pCurItem != NULL)
		{
			QVariant	theData = pCurItem->data(Qt::UserRole);
			int			curPageId = theData.toUInt();

			emit FW_PageDefavorited(curPageId);		// Update database
		}
	}

	// Remove items from GUI
	Clear();
}

//--------------------------------------------------------------------------
void CFavoritesWidget::on_removeSelectedButton_clicked()
{
	QList<QListWidgetItem*>		selectedItemList;

	selectedItemList = ui.favoritesListWidget->selectedItems();

	if (selectedItemList.length() > 0)
	{
		// Just remove the first one (only one should ever be selected anyway)
		QListWidgetItem		*pSelItem = selectedItemList.first();

		if (pSelItem != NULL)
		{
			QVariant	theData = pSelItem->data(Qt::UserRole);
			int			curPageId = theData.toUInt();

			// Update database
			emit FW_PageDefavorited(curPageId);

			ui.favoritesListWidget->takeItem(ui.favoritesListWidget->row(pSelItem));
		}
	}
}

//--------------------------------------------------------------------------
void CFavoritesWidget::OnPageTitleChanged( CPageData pageData )
{
	QListWidgetItem*	pItem = FindItem(pageData.m_pageId);

	if (pItem != NULL)
	{
		pItem->setText(pageData.m_title);
	}
}

//--------------------------------------------------------------------------
void CFavoritesWidget::OnPageDeleted( ENTITY_ID pageId )
{
	QListWidgetItem*	pItem = FindItem(pageId);

	if (pItem != NULL)
	{
		int rowNum = ui.favoritesListWidget->row(pItem);
		QListWidgetItem*	pRemovedItem = ui.favoritesListWidget->takeItem(rowNum);
		delete pRemovedItem;
	}
}

//--------------------------------------------------------------------------
void CFavoritesWidget::OnPageImported( CPageData pageData )
{
	AddItem(pageData.m_pageId, pageData.m_title);
}

//--------------------------------------------------------------------------
void CFavoritesWidget::OnPageUpdatedByImport( CPageData pageData )
{
	QListWidgetItem*	pItem = FindItem(pageData.m_pageId);

	if (pItem != NULL)
	{
		pItem->setText(pageData.m_title);
	}
}

//--------------------------------------------------------------------------
void CFavoritesWidget::OnItemClicked( QListWidgetItem* item )
{
	if (item != NULL)
	{
		QVariant	theData = item->data(Qt::UserRole);

		emit FW_PageSelected(theData.toUInt());
	}
}
