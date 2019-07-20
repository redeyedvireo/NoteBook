#include "CTextTable.h"
#include "TextTableDefs.h"

#include <QTextCursor>
#include <QTextDocumentFragment>


//--------------------------------------------------------------------------
CTextTable::CTextTable()
{
    m_pTextTable = NULL;
}

//--------------------------------------------------------------------------
CTextTable::~CTextTable()
{
}

//--------------------------------------------------------------------------
CTextTable* CTextTable::createAtCursor(QTextCursor &cursor, int rows, int columns)
{
    CTextTable  *newObj = new CTextTable();

    newObj->m_pTextTable = cursor.insertTable(rows, columns);
    return newObj;
}

//--------------------------------------------------------------------------
bool CTextTable::CursorInTable(QTextCursor cursor)
{
    return (cursor.currentTable() != NULL);
}

//--------------------------------------------------------------------------
CTextTable *CTextTable::fromCursor(QTextCursor &cursor)
{
    CTextTable  *pTable = NULL;

    if (CursorInTable(cursor))
    {
        pTable = new CTextTable();
        pTable->m_pTextTable = cursor.currentTable();
    }

    return pTable;
}

//--------------------------------------------------------------------------
void CTextTable::selectionToTable(QTextCursor &selectionCursor)
{
    // It would be nice to be able to preserve the formatting, but QTextCursor::toHtml()
    // returns a rather involved block of HTML, which would be tricky to parse, so we'll
    // just work with the plain text.
    QString     allText = selectionCursor.selectedText();

    // Determine the column delimiter.  The default will be a space, but it might be a
    // tab character.  If a tab character is found in the selected text, we'll assume
    // the fields are tab-delimited.
    QString columnDelimiter;

    if (allText.contains("\t"))
    {
        columnDelimiter = "\\t";
    }
    else
    {
        columnDelimiter = "\\s";
    }

    allText.replace(QChar(8233), "\n");     // Replace Unicode paragraph separators with \n
    QStringList lines = allText.split("\n");

    int     numRows = 0, numColumns = 0;

    // Determine number of rows and columns in the table
    foreach (QString line, lines)
    {
        //qDebug() << "Line: " << line;
        QString     tempLine;
        if (columnDelimiter == "\\s")
        {
            tempLine = line.simplified();
        }
        else
        {
            tempLine = line.trimmed();
        }

        if (!tempLine.isEmpty())
        {
            numRows++;
            QStringList lineElements = tempLine.split(QRegExp(columnDelimiter));
            numColumns = qMax(numColumns, lineElements.size());
        }
    }

    // Remove the text from the document
    selectionCursor.removeSelectedText();

    // Insert the table
    QTextTable  *pTable = selectionCursor.insertTable(numRows, numColumns);

    // Add the text
    int     curRow = 0;

    foreach (QString line, lines)
    {
        QString     tempLine;
        if (columnDelimiter == "\\s")
        {
            tempLine = line.simplified();
        }
        else
        {
            tempLine = line.trimmed();
        }

        if (!tempLine.isEmpty())
        {
            int     curCol = 0;
            QStringList lineElements = tempLine.split(QRegExp(columnDelimiter));

            foreach (QString cellText, lineElements)
            {
                QTextTableCell cell = pTable->cellAt(curRow, curCol++);

                QTextCursor cursor = cell.firstCursorPosition();
                cursor.insertText(cellText);
            }

            curRow++;
        }
    }

    // Set some formatting parameters
    QTextTableFormat		tableFormat = pTable->format();
    QVector<QTextLength>	columnConstraints;

    for (int i = 0; i < numColumns; i++)
    {
        QTextLength		textLength(QTextLength::FixedLength, kDefaultColumnWidth);
        columnConstraints << textLength;
    }

    tableFormat.setColumnWidthConstraints(columnConstraints);
    tableFormat.setBackground(QBrush(Qt::white));
    pTable->setFormat(tableFormat);
}

//--------------------------------------------------------------------------
QString CTextTable::GetTableCellText(QTextTable *pTable, int row, int col)
{
    QTextTableCell  cell = pTable->cellAt(row, col);

    // Get the cell's text
    QTextCursor     sourceCellStart = cell.firstCursorPosition();
    QTextCursor     sourceCellEnd = cell.lastCursorPosition();

    sourceCellStart.setPosition(sourceCellEnd.position(), QTextCursor::KeepAnchor);

    return sourceCellStart.selectedText();
}

//--------------------------------------------------------------------------
int CTextTable::columns()
{
    if (m_pTextTable)
    {
        return m_pTextTable->columns();
    }
    else
        return -1;
}

//--------------------------------------------------------------------------
int CTextTable::rows()
{
    if (m_pTextTable)
    {
        return m_pTextTable->rows();
    }
    else
        return -1;
}

//--------------------------------------------------------------------------
void CTextTable::resize(int rows, int columns)
{
    if (m_pTextTable)
    {
        m_pTextTable->resize(rows, columns);
    }
}

//--------------------------------------------------------------------------
QTextTableFormat CTextTable::textTableFormat()
{
    if (m_pTextTable)
    {
        return m_pTextTable->format();
    }
    else
        return QTextTableFormat();
}

//--------------------------------------------------------------------------
QTextFrameFormat CTextTable::textFrameFormat()
{
    if (m_pTextTable)
    {
        return m_pTextTable->frameFormat();
    }
    else
        return QTextFrameFormat();
}

//--------------------------------------------------------------------------
QBrush CTextTable::background()
{
    if (m_pTextTable)
    {
        QTextTableFormat	tableFormat = m_pTextTable->format();
        return tableFormat.background();
    }
    else
    {
        return QBrush();
    }
}

//--------------------------------------------------------------------------
void CTextTable::setColumnConstraints(QVector<QTextLength> &columnConstraints)
{
    if (m_pTextTable)
    {
        QTextTableFormat    ttFormat = m_pTextTable->format();
        ttFormat.setColumnWidthConstraints(columnConstraints);

        m_pTextTable->setFormat(ttFormat);
    }
}

//--------------------------------------------------------------------------
void CTextTable::setBackground(QBrush brush)
{
    if (m_pTextTable)
    {
        QTextTableFormat    ttFormat = m_pTextTable->format();
        ttFormat.setBackground(brush);
        m_pTextTable->setFormat(ttFormat);
    }
}

//--------------------------------------------------------------------------
int CTextTable::currentTableRow(QTextCursor cursor)
{
    QTextTable      *pTable = cursor.currentTable();
    int             row = -1;

    if (pTable != NULL)
    {
        QTextTableCell curCell = pTable->cellAt(cursor);
        row = curCell.row();
    }
    return row;
}

//--------------------------------------------------------------------------
int CTextTable::currentTableColumn(QTextCursor cursor)
{
    QTextTable      *pTable = cursor.currentTable();
    int             column = -1;

    if (pTable != NULL)
    {
        QTextTableCell curCell = pTable->cellAt(cursor);
        column = curCell.column();
    }
    return column;
}

//--------------------------------------------------------------------------
void CTextTable::tableToText(QTextCursor &cursorInTable)
{
    QTextTable          *pTable = cursorInTable.currentTable();

    if (pTable != NULL)
    {
        int             numRows = pTable->rows();
        int             numColumns = pTable->columns();
        QStringList     lines;

        for (int row = 0; row < numRows; row++)
        {
            QStringList     rowItems;

            for (int col = 0; col < numColumns; col++)
            {
                rowItems << GetTableCellText(pTable, row, col);
            }

            lines << rowItems.join(" ");
        }

        QString combinedText = lines.join("\n");
        combinedText.append("\n");      // Add an additional line break, to keep the table text
                                        // separated from the line below it.

        // Remove the table
        QTextCursor tableStartCursor = pTable->cellAt(0, 0).firstCursorPosition();
        QTextCursor tableEndCursor = pTable->cellAt(numRows - 1, numColumns-1).lastCursorPosition();

        // To remove the entire table, and not just its text, it is necessary to go up a line
        // to start the selection.
        tableStartCursor.movePosition(QTextCursor::Up);

        tableStartCursor.setPosition(tableEndCursor.position(), QTextCursor::KeepAnchor);

        // Similarly, we must move one line below the bottom row of the table.
        tableStartCursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);

        // Now we can delete
        tableStartCursor.removeSelectedText();

        cursorInTable.insertText(combinedText);
    }
}

//--------------------------------------------------------------------------
void CTextTable::copyCell(QTextTable *pTable, int sourceRow, int sourceColumn, int destRow, int destColumn)
{
    QTextTableCell  sourceCell = pTable->cellAt(sourceRow, sourceColumn);
    QTextTableCell  destCell = pTable->cellAt(destRow, destColumn);

    // Select the contents of the source cell
    QTextCursor     sourceCellStart = sourceCell.firstCursorPosition();
    QTextCursor     sourceCellEnd = sourceCell.lastCursorPosition();

    sourceCellStart.setPosition(sourceCellEnd.position(), QTextCursor::KeepAnchor);

    // The source cell should now be selected
    QTextDocumentFragment   textFragment = sourceCellStart.selection();

    // Copy to destination
    QTextCursor     destCellStart = destCell.firstCursorPosition();
    destCellStart.insertFragment(textFragment);
}

//--------------------------------------------------------------------------
void CTextTable::copyRow(QTextCursor cursor, int sourceRow)
{
    QTextTable          *pTable = cursor.currentTable();

    if (pTable != NULL)
    {
        QTextTableCell  curCell = pTable->cellAt(cursor);
        int             destRow = curCell.row();

        // Copy the contents of each column from sourceRow to this row
        int     numColumns = pTable->columns();

        for (int curColumn = 0; curColumn < numColumns; curColumn++)
        {
            copyCell(pTable, sourceRow, curColumn, destRow, curColumn);
        }
    }
}

//--------------------------------------------------------------------------
void CTextTable::copyColumn(QTextCursor cursor, int sourceColumn)
{
    QTextTable          *pTable = cursor.currentTable();

    if (pTable != NULL)
    {
        QTextTableCell  curCell = pTable->cellAt(cursor);
        int             destCol = curCell.column();

        // Copy the contents of each row from sourceColumn to this row
        int     numRows = pTable->rows();

        for (int curRow = 0; curRow < numRows; curRow++)
        {
            copyCell(pTable, curRow, sourceColumn, curRow, destCol);
        }
    }
}

//--------------------------------------------------------------------------
void CTextTable::deleteRowAtCursor(QTextCursor cursor)
{
    QTextTable          *pTable = cursor.currentTable();

    if (pTable != NULL)
    {
        QTextTableCell curCell = pTable->cellAt(cursor);

        pTable->removeRows(curCell.row(), 1);
    }
}

//--------------------------------------------------------------------------
void CTextTable::deleteColumnAtCursor(QTextCursor cursor)
{
    QTextTable          *pTable = cursor.currentTable();

    if (pTable != NULL)
    {
        QTextTableCell curCell = pTable->cellAt(cursor);

        pTable->removeColumns(curCell.column(), 1);
    }
}

//--------------------------------------------------------------------------
void CTextTable::insertRow(QTextCursor cursor, bool bAbove)
{
    QTextTable          *pTable = cursor.currentTable();

    if (pTable != NULL)
    {
        QTextTableCell curCell = pTable->cellAt(cursor);

        if (bAbove)
        {
            pTable->insertRows(curCell.row(), 1);
        }
        else
        {
            if (curCell.row() == pTable->rows() - 1)
            {
                // This is the last row.  Use appendRows to add a row at the end.
                pTable->appendRows(1);
            }
            else
            {
                pTable->insertRows(curCell.row() + 1, 1);
            }
        }
    }
}

void CTextTable::insertColumn(QTextCursor cursor, bool bLeft)
{
    QTextTable          *pTable = cursor.currentTable();

    if (pTable != NULL)
    {
        QTextTableCell curCell = pTable->cellAt(cursor);

        if (bLeft)
        {
            QTextTableCell curCell = pTable->cellAt(cursor);

            pTable->insertColumns(curCell.column(), 1);
        }
        else
        {
            if (curCell.column() == pTable->columns() - 1)
            {
                // This is the last column.  Use appendColumns to add a column at the end.
                pTable->appendColumns(1);
            }
            else
            {
                pTable->insertColumns(curCell.column() + 1, 1);
            }
        }
    }
}
