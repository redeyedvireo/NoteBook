#include "PageData.h"
#include "PluginServices.h"
#include "CCustomTextEdit.h"
#include "CTextTable.h"
#include "CTextImage.h"
#include "ChoosePageToLinkDlg.h"
#include "TextTableDefs.h"
#include "AddWebLinkDlg.h"
#include "StyleManager.h"
#include <QMimeData>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QTextList>
#include <QDebug>

// Defines for in-page links
#define WEBURLTAG	"http://"
#define WEBURLTAGS	"https://"
#define NOTEBOOKTAG	"NB://"


CCustomTextEdit::CCustomTextEdit(QWidget *parent)
	: QTextEdit(parent)
{
    pluginServices = nullptr;
    styleManager = nullptr;
	m_bCursorOverLink = false;
	setMouseTracking(true);

    m_copiedRow = -1;
    m_copiedColumn = -1;

    connect(&m_styleItemMapper, SIGNAL(mapped(int)), this, SLOT(OnStyleTriggered(int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(OnCursorPositionChanged()));
}

//--------------------------------------------------------------------------
CCustomTextEdit::~CCustomTextEdit()
{
    pluginServices = nullptr;
}

//--------------------------------------------------------------------------
void CCustomTextEdit::setPluginServicesPointer(AbstractPluginServices *pServices)
{
    pluginServices = pServices;
}

//--------------------------------------------------------------------------
QString CCustomTextEdit::GetWordAtCursor(QTextCursor& theCursor)
{
	int				curPos = theCursor.position();
	int				wordStartPos, wordEndPos;
	QTextDocument	*pTheDoc = document();

	// First check if cursor is at on a blank.  If so, abort
	if (pTheDoc->characterAt(curPos).isSpace())
		return QString();

	// Scan backwards until white space found, to find the beginning of the current word
	QTextCursor		startOfWord = pTheDoc->find(QRegExp("\\s"), curPos, QTextDocument::FindBackward);

	wordStartPos = startOfWord.position();

	// Scan forwards until white space found, to find end of the current word
	QTextCursor		endOfWord = pTheDoc->find(QRegExp("\\s"), wordStartPos, 0);

	wordEndPos = endOfWord.position();

	theCursor.setPosition(wordStartPos);
	theCursor.setPosition(wordEndPos, QTextCursor::KeepAnchor);

	QString		theWord = theCursor.selectedText();
	theCursor.clearSelection();		// This does not delete the text; it just unselects it

	return theWord;
}

//--------------------------------------------------------------------------
QString CCustomTextEdit::GetBlockAtCursor(QTextCursor& theCursor)
{
	QTextBlock		cursorBlock;

	cursorBlock = theCursor.block();

	return cursorBlock.text();
}

//--------------------------------------------------------------------------
ENTITY_ID CCustomTextEdit::GetNotebookLinkPage(const QString& linkStr)
{
    QRegExp     pageIdRx("page=(\\d+)");
    int         pos = 0;
    ENTITY_ID   pageId = kInvalidPageId;

	if ((pos = pageIdRx.indexIn(linkStr, 0)) != -1)
	{
		QString		pageIdStr = pageIdRx.cap(1);

        pageId = pageIdStr.toUInt();
	}

	return pageId;
}

//--------------------------------------------------------------------------
void CCustomTextEdit::mousePressEvent( QMouseEvent *pEvent )
{
	QTextEdit::mousePressEvent(pEvent);

	if (pEvent->button() != Qt::LeftButton)
	{
		return;
	}

	// Determine if a link was clicked
	QPoint		clickPos = pEvent->pos();
	//QTextCursor	cursor = cursorForPosition(clickPos);

	//QString		wordAtCursor = GetWordAtCursor(cursor);
	//QString			wordAtCursor = GetBlockAtCursor(cursor);
	QString			linkAtCursor = anchorAt(clickPos);

	if (! linkAtCursor.isEmpty())
	{
		if (linkAtCursor.startsWith(WEBURLTAG) || linkAtCursor.startsWith(WEBURLTAGS))
		{
			QUrl		possibleUrl(linkAtCursor);

			if (possibleUrl.isValid())
			{
				QDesktopServices::openUrl(possibleUrl);
			}
		}
		else if (linkAtCursor.startsWith(NOTEBOOKTAG))
		{
            ENTITY_ID		pageId = GetNotebookLinkPage(linkAtCursor);

			emit CTE_GotoPage(pageId);
		}
	}
}

//--------------------------------------------------------------------------
bool CCustomTextEdit::PointOverLink(const QPoint& pt, QString& linkStr)
{
	QPoint		clickPos = pt;
	bool		bOverLink = false;

	QString		linkAtCursor = anchorAt(clickPos);

	if (! linkAtCursor.isEmpty())
	{
		if (linkAtCursor.startsWith(WEBURLTAG) || linkAtCursor.startsWith(WEBURLTAGS))
		{
			QUrl		possibleUrl(linkAtCursor);

			if (possibleUrl.isValid())
			{
				linkStr = linkAtCursor;
				bOverLink = true;
			}
		}
		else if (linkAtCursor.startsWith(NOTEBOOKTAG))
		{
			linkStr = linkAtCursor;
			bOverLink = true;
		}
	}

	return bOverLink;
}

//--------------------------------------------------------------------------
void CCustomTextEdit::mouseMoveEvent( QMouseEvent *pEvent )
{
	QTextEdit::mouseMoveEvent(pEvent);

	QString		linkStr;

	if (PointOverLink(pEvent->pos(), linkStr))
	{
		if (! m_bCursorOverLink)
		{
			// Cursor has just moved over a link
			QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
			m_bCursorOverLink = true;

			QString		messageStr;

			// If it's a notebook page link, get the title of the page
			if (linkStr.startsWith(NOTEBOOKTAG))
			{
				int			pageId = GetNotebookLinkPage(linkStr);
                QString		pageTitle = pluginServices->GetPageTitle(pageId);

                messageStr = QString("Page: %1").arg(pageTitle);
			}
			else
			{
				// For web links, just show the web URL
				messageStr = linkStr;
			}

			if (! messageStr.isEmpty())
			{
				emit CTE_Message(messageStr);
			}
		}
	}
	else
	{
		if (m_bCursorOverLink)
		{
			// Cursor was formerly over a link, and now is not.  Remove the override cursor.
			QApplication::restoreOverrideCursor();
			m_bCursorOverLink = false;

			// Blank out the message
			emit CTE_Message("");
		}
	}
}

//--------------------------------------------------------------------------
void CCustomTextEdit::keyPressEvent(QKeyEvent *pEvent)
{
    if (pEvent->key() == Qt::Key_Tab)
    {
        if (CursorInList())
        {
            IncreaseSelectionIndent();
            return;         // Don't call the base class; it will do unwanted operations
        }
        else if (CTextTable::CursorInTable(textCursor()))
        {
            GoToNextCell();
            pEvent->accept();
            return;         // Don't call the base class, because it will insert a tab character
        }
    }
    else if (pEvent->key() == Qt::Key_Backtab)
    {
        if (CursorInList())
        {
            ReduceSelectionIndent();
            return;         // Don't call the base class; it will do unwanted operations
        }
        else if (CTextTable::CursorInTable(textCursor()))
        {
            GoToPreviousCell();
            pEvent->accept();
            return;         // Don't call the base class; it will do unwanted operations
        }
    }

    QTextEdit::keyPressEvent(pEvent);
}

//--------------------------------------------------------------------------
void CCustomTextEdit::contextMenuEvent( QContextMenuEvent *pEvent )
{
    QMenu   *pRowMenu = nullptr;
    QMenu   *pColumnMenu = nullptr;
    QMenu	*pStyleMenu = nullptr;
	QMenu	*menu = createStandardContextMenu();

    menu->addSeparator();

    if (CursorInSelection() && styleManager->NumStyles() > 0)
    {
        // Add "Apply Style" menu
        pStyleMenu = new (std::nothrow) QMenu(tr("Apply Style"));

        if (pStyleMenu != nullptr)
        {
            PopulateStyleSubmenu(pStyleMenu);
            menu->addMenu(pStyleMenu);
            menu->addSeparator();
        }
    }

    if (CTextTable::CursorInTable(textCursor()))
    {
        pRowMenu = new (std::nothrow) QMenu("Row");

        if (pRowMenu != nullptr)
        {
            pRowMenu->addAction(tr("Insert Row Above"), this, SLOT(OnInsertTableRowAbove()));
            pRowMenu->addAction(tr("Insert Row Below"), this, SLOT(OnInsertTableRowBelow()));
            pRowMenu->addAction(tr("Copy Row"), this, SLOT(OnCopyTableRow()));

            if (m_copiedRow != -1)
            {
                pRowMenu->addAction(tr("Paste Row"), this, SLOT(OnPasteTableRow()));
            }

            pRowMenu->addAction(tr("Delete Row"), this, SLOT(OnDeleteTableRow()));
            menu->addMenu(pRowMenu);
        }

        pColumnMenu = new (std::nothrow) QMenu("Column");

        if (pColumnMenu != nullptr)
        {
            pColumnMenu->addAction(tr("Insert Column to the Left"), this, SLOT(OnInsertTableColumnLeft()));
            pColumnMenu->addAction(tr("Insert Column to the Right"), this, SLOT(OnInsertTableColumnRight()));
            pColumnMenu->addAction(tr("Copy Column"), this, SLOT(OnCopyTableColumn()));

            if (m_copiedColumn != -1)
            {
                pColumnMenu->addAction(tr("Paste Column"), this, SLOT(OnPasteTableColumn()));
            }

            pColumnMenu->addAction(tr("Delete Column"), this, SLOT(OnDeleteTableColumn()));
            menu->addMenu(pColumnMenu);
        }

        menu->addAction(tr("Convert Table to Text"), this, SLOT(OnConvertTableToText()));
        menu->addAction(tr("Format Table..."), this, SIGNAL(CTE_TableFormat()));
        menu->addSeparator();
    }

    menu->addAction(tr("Insert Link to Page..."), this, SLOT(OnInsertPageLinkDlg()));

	// Add menu item for inserting a web page link
	menu->addAction(tr("Insert web link..."), this, SLOT(OnInsertWebLink()));

    // Add menu item for converting selected text to a table
    if (CursorInSelection())
    {
        menu->addSeparator();
        menu->addAction(tr("Convert Selection to Table"), this, SLOT(OnConvertSelectionToTable()));
        menu->addAction(tr("URL-ify selection"), this, SLOT(OnUrlifySelection()));
    }

    // Add menu item to move a list item into a previous list, if there is one
    if (CursorInList())
    {
        QTextBlock  curBlock = textCursor().block();
        QTextBlock  prevBlock = curBlock.previous();
        QTextList   *pPrevList = prevBlock.textList();

        if (pPrevList != nullptr)
        {
            // The previous block is a list; add menu item to merge this list
            // into the previous one.
            menu->addAction(tr("Merge this list with previous list"), this, SLOT(OnMergeListWithPrevious()));
        }
    }

    menu->addSeparator();
    menu->addAction(tr("Insert Image from File..."), this, SLOT(OnInsertImageFromFile()));

	menu->exec(pEvent->globalPos());

    if (pStyleMenu != nullptr)
        delete pStyleMenu;

    if (pRowMenu != nullptr)
        delete pRowMenu;

    if (pColumnMenu != nullptr)
        delete pColumnMenu;

	delete menu;
}

//--------------------------------------------------------------------------
void CCustomTextEdit::PopulateStyleSubmenu(QMenu *menu)
{
    QList<int>      styleIds = styleManager->GetStyleIds();

    foreach (int styleId, styleIds)
    {
        StyleDef    styleDef = styleManager->GetStyle(styleId);

        QAction		*action = menu->addAction(styleDef.strName);

        m_styleItemMapper.setMapping(action, styleId);

        connect(action, SIGNAL(triggered()), &m_styleItemMapper, SLOT(map()));
    }
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnStyleTriggered(int styleId)
{
    // Apply the selected style
    styleManager->ApplyStyle(this, styleId);
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnInsertPageLinkDlg()
{
    ChoosePageToLinkDlg     dlg(pluginServices, this);

    dlg.exec();

    if (dlg.result() == QDialog::Accepted)
    {
        // Insert page link
        InsertPageLink(dlg.getSelectedPage());
    }
}

//--------------------------------------------------------------------------
void CCustomTextEdit::InsertPageLink( int pageId )
{
    QString		pageTitle = pluginServices->GetPageTitle(pageId);

    if (!pageTitle.isEmpty())
	{
		// Note the terminating <br>.  This is being added as a way of providing separation
		// from the link tag.  Without this, if the user were to move the cursor past the link
		// and begin typing, the text he/she types would end up being part of the link.  By
		// adding the <br>, the user is able to type beyond the link.  More research is needed
		// to allow the OnCursorPositionChanged() function to be able to identify that the cursor
		// is within a link, and to move it out of the link.
		QString		pageLinkStr = QString("<a href=\"NB://page=%1\">[%2]</a><br>")
			.arg(pageId)
			.arg(pageTitle);

		insertHtml(pageLinkStr);
	}
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnInsertWebLink()
{
	CAddWebLinkDlg	dlg;

	dlg.exec();

	if (dlg.result() == QDialog::Accepted)
	{
		QString		urlStr, descriptionStr;

		dlg.GetLink(urlStr, descriptionStr);

        InsertWebLink(urlStr, descriptionStr, true);
    }
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnUrlifySelection()
{
    QTextCursor         selectionCursor(textCursor());

    QString             selectedText = textCursor().selectedText();

    selectionCursor.removeSelectedText();
    InsertWebLink(selectedText, selectedText, false);
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnInsertImageFromFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Select Image File"), QString(), tr("Images (*.png *.jpg *.bmp)"));

    if (!filePath.isEmpty())
    {
        CTextImage::insertImageIntoDocument(pluginServices, document(), textCursor(), filePath);
    }
}

//--------------------------------------------------------------------------
void CCustomTextEdit::InsertWebLink( const QString& urlStr, const QString& descriptionStr, bool withBrackets )
{
	QString		webUrlStr(urlStr);

	if (! (urlStr.startsWith(WEBURLTAG) || urlStr.startsWith(WEBURLTAGS)) )
	{
		// Add "http://" header
		webUrlStr.prepend(WEBURLTAG);
	}

	// Note the terminating <br>.  This is being added as a way of providing separation
	// from the link tag.  Without this, if the user were to move the cursor past the link
	// and begin typing, the text he/she types would end up being part of the link.  By
	// adding the <br>, the user is able to type beyond the link.  More research is needed
	// to allow the OnCursorPositionChanged() function to be able to identify that the cursor
	// is within a link, and to move it out of the link.
    QString webLinkStr;

    if (withBrackets)
    {
        webLinkStr = QString("<a href=\"%1\">[%2]</a><br>")
                        .arg(webUrlStr)
                        .arg(descriptionStr);
    }
    else
    {
        webLinkStr = QString("<a href=\"%1\">%2</a><br>")
                        .arg(webUrlStr)
                        .arg(descriptionStr);
    }

	insertHtml(webLinkStr);
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnCursorPositionChanged()
{
	return;		// For now, this function is just experimental.  See the below TODO comments.

	QTextCursor		tempCursor = textCursor();
	//QString			blockContents = GetBlockAtCursor(tempCursor);
	//qDebug() << "CCustomTextEdit::OnCursorPositionChanged(): " << blockContents;

	// TODO: Would like to determine if the cursor is now in the middle of text
	//		that contains an HTML link.  The function QTextEdit::anchorAt() can
	//		be used for this, but it takes a point, rather than a text position.
	//		Thus far, I have not found a way to determine if the cursor is in an
	//		HTML link.
	//		This is needed so that if the user adds an HTML link, and then adds text
	//		to the right of it (or hits return), the text will not be part of the HTML
	//		link.  This is turning out to be an extremely difficult thing to do in Qt.

	// DEBUG
#if 0
	int		curPos = tempCursor.position();
	int		numChars = document()->characterCount();
	int		lastChar = numChars - 1;
	bool	bFoundNonAnchorBlock = false;

	while (! anchorAt(curPos).isEmpty())
	{
		if (curPos == lastChar)
		{
			break;
		}
		else
		{
			// Go to next position
			curPos++;
		}
	}

	if (bFoundNonAnchorBlock)
	{
		qDebug() << "CCustomTextEdit::OnCursorPositionChanged(): Found non-anchor block at pos: " << curPos;
	}
	else
	{
		qDebug() << "CCustomTextEdit::OnCursorPositionChanged(): Could not find non-anchor block.";
	}
#endif
	// END DEBUG
}

//--------------------------------------------------------------------------
void CCustomTextEdit::insertFromMimeData( const QMimeData *source )
{
	if (source->hasHtml())
	{
		// The text has formatting.  Ask user if it should be pasted with
		// the formatting intact, or just as plain text.
		// On Linux, QMessageBox::question() seems to corrupt *source, so on Linux we cannot
		// ask the user whether he wants to paste with formatting.  We'll just go ahead and
		// paste as plain text.
		bool	bRemoveFormatting = true;

#ifdef _WIN32
		QMessageBox::StandardButton		result;

		result = QMessageBox::question(this, tr("Paste"),
										tr("The text contains formatting.  Paste text with formatting intact?"),
										QMessageBox::Yes | QMessageBox::No);

		bRemoveFormatting = (result == QMessageBox::No);
#endif

		if (bRemoveFormatting)
		{
			// Convert it to a plain text object
            if (source != nullptr)
			{
				QString		plainText = source->text();
				QMimeData	*pNewMimeData;

				pNewMimeData = new (std::nothrow) QMimeData();

				if (pNewMimeData)
				{
					pNewMimeData->setText(plainText);
					QTextEdit::insertFromMimeData(pNewMimeData);

					delete pNewMimeData;
					return;
				}
			}
		}
	}

	// Call base class
	QTextEdit::insertFromMimeData(source);
}

//--------------------------------------------------------------------------
bool CCustomTextEdit::CursorInList()
{
    QTextCursor			selectionCursor(textCursor());
    QTextList           *pList = selectionCursor.currentList();

    return (pList != nullptr);
}

//--------------------------------------------------------------------------
bool CCustomTextEdit::CursorInSelection()
{
    QTextCursor			selectionCursor(textCursor());
    return selectionCursor.hasSelection();
}

//--------------------------------------------------------------------------
QTextListFormat::Style CCustomTextEdit::NextBullet(QTextListFormat::Style currentBullet)
{
    int     nValue = (int) currentBullet;
    int     nextValue = nValue - 1;         // Next bullet in sequence

    if (nValue >= QTextListFormat::ListSquare && nValue <= QTextListFormat::ListDisc)
    {
        // Non-numeric bullet
        if (currentBullet == QTextListFormat::ListSquare)
        {
            nextValue = (int) QTextListFormat::ListDisc;
        }
    }
    else
    {
        // Numeric bullet
        if (currentBullet == QTextListFormat::ListUpperRoman)
        {
            nextValue = (int) QTextListFormat::ListDecimal;
        }
    }

    return (QTextListFormat::Style) nextValue;
}

//--------------------------------------------------------------------------
QTextListFormat::Style CCustomTextEdit::PrevBullet(QTextListFormat::Style currentBullet)
{
    int     nValue = (int) currentBullet;
    int     prevValue = nValue + 1;         // Previous bullet in sequence

    if (nValue >= QTextListFormat::ListSquare && nValue <= QTextListFormat::ListDisc)
    {
        // Non-numeric bullet
        if (currentBullet == QTextListFormat::ListDisc)
        {
            prevValue = (int) QTextListFormat::ListSquare;
        }
    }
    else
    {
        // Numeric bullet
        if (currentBullet == QTextListFormat::ListDecimal)
        {
            prevValue = (int) QTextListFormat::ListUpperRoman;
        }
    }

    return (QTextListFormat::Style) prevValue;
}

//--------------------------------------------------------------------------
QTextBlock CCustomTextEdit::getPreviousMatchingListBlock(QTextBlock& block)
{
    QTextCursor		selectionCursor(textCursor());
    QTextList       *pList = selectionCursor.currentList();
    QTextBlock      returnBlock = QTextBlock();

    if (pList != nullptr)
    {
        QTextListFormat     listFormat = pList->format();
        int                 initialIndent = listFormat.indent();

        QTextBlock  curBlock = block.previous();
        QTextList   *pCurList = curBlock.textList();

        while (pCurList != nullptr)
        {
            QTextListFormat     curListFormat = pCurList->format();

            if (curListFormat.indent() == initialIndent)
            {
                // Found one
                returnBlock = curBlock;
                break;
            }

            // Go to previous block
            curBlock = curBlock.previous();
            pCurList = curBlock.textList();
        }
    }

    return returnBlock;
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnMergeListWithPrevious()
{
    QTextBlock  curBlock = textCursor().block();
    QTextBlock  prevBlock = getPreviousMatchingListBlock(curBlock);

    if (prevBlock.isValid())
    {
        QTextList   *pPrevList = prevBlock.textList();

        if (pPrevList != nullptr)
        {
            pPrevList->add(curBlock);
        }
    }
}

//--------------------------------------------------------------------------
void CCustomTextEdit::ReduceSelectionIndent()
{
    QTextCursor			selectionCursor(textCursor());
    QTextList           *pList = selectionCursor.currentList();

    if (pList != nullptr)
    {
        QTextListFormat     listFormat = pList->format();
        int                 curIndent = listFormat.indent();

        if (curIndent > 0)
        {
            if (selectionCursor.hasSelection())
            {
                // Indent the lines that are selected
                listFormat.setIndent(listFormat.indent() - 1);
                listFormat.setStyle(PrevBullet(listFormat.style()));
                selectionCursor.createList(listFormat);
            }
            else
            {
                // Reduce indent of the current line
                if (pList->count() == 1)
                {
                    // There is only one item in the list, so can just outdent it
                    listFormat.setIndent(curIndent - 1);
                    listFormat.setStyle(PrevBullet(listFormat.style()));
                    pList->setFormat(listFormat);

                    // In case this list is adjacent to a previous list, check if it can
                    // be merged with the previous list.
                    QTextBlock  newBlock = pList->item(0);   // This will be the first item
                    QTextBlock  prevMatchingBlock = getPreviousMatchingListBlock(newBlock);

                    if (prevMatchingBlock.isValid())
                    {
                        QTextList   *pPrevList = prevMatchingBlock.textList();
                        pPrevList->add(newBlock);
                    }

                    setTextCursor(selectionCursor);
                }
                else
                {
                    // Reduce indent the line containing the cursor
                    listFormat.setIndent(listFormat.indent() - 1);
                    listFormat.setStyle(PrevBullet(listFormat.style()));

                    // Must create a new list, or else the entire sublist will
                    // be outdented.
                    QTextList   *pNewList = selectionCursor.createList(listFormat);
                    QTextBlock  newBlock = pNewList->item(0);   // This will be the first item
                    QTextBlock  prevMatchingBlock = getPreviousMatchingListBlock(newBlock);

                    if (prevMatchingBlock.isValid())
                    {
                        QTextList   *pPrevList = prevMatchingBlock.textList();
                        pPrevList->add(newBlock);
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------------------
void CCustomTextEdit::IncreaseSelectionIndent()
{
    QTextCursor			selectionCursor(textCursor());
    QTextList           *pList = selectionCursor.currentList();

    if (pList != nullptr)
    {
        QTextListFormat     listFormat = pList->format();

        if (selectionCursor.hasSelection())
        {
            // Indent the lines that are selected
            listFormat.setIndent(listFormat.indent() + 1);
            listFormat.setStyle(NextBullet(listFormat.style()));
            selectionCursor.createList(listFormat);
        }
        else
        {
            // Indent the current line
            if (pList->count() == 1)
            {
                // There is only one item in the list, so can just indent it
                listFormat.setIndent(listFormat.indent() + 1);
                listFormat.setStyle(NextBullet(listFormat.style()));
                pList->setFormat(listFormat);
            }
            else
            {
                // Indent the line containing the cursor
                listFormat.setIndent(listFormat.indent() + 1);
                listFormat.setStyle(NextBullet(listFormat.style()));
                selectionCursor.createList(listFormat);
            }
        }
    }
}

//--------------------------------------------------------------------------
void CCustomTextEdit::GoToNextCell()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::NextCell);
    setTextCursor(cursor);
}

//--------------------------------------------------------------------------
void CCustomTextEdit::GoToPreviousCell()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::PreviousCell);
    setTextCursor(cursor);
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnInsertTableRowAbove()
{
    CTextTable::insertRow(textCursor(), true);
    m_copiedRow = -1;        // Inserting a new row will throw off the row numbering
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnInsertTableRowBelow()
{
    CTextTable::insertRow(textCursor(), false);
    m_copiedRow = -1;        // Inserting a new row will throw off the row numbering
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnCopyTableRow()
{
    m_copiedRow = CTextTable::currentTableRow(textCursor());
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnPasteTableRow()
{
    CTextTable::copyRow(textCursor(), m_copiedRow);
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnInsertTableColumnLeft()
{
    CTextTable::insertColumn(textCursor(), true);
    m_copiedColumn = -1;        // Inserting a new column will throw off the column numbering
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnInsertTableColumnRight()
{
    CTextTable::insertColumn(textCursor(), false);
    m_copiedColumn = -1;        // Inserting a new column will throw off the column numbering
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnCopyTableColumn()
{
    m_copiedColumn = CTextTable::currentTableColumn(textCursor());
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnPasteTableColumn()
{
    CTextTable::copyColumn(textCursor(), m_copiedColumn);
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnDeleteTableRow()
{
    CTextTable::deleteRowAtCursor(textCursor());
    m_copiedRow = -1;        // Deleting a row will throw off the row numbering
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnDeleteTableColumn()
{
    CTextTable::deleteColumnAtCursor(textCursor());
    m_copiedColumn = -1;        // Deleting a column will throw off the column numbering
}


//--------------------------------------------------------------------------
void CCustomTextEdit::OnConvertSelectionToTable()
{
    QTextCursor         selectionCursor(textCursor());
    CTextTable::selectionToTable(selectionCursor);
}

//--------------------------------------------------------------------------
void CCustomTextEdit::OnConvertTableToText()
{
    QTextCursor			selectionCursor(textCursor());
    CTextTable::tableToText(selectionCursor);
}
