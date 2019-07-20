#ifndef CCUSTOMTEXTEDIT_H
#define CCUSTOMTEXTEDIT_H

#include "PageData.h"
#include <QTextEdit>
#include <QSignalMapper>

class QMimeData;
class AbstractPluginServices;
class CStyleManager;


class CCustomTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	CCustomTextEdit(QWidget *parent);
	~CCustomTextEdit();

    /**
     * Sets the pluginServices pointer.
     * @param pServices
     */
    void setPluginServicesPointer(AbstractPluginServices *pServices);

    /**
     * Sets the CStyleManager pointer.
     * @param styleManager
     */
    void setStyleManagerPointer(CStyleManager *styleManager)    { this->styleManager = styleManager; }

	/*
	 *	Inserts a link to a notebook page.
	 */
	void InsertPageLink( int pageId );

	/*
	 *	Inserts a link to a web page.
	 */
    void InsertWebLink(const QString& urlStr, const QString& descriptionStr, bool withBrackets);

    /**
     * @brief Shifts the selection left by one tab indent.
     */
    void ReduceSelectionIndent();

    /**
     * @brief Shifts the selection right by one tab indent.
     */
    void IncreaseSelectionIndent();

protected:
	QString GetWordAtCursor(QTextCursor& theCursor);
	QString GetBlockAtCursor(QTextCursor& theCursor);
    ENTITY_ID GetNotebookLinkPage(const QString& linkStr);
	bool PointOverLink(const QPoint& pt, QString& linkStr);
    void PopulateStyleSubmenu(QMenu *menu);

	// Overrides
	virtual void mousePressEvent(QMouseEvent *pEvent);
	virtual void mouseMoveEvent(QMouseEvent *pEvent);
    virtual void keyPressEvent(QKeyEvent *pEvent);
    virtual void contextMenuEvent(QContextMenuEvent *pEvent);
	virtual void insertFromMimeData(const QMimeData *source);

    bool CursorInList();
    bool CursorInSelection();

    QTextListFormat::Style NextBullet(QTextListFormat::Style currentBullet);
    QTextListFormat::Style PrevBullet(QTextListFormat::Style currentBullet);

    /**
     * @brief Returns a list block with the same indent as the current list block.
     *        Scanning stops if either the start of the document or a non-list block
     *          is reached.
     * @param block
     * @return A list block with the same indent as the current list block, or an invalid
     *          block if none could be found.
     */
    QTextBlock getPreviousMatchingListBlock(QTextBlock &block);

    void GoToNextCell();
    void GoToPreviousCell();

    void CopyTableCell(QTextTable *pTable, int sourceRow, int sourceColumn, int destRow, int destColumn);

signals:
	// Signal sent when user clicks a link to another page
    void CTE_GotoPage(ENTITY_ID pageId);

	// Signal sent when the edit control needs to send a message (it
	// is mainly used when the user moves the cursor over a link).
	void CTE_Message(QString msg);

    void CTE_IndentList();
    void CTE_OutdentList();

    void CTE_TableFormat();

private slots:
    void OnInsertPageLinkDlg();
	void OnInsertWebLink();
    void OnUrlifySelection();
    void OnInsertImageFromFile();
	void OnCursorPositionChanged();

    void OnMergeListWithPrevious();

    void OnInsertTableRowAbove();
    void OnInsertTableRowBelow();
    void OnCopyTableRow();
    void OnPasteTableRow();

    void OnInsertTableColumnLeft();
    void OnInsertTableColumnRight();
    void OnCopyTableColumn();
    void OnPasteTableColumn();

    void OnDeleteTableRow();
    void OnDeleteTableColumn();

public slots:
    void OnStyleTriggered(int styleId);
    void OnConvertSelectionToTable();
    void OnConvertTableToText();

private:
    AbstractPluginServices  *pluginServices;
    CStyleManager           *styleManager;

	bool				m_bCursorOverLink;

    QSignalMapper		m_styleItemMapper;

    int                 m_copiedRow;        // Row to be copied
    int                 m_copiedColumn;     // Column to be copied
};

#endif // CCUSTOMTEXTEDIT_H
