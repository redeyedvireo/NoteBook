#include "stdafx.h"
#include "AddWebLinkDlg.h"

//--------------------------------------------------------------------------
CAddWebLinkDlg::CAddWebLinkDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

//--------------------------------------------------------------------------
CAddWebLinkDlg::~CAddWebLinkDlg()
{

}

//--------------------------------------------------------------------------
void CAddWebLinkDlg::GetLink( QString& urlStr, QString& descriptionStr )
{
	urlStr = ui.urlEdit->text();
	descriptionStr = ui.descriptionEdit->text();
}
