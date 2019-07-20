#include "stdafx.h"
#include "SearchWidget.h"
#include "Database.h"
#include "notebook.h"
#include <QTextDocument>


//--------------------------------------------------------------------------
CSearchWidget::CSearchWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

//--------------------------------------------------------------------------
CSearchWidget::~CSearchWidget()
{
}

//--------------------------------------------------------------------------
void CSearchWidget::Initialize(CNoteBook *parent , CDatabase *db)
{
    m_db = db;
	SetConnections(parent);
}

//--------------------------------------------------------------------------
void CSearchWidget::Clear()
{
	ui.resultsListWidget->clear();
	ui.searchEdit->clear();
}

//--------------------------------------------------------------------------
void CSearchWidget::SetConnections(CNoteBook *parent)
{
	connect(parent, SIGNAL(MW_PageTitleChanged(CPageData)), this, SLOT(OnPageTitleChanged(CPageData)));
	connect(parent, SIGNAL(MW_PageDeleted(ENTITY_ID)), this, SLOT(OnPageDeleted(ENTITY_ID)));
	connect(parent, SIGNAL(MW_PageImported(CPageData)), this, SLOT(OnPageImported(CPageData)));
	connect(parent, SIGNAL(MW_PageUpdatedByImport(CPageData)), this, SLOT(OnPageUpdatedByImport(CPageData)));

	connect(ui.resultsListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(OnItemClicked(QListWidgetItem*)));
	connect(ui.searchEdit, SIGNAL(returnPressed()), this, SLOT(on_searchButton_clicked()));
}

//--------------------------------------------------------------------------
void CSearchWidget::on_searchButton_clicked()
{
	QString		searchText = ui.searchEdit->text();

	DoSearch(searchText);
}

//--------------------------------------------------------------------------
void CSearchWidget::DoSearch(const QString& searchText)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	ui.resultsListWidget->clear();

	PAGE_HASH			pageHash;

    m_db->GetPageList(pageHash);

	QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	it(pageHash);

	while (it.hasNext())
	{
		it.next();

		bool				bFound = false;
		SHARED_CPAGEDATA	pageData = it.value();

		// Search title
		if (pageData->m_title.contains(searchText, Qt::CaseInsensitive))
		{
			bFound = true;
		}
		else
		{
			CPageData		fullPageData;
            if (m_db->GetPage(pageData->m_pageId, fullPageData))
			{
				// Since pages are stored as HTML, it is necessary to create a QTextDocument
				// with the text, then pull out the text with plainText.  Then, the plain text
				// can be searched.
				QTextDocument		textDoc;

				textDoc.setHtml(fullPageData.m_contentString);

				if (textDoc.toPlainText().contains(searchText, Qt::CaseInsensitive))
				{
					bFound = true;
				}
			}
		}

		if (bFound)
		{
			AddItem(pageData->m_pageId, pageData->m_title);
		}
	}

	QApplication::restoreOverrideCursor();
}

//--------------------------------------------------------------------------
QListWidgetItem* CSearchWidget::NewItem(ENTITY_ID pageId, const QString& pageTitle)
{
	QListWidgetItem*	pNewItem = new (std::nothrow) QListWidgetItem(pageTitle);

	if (pNewItem != NULL)
	{
		pNewItem->setData(Qt::UserRole, QVariant(pageId));
	}

	return pNewItem;
}

//--------------------------------------------------------------------------
void CSearchWidget::AddItem( ENTITY_ID pageId, const QString& pageTitle )
{
	QListWidgetItem	*pNewItem = NewItem(pageId, pageTitle);

	ui.resultsListWidget->addItem(pNewItem);
}

//--------------------------------------------------------------------------
QListWidgetItem* CSearchWidget::FindItem( ENTITY_ID pageId )
{
	for (int i = 0; i < ui.resultsListWidget->count(); i++)
	{
		QListWidgetItem*	pItem = ui.resultsListWidget->item(i);

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

void CSearchWidget::OnPageTitleChanged( CPageData pageData )
{
	QListWidgetItem*	pItem = FindItem(pageData.m_pageId);

	if (pItem != NULL)
	{
		pItem->setText(pageData.m_title);
	}
}

//--------------------------------------------------------------------------
void CSearchWidget::OnPageDeleted(ENTITY_ID pageId)
{
	QListWidgetItem*	pItem = FindItem(pageId);

	if (pItem != NULL)
	{
		int rowNum = ui.resultsListWidget->row(pItem);
		QListWidgetItem*	pRemovedItem = ui.resultsListWidget->takeItem(rowNum);
		delete pRemovedItem;
	}
}

//--------------------------------------------------------------------------
void CSearchWidget::OnPageImported( CPageData pageData )
{
	AddItem(pageData.m_pageId, pageData.m_title);
}

//--------------------------------------------------------------------------
void CSearchWidget::OnPageUpdatedByImport( CPageData pageData )
{
	QListWidgetItem*	pItem = FindItem(pageData.m_pageId);

	if (pItem != NULL)
	{
		pItem->setText(pageData.m_title);
	}
}

//--------------------------------------------------------------------------
void CSearchWidget::OnItemClicked( QListWidgetItem* item )
{
	if (item != NULL)
	{
		QVariant	theData = item->data(Qt::UserRole);

		emit SW_PageSelected(theData.toUInt());
	}
}
