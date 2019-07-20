#ifndef STYLEDLG_H
#define STYLEDLG_H

#include <QDialog>
#include "StyleDef.h"
#include "ui_StyleDlg.h"

class CStyleDlg : public QDialog
{
	Q_OBJECT

public:
	CStyleDlg(const StyleDef& inStyleDef, QWidget *parent = 0);
	~CStyleDlg();

	StyleDef GetStyle();

private slots:
	void on_fontButton_clicked();
	void OnColorChanged();
	void UpdateOkButton();

protected:
	void PopulateDialog(const StyleDef& inStyleDef);
	QString OnOrOffToString(const QString& styleItem, bool bOn);
	void UpdateFontLabel();
	void UpdateResultLabel();

private:
	Ui::CStyleDlg			ui;

	QFont					m_currentFont;
};

#endif // STYLEDLG_H
