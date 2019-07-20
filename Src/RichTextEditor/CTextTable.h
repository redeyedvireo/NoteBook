#ifndef CTEXTTABLE_H
#define CTEXTTABLE_H

#include <QTextTable>


class CTextTable
{
    // Constructors are private.  Factory methods must be used to create new objects.
private:
    CTextTable();

public:
    ~CTextTable();

    /**
     * @brief createAtCursor
     * @param cursor
     * @param rows
     * @param columns
     * @return Pointer to the newly created CTextTable.
     *
     * Creates a new text table and returns a pointer to it.
     */
    static CTextTable* createAtCursor(QTextCursor& cursor, int rows, int columns);

    /**
     * @brief tableFromCursor
     * @param cursor
     *
     * Returns a CTextTable object corresponding to the QTextTable located at the
     * cursor position.  Returns NULL if the cursor is not within a QTextTable.
     *
     * @return
     */
    static CTextTable *fromCursor(QTextCursor& cursor);

    /**
     * @brief createFromSelection
     * @param selectionCursor
     *
     * Converts a selection to a QTextTable.
     * @return
     */
    static void selectionToTable(QTextCursor& selectionCursor);

    /**
     * @brief CursorInTable
     * @param cursor
     *
     * Indicates whether the given cursor is currently within a table.
     *
     * @return True if the cursor is within a table, false otherwise.
     */
    static bool CursorInTable(QTextCursor cursor);

    /**
     * @brief currentTableRow
     * @param cursor
     * @return
     * Returns the row of the table that the cursor is currently occupying.
     */
    static int currentTableRow(QTextCursor cursor);

    /**
     * @brief currentTableColumn
     * @param cursor
     * @return
     * Returns the column of the table that the cursor is currently occupying.
     */
    static int currentTableColumn(QTextCursor cursor);

    /**
     * @brief ConvertTableToText
     * @param CursorInTable
     *
     * Converts a table to text.
     */
    static void tableToText(QTextCursor &cursorInTable);

    /**
     * @brief copyCell
     * @param pTable
     * @param sourceRow
     * @param sourceColumn
     * @param destRow
     * @param destColumn
     *
     * Copies a cell.
     */
    static void copyCell(QTextTable *pTable, int sourceRow, int sourceColumn, int destRow, int destColumn);

    /**
     * @brief copyRow
     * @param sourceRow
     * @param destRow
     *
     * Copies source row to the row containing the cursor.  Table's size remains the same.
     */
    static void copyRow(QTextCursor cursor, int sourceRow);

    /**
     * @brief copyColumn
     * @param cursor
     * @param sourceColumn
     *
     * Copies source column to the row containing the cursor.  Table's size remains the same.
     */
    static void copyColumn(QTextCursor cursor, int sourceColumn);

    /**
     * @brief deleteRowAtCursor
     * @param cursor
     *
     * Deletes the row in which the cursor currently resides.
     */
    static void deleteRowAtCursor(QTextCursor cursor);

    /**
     * @brief deleteColumnAtCursor
     * @param cursor
     *
     * Deletes the column in which the cursor currently resides.
     */
    static void deleteColumnAtCursor(QTextCursor cursor);

    int columns();
    int rows();

    void resize(int rows, int columns);

    QTextTableFormat    textTableFormat();
    QTextFrameFormat    textFrameFormat();
    QBrush              background();

    void setColumnConstraints(QVector<QTextLength>& columnConstraints);

    void setBackground(QBrush brush);

    /**
     * @brief insertRow
     * @param cursor
     * @param bAbove If true, the new row will be inserted above the row containing
     *        the cursor; otherwise the new row will be inserted below.
     *
     * Inserts a row in the table containing the cursor.
     */
    static void insertRow(QTextCursor cursor, bool bAbove);

    /**
     * @brief insertColumn
     * @param cursor
     * @param bLeft If true, the new column will be inserted to the left of the row
     *        containing the cursor; otherwise the new row will be inserted to the right.
     *
     * Inserts a column in the table containing the cursor.
     */
    static void insertColumn(QTextCursor cursor, bool bLeft);

private:
    /**
     * @brief GetTableCellText
     * @param pTable
     * @param row
     * @param col
     * @return
     *
     * Returns the text in the given cell of the given table.
     */
    static QString GetTableCellText(QTextTable *pTable, int row, int col);

private:
    QTextTable          *m_pTextTable;
};

#endif // CTEXTTABLE_H
