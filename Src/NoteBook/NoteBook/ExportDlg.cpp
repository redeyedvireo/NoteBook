#include "stdafx.h"
#include "ExportDlg.h"
#include "Database.h"
#include <QFileInfo>
#include <QFileDialog>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif


//--------------------------------------------------------------------------
CExportDlg::CExportDlg(CDatabase *db, QWidget *parent)
    : m_db(db), QDialog(parent)
{
	ui.setupUi(this);

#if QT_VERSION >= 0x050000
	m_currentDir.setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
#else
	m_currentDir.setPath(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
#endif

	PopulatePages();
	on_allButton_clicked();		// Select all pages by default

	UpdateOkButton();
}

//--------------------------------------------------------------------------
CExportDlg::~CExportDlg()
{

}

//--------------------------------------------------------------------------
ENTITY_LIST CExportDlg::GetPageList()
{
	ENTITY_LIST		pageIdList;

	int		numItems = ui.pageListWidget->count();

	for (int row = 0; row < numItems; row++)
	{
		QListWidgetItem		*pCurItem = ui.pageListWidget->item(row);

		if (pCurItem->checkState() == Qt::Checked)
		{
			int		pageId = GetPageIdForRow(row);
			pageIdList.append(pageId);
		}
	}

	return pageIdList;
}

//--------------------------------------------------------------------------
int CExportDlg::ExportFormat()
{
	return ui.richTextRadioButton->isChecked() ? kRichText : kPlainText;
}

//--------------------------------------------------------------------------
QString CExportDlg::FilePath()
{
	return ui.pathEdit->text();
}

//--------------------------------------------------------------------------
void CExportDlg::PopulatePages()
{
	PAGE_HASH	pageHash;

    m_db->GetPageList(pageHash);

	if (pageHash.size() == 0)
		return;

	// Add the rest of the pages (these should all be regular pages; no folders).
	QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	pageIt(pageHash);

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
void CExportDlg::AddPageTitle(SHARED_CPAGEDATA pageData)
{
	QListWidgetItem		*pItem = new (std::nothrow) QListWidgetItem(pageData.data()->m_title);

	if (pItem != NULL)
	{
		pItem->setFlags(pItem->flags() | Qt::ItemIsUserCheckable);
		pItem->setCheckState(Qt::Unchecked);

		// Set the page ID in the data
		pItem->setData(Qt::UserRole, QVariant(pageData.data()->m_pageId));

		ui.pageListWidget->addItem(pItem);
	}
}

//--------------------------------------------------------------------------
ENTITY_ID CExportDlg::GetPageIdForRow(int row)
{
	ENTITY_ID			pageId = kInvalidPageId;
	QListWidgetItem*	pItem = ui.pageListWidget->item(row);

	if (pItem != NULL)
	{
		QVariant	itemData = pItem->data(Qt::UserRole);
		pageId = itemData.toInt();
	}

	return pageId;
}

//--------------------------------------------------------------------------
bool CExportDlg::AnyPagesSelected()
{
	int		numItems = ui.pageListWidget->count();

	for (int row = 0; row < numItems; row++)
	{
		QListWidgetItem		*pCurItem = ui.pageListWidget->item(row);

		if (pCurItem->checkState() == Qt::Checked)
			return true;
	}

	return false;
}

//--------------------------------------------------------------------------
void CExportDlg::UpdateOkButton()
{
	bool	bEnable = true;

	if (ui.pathEdit->text().isEmpty() || !AnyPagesSelected())
	{
		bEnable = false;
	}

	ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(bEnable);
}

//--------------------------------------------------------------------------
void CExportDlg::on_allButton_clicked()
{
	int		numItems = ui.pageListWidget->count();

	for (int row = 0; row < numItems; row++)
	{
		QListWidgetItem		*pCurItem = ui.pageListWidget->item(row);

		pCurItem->setCheckState(Qt::Checked);
	}
}

//--------------------------------------------------------------------------
void CExportDlg::on_noneButton_clicked()
{
	int		numItems = ui.pageListWidget->count();

	for (int row = 0; row < numItems; row++)
	{
		QListWidgetItem		*pCurItem = ui.pageListWidget->item(row);

		pCurItem->setCheckState(Qt::Unchecked);
	}
}

//--------------------------------------------------------------------------
void CExportDlg::on_browseButton_clicked()
{
	QString		filePath;
	QString		fileFilterStr;

	if (ExportFormat() == kPlainText)
	{
		fileFilterStr = QString(tr("Text Files (*.txt)"));
	}
	else
	{
		fileFilterStr = QString(tr("HTML Files (*.html)"));
	}

#if QT_VERSION >= 0x050000
	QDir		settingsDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
#else
	QDir		settingsDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
#endif

	filePath = QFileDialog::getSaveFileName(this,
											tr("Export File"),
											m_currentDir.absolutePath(),
											fileFilterStr);

	if (! filePath.isEmpty())
	{
		ui.pathEdit->setText(filePath);

		QFileInfo		fileInfo(filePath);
		m_currentDir = fileInfo.absoluteDir();
	}
}

//--------------------------------------------------------------------------
void CExportDlg::on_pathEdit_textChanged()
{
	UpdateOkButton();
}

//--------------------------------------------------------------------------
void CExportDlg::on_pageListWidget_itemClicked( QListWidgetItem *item )
{
	UpdateOkButton();
}
