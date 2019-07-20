#include "stdafx.h"
#include "PreviewImportDialog.h"
#include <QDebug>


//--------------------------------------------------------------------------
CPreviewImportDialog::CPreviewImportDialog( const IMPORT_PAGE_LIST& inPageList, QWidget *parent /*= 0*/ )
	: QDialog(parent)
{
	ui.setupUi(this);

	m_keepList = inPageList;		// Initially, assume all items are to be kept

	InitControls();
}

//--------------------------------------------------------------------------
CPreviewImportDialog::~CPreviewImportDialog()
{
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::InitControls()
{
	m_currentKeepIndex = 0;
	m_currentSkipIndex = 0;

	if (m_keepList.size() > 0)
	{
		// Set Number of Tiddlers text.  Only do this once.
		QString		numTidsStr = QString("%1 Tiddlers Found").arg(m_keepList.size());
		ui.numTiddlersLabel->setText(numTidsStr);

		ui.keepHorizontalScrollBar->setMinimum(0);
		ui.keepHorizontalScrollBar->setMaximum(m_keepList.size() - 1);

		DisplayKeepEntry(m_currentKeepIndex);
	}

	ui.skipHorizontalScrollBar->setMinimum(0);
	ui.skipHorizontalScrollBar->setMaximum(0);

}

//--------------------------------------------------------------------------
void CPreviewImportDialog::DisplayKeepEntry(int index)
{
	if (index >= m_keepList.size())
		return;

	m_currentKeepIndex = index;

	ui.keepCreatorLabel->setText(m_keepList.at(index).m_pageModifier);
	ui.keepPageTitleLabel->setText(m_keepList.at(index).m_title);
	ui.keepDateCreatedLabel->setText(QString("%1").arg(m_keepList.at(index).m_createdDateTime.toString(Qt::SystemLocaleLongDate)));
	ui.keepDateModifiedLabel->setText(QString("%1").arg(m_keepList.at(index).m_modifiedDateTime.toString(Qt::SystemLocaleLongDate)));
	ui.keepNumModsLabel->setText(QString("%1").arg(m_keepList.at(index).m_numModifications));
	ui.keepPageBrowser->setHtml(m_keepList.at(index).m_contentString);
	ui.keepTagsLabel->setText(QString("%1").arg(m_keepList.at(index).m_tags));

	ui.keepHorizontalScrollBar->setValue(index);
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::DisplaySkipEntry(int index)
{
	if (index >= m_skipList.size())
		return;

	m_currentSkipIndex = index;

	ui.skipCreatorLabel->setText(m_skipList.at(index).m_pageModifier);
	ui.skipPageTitleLabel->setText(m_skipList.at(index).m_title);
	ui.skipDateCreatedLabel->setText(QString("%1").arg(m_skipList.at(index).m_createdDateTime.toString(Qt::SystemLocaleLongDate)));
	ui.skipDateModifiedLabel->setText(QString("%1").arg(m_skipList.at(index).m_modifiedDateTime.toString(Qt::SystemLocaleLongDate)));
	ui.skipNumModsLabel->setText(QString("%1").arg(m_skipList.at(index).m_numModifications));
	ui.skipPageBrowser->setHtml(m_skipList.at(index).m_contentString);
	ui.skipTagsLabel->setText(QString("%1").arg(m_skipList.at(index).m_tags));

	ui.skipHorizontalScrollBar->setValue(index);
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::MovePageToSkipList(int index)
{
	if (index < m_keepList.size())
	{
		CImportPageData		pageData = m_keepList.takeAt(index);

		m_skipList.append(pageData);

		// Scroll skip list to end
		m_currentSkipIndex = m_skipList.size() - 1;

		UpdateKeepList();
		UpdateSkipList();
	}
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::MovePageToKeepList(int index)
{
	if (index < m_skipList.size())
	{
		CImportPageData		pageData = m_skipList.takeAt(index);

		m_keepList.append(pageData);

		// Scroll keep list to end
		m_currentKeepIndex = m_keepList.size() - 1;

		UpdateKeepList();
		UpdateSkipList();
	}
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::UpdateKeepList()
{
	ui.keepHorizontalScrollBar->setMaximum(m_keepList.size() - 1);

	if (m_currentKeepIndex >= m_keepList.size())
	{
		m_currentKeepIndex = m_keepList.size() - 1;
	}

	qDebug() << "KeepList size: " << m_keepList.size() << ", keepIndex: " << m_currentKeepIndex;
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::UpdateSkipList()
{
	ui.skipHorizontalScrollBar->setMaximum(m_skipList.size() - 1);

	if (m_currentSkipIndex >= m_skipList.size())
	{
		m_currentSkipIndex = m_skipList.size() - 1;
	}

	qDebug() << "SkipList size: " << m_skipList.size() << ", skipIndex: " << m_currentSkipIndex;
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::on_keepHorizontalScrollBar_valueChanged( int index )
{
	DisplayKeepEntry(index);
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::on_skipHorizontalScrollBar_valueChanged( int index )
{
	DisplaySkipEntry(index);
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::on_addPageButton_clicked()
{
	MovePageToKeepList(m_currentSkipIndex);

	DisplaySkipEntry(m_currentSkipIndex);
	DisplayKeepEntry(m_currentKeepIndex);
}

//--------------------------------------------------------------------------
void CPreviewImportDialog::on_removePageButton_clicked()
{
	MovePageToSkipList(m_currentKeepIndex);

	DisplayKeepEntry(m_currentKeepIndex);
	DisplaySkipEntry(m_currentSkipIndex);
}

//--------------------------------------------------------------------------
PAGE_LIST CPreviewImportDialog::GetPages()
{
	PAGE_LIST	pageList = PAGE_LIST();

	foreach (CImportPageData importPageData, m_keepList)
	{
		CPageData	pageData = importPageData.ToPageData();
		pageList.append(pageData);
	}

	return pageList;
}
