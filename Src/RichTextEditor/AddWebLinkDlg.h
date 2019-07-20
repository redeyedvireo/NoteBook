#ifndef ADDWEBLINKDLG_H
#define ADDWEBLINKDLG_H

#include <QDialog>
#include "ui_AddWebLinkDlg.h"

class CAddWebLinkDlg : public QDialog
{
	Q_OBJECT

public:
	CAddWebLinkDlg(QWidget *parent = 0);
	~CAddWebLinkDlg();

	void GetLink(QString& urlStr, QString& descriptionStr);

private:
	Ui::CAddWebLinkDlg ui;
};

#endif // ADDWEBLINKDLG_H
