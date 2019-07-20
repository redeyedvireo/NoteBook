#include "PreviewImportDialog.h"
#include <QHashIterator>
#include <QSharedPointer>
#include "ImportDatabase.h"


//--------------------------------------------------------------------------
CPreviewImportDialog::CPreviewImportDialog(QWidget *parent, PAGE_HASH& pageHash, CImportDatabase *pDatabase)
	: QDialog(parent), m_pageHash(pageHash), m_pDatabase(pDatabase)
{
	ui.setupUi(this);

	PopulatePages();
}

//--------------------------------------------------------------------------
CPreviewImportDialog::~CPreviewImportDialog()
{

}

//--------------------------------------------------------------------------
PAGE_LIST CPreviewImportDialog::GetPages()
{
	PAGE_LIST		pageList;

	int		numItems = ui.listWidget->count();

	for (int row = 0; row < numItems; row++)
	{
		QListWidgetItem		*pCurItem = ui.listWidget->item(row);

		if (pCurItem->checkState() == Qt::Checked)
		{
			// This one is checked.  Add it to the list
			ENTITY_ID	pageId = GetPageIdForRow(row);
			CPageData	pageData;

			if (m_pDatabase->GetPage(pageId, pageData))
			{
				// For now, I'm imposing the restriction that only pages
				// can be imported, rather than folders with pages in them.
				// This requires that the parentId field be set to kInvalidPageId.
				pageData.m_parentId = kInvalidPageId;
				pageData.m_bIsFavorite = false;

				pageList.append(pageData);
			}
		}
	}

	return pageList;
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::PopulatePages()
{
	if (m_pageHash.size() == 0)
		return;

	// Add the rest of the pages (these should all be regular pages; no folders).
	QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	pageIt(m_pageHash);

	while (pageIt.hasNext())
	{
		pageIt.next();

		SHARED_CPAGEDATA	pageData = pageIt.value();

		if (pageData.data()->m_pageType == kPageTypeUserText)
		{
			AddPageTitle(pageData);
		}
	}
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::AddPageTitle(SHARED_CPAGEDATA pageData)
{
	QListWidgetItem		*pItem = new (std::nothrow) QListWidgetItem(pageData.data()->m_title);

	if (pItem != NULL)
	{
		pItem->setFlags(pItem->flags() | Qt::ItemIsUserCheckable);
		pItem->setCheckState(Qt::Unchecked);

		// Set the page ID in the data
		pItem->setData(Qt::UserRole, QVariant(pageData.data()->m_pageId));

		ui.listWidget->addItem(pItem);
	}
}

//--------------------------------------------------------------------------
ENTITY_ID CPreviewImportDialog::GetPageIdForRow(int row)
{
	ENTITY_ID			pageId = kInvalidPageId;
	QListWidgetItem*	pItem = ui.listWidget->item(row);

	if (pItem != NULL)
	{
		QVariant	itemData = pItem->data(Qt::UserRole);
		pageId = itemData.toInt();
	}

	return pageId;
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::on_selectAllButton_clicked()
{
	int		numItems = ui.listWidget->count();

	for (int row = 0; row < numItems; row++)
	{
		QListWidgetItem		*pCurItem = ui.listWidget->item(row);

		pCurItem->setCheckState(Qt::Checked);
	}
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::on_selectNoneButton_clicked()
{
	int		numItems = ui.listWidget->count();

	for (int row = 0; row < numItems; row++)
	{
		QListWidgetItem		*pCurItem = ui.listWidget->item(row);

		pCurItem->setCheckState(Qt::Unchecked);
	}
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::on_listWidget_currentRowChanged(int currentRow)
{
	// Display the page in the page pane
	ENTITY_ID	pageId = GetPageIdForRow(currentRow);

	DisplayPage(pageId);
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::DisplayPage(ENTITY_ID pageId)
{
	CPageData		pageData;

	if (m_pDatabase->GetPage(pageId, pageData))
	{
		ui.pageTitleLabel->setText(pageData.m_title);
		ui.createdLabel->setText(pageData.m_createdDateTime.toString(Qt::SystemLocaleLongDate));
		ui.modifiedLabel->setText(pageData.m_modifiedDateTime.toString(Qt::SystemLocaleLongDate));
		ui.numModsLabel->setText(QString("%1").arg(pageData.m_numModifications));
		ui.tagsLabel->setText(pageData.m_tags);

		ui.textBrowser->setHtml(pageData.m_contentString);
	}
}
