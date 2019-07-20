#include "stdafx.h"
#include "PageInfoDlg.h"

//--------------------------------------------------------------------------
CPageInfoDlg::CPageInfoDlg(const CPageData& thePage, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	PopulateControls(thePage);
}

//--------------------------------------------------------------------------
CPageInfoDlg::~CPageInfoDlg()
{

}

//--------------------------------------------------------------------------
void CPageInfoDlg::PopulateControls(const CPageData& thePage)
{
	ui.pageTitleLabel->setText(thePage.m_title);
	ui.createdLabel->setText(thePage.m_createdDateTime.toString(Qt::SystemLocaleLongDate));
	ui.modifiedLabel->setText(thePage.m_modifiedDateTime.toString(Qt::SystemLocaleLongDate));
	ui.numChangesLabel->setText(QString("%1").arg(thePage.m_numModifications));

	// Character count
	QTextDocument	tempDoc;

	tempDoc.setHtml(thePage.m_contentString);
	ui.sizeLabel->setText((QString("%1 characters").arg(tempDoc.characterCount())));
}
