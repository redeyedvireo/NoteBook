#ifndef TABLEFORMATDLG_H
#define TABLEFORMATDLG_H

#include <QDialog>
#include "ui_TableFormatDlg.h"

class CTextTable;

class CTableFormatDlg : public QDialog
{
	Q_OBJECT

public:
	CTableFormatDlg(QWidget *parent = 0);
	~CTableFormatDlg();

    void SetTable(CTextTable* pTable);

    QVector<QTextLength> GetColumnConstraints();

	void SetHeaderBackgroundColor(const QColor& headerColor);
	void SetBackgroundColor(const QColor& bgColor);

	QColor BackgroundColor()	{ return ui.backgroundColorButton->GetColor(); }
	QColor HeaderBackgroundColor()	{ return ui.headerBackgroundButton->GetColor(); }

	int Rows()		{ return ui.rowsSpin->value(); }
	int Columns()	{ return ui.columnsSpin->value(); }

protected:
    void SetTableValue(int row, int col, qreal value);
    qreal GetTableValue(int row, int col, qreal defaultValue);
    void SetTableText(int row, int col, QString text);
    void PopulateColumns();
    void AdjustTableRows(int numRows);

private slots:
    void on_columnsSpin_valueChanged(int i);

private:
	Ui::TableFormatDlg ui;

    CTextTable      *m_pTextTable;
};

#endif // TABLEFORMATDLG_H
