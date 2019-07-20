#ifndef PAGEINFODLG_H
#define PAGEINFODLG_H

#include <QDialog>
#include "PageData.h"
#include "ui_PageInfoDlg.h"

class CPageInfoDlg : public QDialog
{
	Q_OBJECT

public:
	CPageInfoDlg(const CPageData& thePage, QWidget *parent = 0);
	~CPageInfoDlg();

private:
	Ui::CPageInfoDlg ui;

	void PopulateControls(const CPageData& thePage);
};

#endif // PAGEINFODLG_H
