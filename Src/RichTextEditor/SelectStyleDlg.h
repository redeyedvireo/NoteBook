#ifndef SELECTSTYLEDLG_H
#define SELECTSTYLEDLG_H

#include <QDialog>
#include "StyleManager.h"
#include "ui_SelectStyleDlg.h"

class CSelectStyleDlg : public QDialog
{
	Q_OBJECT

public:
    CSelectStyleDlg(CStyleManager *styleManager, QWidget *parent = 0);
	~CSelectStyleDlg();

	int GetSelectedStyle();

public slots:
	void on_styleList_currentRowChanged(int currentRow);
	void on_newButton_clicked();
	void on_deleteButton_clicked();
	void on_editButton_clicked();

private:
	Ui::SelectStyleDlg		ui;
    CStyleManager			*m_styleManager;

	void LoadStyles();
	void AddStyle(const QString& styleName, int styleId);
	int GetStyleIdForRow(int row);
	QString GetStyleNameForRow(int row);
};

#endif // SELECTSTYLEDLG_H
