#include "stdafx.h"
#include "TableFormatDlg.h"
#include "TextTableDefs.h"
#include "CTextTable.h"
#include <QDebug>

#define kTableColumns   2
#define kWidthColumn    0
#define kTypeColumn     1

//--------------------------------------------------------------------------
CTableFormatDlg::CTableFormatDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

    m_pTextTable = NULL;

	ui.headerBackgroundButton->SetColorSwatchFillsButton(true);
	ui.backgroundColorButton->SetColorSwatchFillsButton(true);
	ui.headerBackgroundButton->SetNoColor();
	ui.backgroundColorButton->SetNoColor();
}

//--------------------------------------------------------------------------
CTableFormatDlg::~CTableFormatDlg()
{

}


//--------------------------------------------------------------------------
void CTableFormatDlg::on_columnsSpin_valueChanged(int i)
{
    AdjustTableRows(i);
}

//--------------------------------------------------------------------------
QVector<QTextLength> CTableFormatDlg::GetColumnConstraints()
{
    QVector<QTextLength>	columnConstraints;
    int                     numColumns = ui.tableWidget->rowCount();

    for (int i = 0; i < numColumns; i++)
    {
        qreal           colWidth = GetTableValue(i, 0, kDefaultColumnWidth);
        QTextLength		textLength(QTextLength::FixedLength, colWidth);
        columnConstraints << textLength;
    }

    return columnConstraints;
}

//--------------------------------------------------------------------------
void CTableFormatDlg::SetHeaderBackgroundColor(const QColor& headerColor)
{
	ui.headerBackgroundButton->SetColor(headerColor);
}

//--------------------------------------------------------------------------
void CTableFormatDlg::SetBackgroundColor(const QColor& bgColor)
{
    ui.backgroundColorButton->SetColor(bgColor);
}

//--------------------------------------------------------------------------
void CTableFormatDlg::SetTable(CTextTable *pTable)
{
    m_pTextTable = pTable;      // Note: This could be NULL.

    PopulateColumns();

    if (pTable != NULL)
    {
        ui.rowsSpin->setValue(m_pTextTable->rows());
        ui.columnsSpin->setValue(m_pTextTable->columns());
    }
}

//--------------------------------------------------------------------------
void CTableFormatDlg::SetTableValue(int row, int col, qreal value)
{
    SetTableText(row, col, QString::number((double) value));
}

//--------------------------------------------------------------------------
qreal CTableFormatDlg::GetTableValue(int row, int col, qreal defaultValue)
{
    qreal               retVal = defaultValue;
    QTableWidgetItem    *pItem = ui.tableWidget->item(row, col);

    if (pItem != NULL)
    {
        QString     cellText = pItem->text();
        retVal = cellText.toDouble();
    }

    return retVal;
}

//--------------------------------------------------------------------------
void CTableFormatDlg::SetTableText(int row, int col, QString text)
{
    QTableWidgetItem    *pItem = ui.tableWidget->item(row, col);

    if (pItem == NULL)
    {
        // Need to create a new item for this position
        pItem = new QTableWidgetItem();
        ui.tableWidget->setItem(row, col, pItem);
    }

    pItem->setText(text);
}

//--------------------------------------------------------------------------
void CTableFormatDlg::PopulateColumns()
{
    int                     numColumns;
    QVector<QTextLength>    colWidths;

    if (m_pTextTable == NULL)
    {
        // No table exists; a new table will be created.  Create a default table format.
        numColumns = 1;

        QTextLength		textLength(QTextLength::FixedLength, kDefaultColumnWidth);
        colWidths << textLength;
    }
    else
    {
        numColumns = m_pTextTable->columns();

        QTextTableFormat        tableFormat = m_pTextTable->textTableFormat();
        colWidths = tableFormat.columnWidthConstraints();
    }

    ui.tableWidget->setColumnCount(kTableColumns);
    ui.tableWidget->setRowCount(numColumns);

    // Set Header Labels
    ui.tableWidget->setHorizontalHeaderLabels(QStringList() << "Width" << "Type");

    for (int col = 0; col < numColumns; col++)
    {
        // Get column width for column
        QTextLength     textLength = colWidths.at(col);
        SetTableValue(col, kWidthColumn, textLength.rawValue());

        // For now, make all columns "fixed"
        SetTableText(col, kTypeColumn, "Fixed");
    }

    QHeaderView *pHorizHeader = ui.tableWidget->horizontalHeader();
    pHorizHeader->setStretchLastSection(true);
}

//--------------------------------------------------------------------------
void CTableFormatDlg::AdjustTableRows(int numRows)
{
    int     currentNumRows = ui.tableWidget->rowCount();

    if (numRows == currentNumRows)
    {
        return;     // Nothing to do
    }

    ui.tableWidget->setRowCount(numRows);

    if (numRows > currentNumRows)
    {
        // Add rows, and set text table column constrains to default values
        int  rowsToAdd = numRows - currentNumRows;

        for (int i = currentNumRows; i < numRows; i++)
        {
            SetTableValue(i, kWidthColumn, kDefaultColumnWidth);

            // For now, make all columns "fixed"
            SetTableText(i, kTypeColumn, "Fixed");
        }
    }
}
