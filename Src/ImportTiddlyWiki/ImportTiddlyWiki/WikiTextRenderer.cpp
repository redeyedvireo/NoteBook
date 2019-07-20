#include "stdafx.h"
#include "WikiTextRenderer.h"
#include <QTextCursor>
#include <QTextCharFormat>
#include <QVector>
#include <QTextLength>
#include <QTextTableFormat>
#include <QRegExp>


// This is the maximum header size, indicated with one exclamation mark.
#define kMaxHeaderSize 18


//--------------------------------------------------------------------------
CWikiTextRenderer::CWikiTextRenderer(void)
{
	m_pDoc = NULL;
}

//--------------------------------------------------------------------------
CWikiTextRenderer::~CWikiTextRenderer(void)
{
	DeleteDoc();
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::DeleteDoc()
{
	if (m_pDoc != NULL)
	{
		delete m_pDoc;
		m_pDoc = NULL;
	}
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::SetWikiText( const QString& wikiText, int defaultFontSize )
{
	m_wikiText = wikiText;

	// Render the text
	Render(defaultFontSize);
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::Render(int fontSize)
{
	if (m_pDoc != NULL)
	{
		DeleteDoc();
	}

	m_pDoc = new (std::nothrow) QTextDocument;

	if (m_pDoc != NULL)
	{
		m_pDoc->setPlainText(m_wikiText);

		SetDocumentFontSize(fontSize);

		// Perform a few wiki text replacements.  This will not be a complete wiki renderer.

		ReplaceTableElements();
		ReplaceHorizLineElements();		// Must be done before ReplaceStrikethroughDashes(), due to similarity of the markup
		ReplaceMonospaceBlock();		// Must be done before ReplaceCurlyBraceTrio()
		ReplaceCurlyBraceTrio();
		ReplaceSlashItalics();
		ReplaceBoldQuotes();
		ReplaceStrikethroughDashes();
		ReplaceUnderlineMarkup();
		ReplaceColorMarkup();
		ReplaceHighlightMarkup();
		ReplaceHeaders();
		ReplaceBlockQuoteBlock();
		ReplaceListElements();
	}
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::SetDocumentFontSize(int fontSize)
{
	// Select entire document
	QTextCursor			selectionCursor(m_pDoc);
	QTextCharFormat		selectionFormat, tempCharFormat;

	selectionCursor.movePosition(QTextCursor::Start);
	selectionCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

	selectionFormat = selectionCursor.charFormat();

	tempCharFormat.setFontPointSize((qreal) fontSize);
	selectionCursor.mergeCharFormat(tempCharFormat);
}

//--------------------------------------------------------------------------
bool CWikiTextRenderer::FindAndSelectMarkupText(const QString& startMarkupString,
												const QString& endMarkupString,
												QTextCursor& selectionCursor)
{
	bool	bFound = false;

	QTextCursor		startPos, endPos;

	startPos = m_pDoc->find(startMarkupString);

	if (! startPos.isNull())
	{
		endPos = m_pDoc->find(endMarkupString, startPos, 0);

		if (! endPos.isNull())
		{
			// Remove the markup text
			startPos.removeSelectedText();
			endPos.removeSelectedText();

			// Select the text delimited by the markup text
			selectionCursor.setPosition(startPos.position(), QTextCursor::MoveAnchor);
			selectionCursor.setPosition(endPos.position(), QTextCursor::KeepAnchor);

			bFound = true;
		}
	}

	return bFound;
}

//--------------------------------------------------------------------------
bool CWikiTextRenderer::FindAndSelectMarkupText(const QString& markupString, QTextCursor& selectionCursor)
{
	return FindAndSelectMarkupText(markupString, markupString, selectionCursor);
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceCurlyBraceTrio()
{
	CMarkupFinder			markupFinder(m_pDoc, "{{{", "}}}");
	CMonospaceConverter		monospaceConverter;

	CLooper			docLooper(&markupFinder, &monospaceConverter);
	docLooper.TraverseDocument();
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceMonospaceBlock()
{
	CLineFinder					lineFinder(m_pDoc, "{{{", "}}}");
	CMonospaceBlockConverter	monospaceBlockConverter;

	CLooper		docLooper(&lineFinder, &monospaceBlockConverter);
	docLooper.TraverseDocument();
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceBlockQuoteBlock()
{
	CLineFinder				lineFinder(m_pDoc, "<<<", "<<<");
	CBlockQuoteConverter	blockQuoteConverter;

	CLooper		docLooper(&lineFinder, &blockQuoteConverter);
	docLooper.TraverseDocument();
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceSlashItalics()
{
	CMarkupFinder		markupFinder(m_pDoc, "//");
	CItalicConverter	italicConverter;

	CLooper			docLooper(&markupFinder, &italicConverter);
	docLooper.TraverseDocument();
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceBoldQuotes()
{
	CMarkupFinder		markupFinder(m_pDoc, "\'\'");
	CBoldConverter		boldConverter;

	CLooper			docLooper(&markupFinder, &boldConverter);
	docLooper.TraverseDocument();
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceStrikethroughDashes()
{
	// This must be done after the call to ReplaceHorizLineElements(), due to the similarity of the markup text.
	// Unfortunately, using a regular expression for finding the '--' Still doesn't prevent it from finding
	// '----'.  Using a full-fledged syntax parser is probably the only way to get around this problem.
	CMarkupFinder				markupFinder(m_pDoc, "--");
	CStrikethroughConverter		strikethroughConverter;

	CLooper			docLooper(&markupFinder, &strikethroughConverter);
	docLooper.TraverseDocument();
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceUnderlineMarkup()
{
	CMarkupFinder		markupFinder(m_pDoc, "__");
	CUnderlineConverter	underlineConverter;

	CLooper			docLooper(&markupFinder, &underlineConverter);
	docLooper.TraverseDocument();
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceHighlightMarkup()
{
	CMarkupFinder		markupFinder(m_pDoc, "@@");
	CHighlightConverter	highlightConverter;

	CLooper		docLooper(&markupFinder, &highlightConverter);
	docLooper.TraverseDocument();
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceHeaders()
{
	QRegExp				rx("^([!]+)\\s*");
	CLineFinder			lineFinder(m_pDoc, rx);
	CHeaderConverter	headerConverter;

	CLooper		docLooper(&lineFinder, &headerConverter);
	docLooper.TraverseDocument();
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceColorMarkup()
{
	QRegExp					rx("@@color\\(([A-Za-z]+)\\):");
	CMarkupFinder			markupFinder(m_pDoc, rx, "@@");
	CColorMarkupConverter	colorMarkupConverter;

	CLooper		docLooper(&markupFinder, &colorMarkupConverter);
	docLooper.TraverseDocument();
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceListElements()
{
	// Convert '#' and '*' to list elements
	QRegExp		listRx("^([#*]+)");

	QHash<QString, QTextList*>	listHash;		// Holds pointers to lists <format, list pointer>

	QTextCursor			curPos(m_pDoc);

	curPos.movePosition(QTextCursor::Start);

	while (true)
	{
		// Select current line, and retrieve it
		int			curRow;
		QString		strCurLine;
		curPos.movePosition(QTextCursor::StartOfLine);
		curPos.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
		strCurLine = curPos.selectedText();
		curPos.movePosition(QTextCursor::StartOfLine);		// Move cursor to start and deselect the line

		// Does it contain a list format string at the beginning of the line?
		int  foundPos = listRx.indexIn(strCurLine);
		if (foundPos != -1)
		{
			// Found list format string
			QString		listFormatStr = listRx.cap(1);
			int			indentLevel = listFormatStr.length();

			if (listHash.contains(listFormatStr))
			{
				// This list format has been seen before.  Make this line a child of that list.
				for (int i = 0; i < indentLevel; i++)
				{
					curPos.deleteChar();		// Remove the list format string
				}

				listHash.value(listFormatStr)->add(curPos.block());
			}
			else
			{
				// This is a new list format string.  Create a new list format, add the line
				// to that format, and make this new list a sublist of the list found in the
				// previous line.
				QTextListFormat		newListFormat;

				newListFormat.setIndent(indentLevel);

				if (listFormatStr.right(1) == "*")
					newListFormat.setStyle(QTextListFormat::ListDisc);
				else
					newListFormat.setStyle(QTextListFormat::ListDecimal);

				for (int i = 0; i < indentLevel; i++)
				{
					curPos.deleteChar();		// Remove the list format string
				}

				QTextList* pNewList = curPos.createList(newListFormat);
				listHash.insert(listFormatStr, pNewList);
			}
		}
		else
		{
			// No list formatting string found.  Clear the list hash
			listHash.clear();
		}

		// Go to next line
		if (! curPos.movePosition(QTextCursor::Down))
		{
			break;		// At end of document
		}
	}
}

//--------------------------------------------------------------------------
int CWikiTextRenderer::CellWidth(QTextTable* pTable, int row, int column)
{
	int pixelWidth = 0;

	if (pTable != NULL)
	{
		QTextCursor		aCursor;
		QTextTableCell	cell = pTable->cellAt(row, column);
		QTextCursor		beginPos = cell.firstCursorPosition();
		QTextCursor		lastPos = cell.lastCursorPosition();

		// Select the text
		beginPos.select(QTextCursor::LineUnderCursor);

		// Retrieve the text
		QString		selectedText = beginPos.selectedText();

		// Retrieve the font
		QTextCharFormat		cellFormat = cell.format();
		QFont				cellFont = cellFormat.font();
		QFontMetrics		cellFontMetrics(cellFont);

		pixelWidth = cellFontMetrics.width(selectedText);
	}

	return pixelWidth;
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceTableElements()
{
	// Convert lines starting with '|' to table elements
	QTextCursor		curPos(m_pDoc);

	curPos.movePosition(QTextCursor::Start);

	// State machine approach
	// Traverse each line, and look for '|' at the beginning of a line
	QTextTable*	pCurrentTable = NULL;

	while (true)
	{
		// Select current line, and retrieve it
		int			curRow;
		QString		strCurLine;
		curPos.movePosition(QTextCursor::StartOfLine);
		curPos.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
		strCurLine = curPos.selectedText();

		// Does the line begin and end with a '|'?
		if (strCurLine.startsWith("|") && (strCurLine.endsWith("|") || strCurLine.endsWith("|h")))
		{
			// Found table.  Determine number of columns
			int			numColumns;

			// Remove the '|' at the beginning and end of the line
			QString			trimmedStr = strCurLine.right(strCurLine.length() - 1);
			trimmedStr.remove(QRegExp("\\|[h]{0,1}$"));

			QStringList		strListColumns = trimmedStr.split(QRegExp("\\|(?!\"\"\")"));	// Avoid """|"""
			numColumns = strListColumns.size();

			// Remove stuff within """
			QStringList		strListColumnsProcessed;
			foreach (QString strListItem, strListColumns)
			{
				strListColumnsProcessed << strListItem.replace("\"\"\"", "");
			}

			if (numColumns > 1)
			{
				if (pCurrentTable == NULL)
				{
					// New table
					QTextTableFormat		newTableFormat;

					// Delete the text row (it is still selected, so deleteChar()
					// should delete the entire row).
					curPos.deleteChar();
					pCurrentTable = curPos.insertTable(1, numColumns, newTableFormat);

					// Insert text into the table.
					curRow = 0;

					// Cursor is now at start of table.  Move it past the table.
					curPos.movePosition(QTextCursor::Down);
				}
				else
				{
					// Existing table; move line into table.

					// Delete the text row (it is still selected, so deleteChar()
					// should delete the entire row).
					curPos.deleteChar();
					pCurrentTable->appendRows(1);

					// Insert text into the table.
					curRow++;
				}

				for (int i = 0; i < numColumns; i++)
				{
					QTextTableCell	curCell = pCurrentTable->cellAt(curRow, i);

					Q_ASSERT(curCell.isValid());

					if (curCell.isValid())
					{
						QTextCursor		cellCursor = curCell.firstCursorPosition();
						cellCursor.insertHtml(strListColumnsProcessed.at(i));

						// Process leading !s
						if (strListColumnsProcessed.at(i).startsWith("!"))
						{
							// Remove the ! and set the background color
							cellCursor.movePosition(QTextCursor::StartOfBlock);
							cellCursor.deleteChar();		// Delete the !
							cellCursor.select(QTextCursor::LineUnderCursor);

							QTextCharFormat	selectionFormat;
							selectionFormat = cellCursor.blockCharFormat();
							selectionFormat.setBackground(QBrush(QColor(255, 255, 204)));
							cellCursor.setBlockCharFormat(selectionFormat);
						}
					}
				}

			}
		}
		else
		{
			// No table here
			if (pCurrentTable != NULL)
			{
				// We've just finished constructing a table.  Set the column sizes.
				QTextTableFormat		tableFormat = pCurrentTable->format();
				QVector<QTextLength>	columnWidthConstraints(0);
				QVector<int>			maxColumnWidths(pCurrentTable->columns());

				// Initialize max column widths
				for (int i = 0; i < pCurrentTable->columns(); i++)
				{
					maxColumnWidths[i] = 0;
				}

#if 0
				// 11/18/2010: I found it works better to not try to explicitly set column widths.
				// This way, the table expands to fill the document width, and changes as the
				// document width changes.

				// Determine max widths for each column
				for (int row = 0; row < pCurrentTable->rows(); row++)
				{
					for (int col = 0; col < pCurrentTable->columns(); col++)
					{
						int cellWidth = CellWidth(pCurrentTable, row, col);

						if (cellWidth > maxColumnWidths.at(col))
						{
							maxColumnWidths[col] = cellWidth;
						}
					}
				}

				//QTextLength				tempTextLength(QTextLength::FixedLength, maxColumnWidths.at(i));
				//QTextLength				tempTextLength(QTextLength::FixedLength, 150.0);

				for (int i = 0; i < pCurrentTable->columns(); i++)
				{
					QTextLength		*tempTextLength = new QTextLength(QTextLength::FixedLength, (qreal) maxColumnWidths.at(i));

					if (tempTextLength != NULL)
					{
						columnWidthConstraints.append(*tempTextLength);
					}

					// TODO: Should tempTextLength be deleted?
				}

				tableFormat.setColumnWidthConstraints(columnWidthConstraints);
#endif
				pCurrentTable->setFormat(tableFormat);
				pCurrentTable = NULL;		// Done with current table
			}
		}

		// Go to next line
		if (! curPos.movePosition(QTextCursor::Down))
		{
			break;		// At end of document
		}
	}
}

//--------------------------------------------------------------------------
void CWikiTextRenderer::ReplaceHorizLineElements()
{
	CLineFinder				lineFinder(m_pDoc, "----");
	CHorizLineConverter		horizLineConverter;

	CLooper			docLooper(&lineFinder, &horizLineConverter);
	docLooper.TraverseDocument();
}

//--------------------------------------------------------------------------
QString CWikiTextRenderer::GetRenderedHtml()
{
	return m_pDoc->toHtml();
}


/************************************************************************/
/* Converter functions                                                  */
/************************************************************************/

bool CHeaderConverter::Convert( QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText )
{
	int		fontSize;
	int		numExcl = foundText.length();		// Number of exclamation marks

	// The font size varies based on the number of exclamation marks found.
	// A single exclamation mark indicates the largest font.  Additional
	// exclamation marks reduce the size by 2 points per exclamation mark,
	// with a lower limit of ???.

	fontSize = kMaxHeaderSize - (numExcl - 1) * 2;
	if (fontSize < 3)
		fontSize = 3;

	// Set font size for the line
	curPosition.select(QTextCursor::LineUnderCursor);

	if (curPosition.hasSelection())
	{
		QTextCharFormat		lineCharFormat;

		lineCharFormat = curPosition.charFormat();
		lineCharFormat.setFontPointSize(fontSize);
		curPosition.setCharFormat(lineCharFormat);
	}

	// Insert a horizontal line, for one, two, and three exclamation marks
	if (numExcl < 4)
	{
		curPosition.movePosition(QTextCursor::EndOfLine);
		curPosition.insertHtml("<hr />");
	}

	return false;		// Don't need caller to apply the character format
}

//--------------------------------------------------------------------------
bool CMonospaceBlockConverter::Convert( QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText )
{
	selectionFormat.setFontFamily("Courier New");
	selectionFormat.setFontFixedPitch(true);
	selectionFormat.setForeground(QBrush(Qt::black));

	// This was intended to set a margin around the block so that the block's
	// background could be made to look as it does in the TiddlyWiki.  However,
	// it has proved impossible to select the text under the block for
	// some reason, so I've abandoned this for now.  I may revisit it in the future.

	// See if a margin can be set
	QTextBlockFormat	blockFormat = curPosition.blockFormat();
	qreal	topMargin = blockFormat.topMargin();
	qreal	bottomMargin = blockFormat.bottomMargin();
	qreal	leftMargin = blockFormat.leftMargin();

// 		topMargin += 2;
// 		bottomMargin += 2;
// 
// 		blockFormat.setTopMargin(topMargin);
// 		blockFormat.setBottomMargin(bottomMargin);

//		blockFormat.setLeftMargin(2);
//		blockFormat.setTextIndent(5);

	// Attempt to set the background for the block
	blockFormat.setBackground(QBrush(QColor(255, 255, 204)));

	curPosition.setBlockFormat(blockFormat);

#if 0
	// It doesn't seem possible to set a border around a block.

	// Attempt to set a border color
	QTextFrame	*pFrame = curPosition.currentFrame();

	if (pFrame != NULL)
	{
		QTextFrameFormat	frameFormat = pFrame->frameFormat();
		qreal	currentBorder = frameFormat.border();

		//frameFormat.setBorderBrush(QBrush(Qt::darkYellow));
		frameFormat.setBorderBrush(QBrush(Qt::blue));
		frameFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
		frameFormat.setBorder(2);

		pFrame->setFrameFormat(frameFormat);
	}
#endif

	return true;
}

//--------------------------------------------------------------------------
bool CBlockQuoteConverter::Convert( QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText )
{
	// This is very similar to the monospace block quote section
	selectionFormat.setForeground(QBrush(Qt::black));

	QTextBlockFormat	blockFormat = curPosition.blockFormat();

	blockFormat.setLeftMargin(20);

	curPosition.setBlockFormat(blockFormat);

	return true;
}


/************************************************************************/
/* CAbstractFinder                                                      */
/************************************************************************/

CAbstractFinder::CAbstractFinder(QTextDocument* pDoc, const QString& markup)
: m_pDocument(pDoc), m_startMarkup(markup), m_endMarkup(markup), m_curPosition(pDoc), m_selectionCursor(pDoc),
m_capturedText(""), m_foundText("")
{
	// Note: passing the document to the constructor of m_curPosition will initialize it
	// to point to the beginning of the document.
}

//--------------------------------------------------------------------------
CAbstractFinder::CAbstractFinder(QTextDocument* pDoc, const QString& startMarkup, const QString& endMarkup)
: m_pDocument(pDoc), m_startMarkup(startMarkup), m_endMarkup(endMarkup), m_curPosition(pDoc), m_selectionCursor(pDoc),
m_capturedText(""), m_foundText("")
{
	// Note: passing the document to the constructor of m_curPosition will initialize it
	// to point to the beginning of the document.
}

//--------------------------------------------------------------------------
CAbstractFinder::CAbstractFinder( QTextDocument* pDoc, QRegExp& regExp, const QString& endMarkup )
: m_pDocument(pDoc), m_startMarkup(""), m_endMarkup(endMarkup), m_curPosition(pDoc), m_selectionCursor(pDoc),
m_startRegExp(regExp), m_capturedText(""), m_foundText("")
{
	// Note: passing the document to the constructor of m_curPosition will initialize it
	// to point to the beginning of the document.

	// For this case, where a regular expression is used, the m_startMarkup
	// member will be blank.
}

//--------------------------------------------------------------------------
QString CAbstractFinder::CapturedText( int captureIndex )
{
	if (captureIndex == 1)
	{
		return m_capturedText;
	}
	else
	{
		return m_startRegExp.cap(captureIndex);
	}
}


/************************************************************************/
/* CMarkupFinder                                                        */
/************************************************************************/

CMarkupFinder::CMarkupFinder(QTextDocument* pDoc, const QString& markup)
: CAbstractFinder(pDoc, markup)
{
}

//--------------------------------------------------------------------------
CMarkupFinder::CMarkupFinder(QTextDocument* pDoc, const QString& startMarkup, const QString& endMarkup)
: CAbstractFinder(pDoc, startMarkup, endMarkup)
{
}

//--------------------------------------------------------------------------
CMarkupFinder::CMarkupFinder(QTextDocument* pDoc, QRegExp& startRegExp, const QString& endMarkup)
: CAbstractFinder(pDoc, startRegExp, endMarkup)
{
}

//--------------------------------------------------------------------------
bool CMarkupFinder::Find()
{
	bool	bFound = false;

	QTextCursor		startPos, endPos;

	if (m_startMarkup.isEmpty())
	{
		// If m_startMarkup is empty, use the regular expression
		startPos = m_pDocument->find(m_startRegExp, m_curPosition);

		// m_foundText contains the entire matched text.  The "capture" part,
		// from the regular expression, could be (and is likely to be) a subset
		// of this string.
		m_foundText = startPos.selectedText();

		int		foundPos = m_startRegExp.indexIn(m_foundText);

		if (foundPos != -1)
		{
			m_capturedText = m_startRegExp.cap(1);
		}
	}
	else
	{
		startPos = m_pDocument->find(m_startMarkup, m_curPosition);
	}

	if (! startPos.isNull())
	{
		endPos = m_pDocument->find(m_endMarkup, startPos, 0);

		if (! endPos.isNull())
		{
			// Remove the markup text
			startPos.removeSelectedText();
			endPos.removeSelectedText();

			// Select the text delimited by the markup text
			m_selectionCursor.setPosition(startPos.position(), QTextCursor::MoveAnchor);
			m_selectionCursor.setPosition(endPos.position(), QTextCursor::KeepAnchor);

			m_curPosition = endPos;		// Save current position
			bFound = true;
		}
	}

	return bFound;
}


/************************************************************************/
/* CLineFinder                                                          */
/************************************************************************/

CLineFinder::CLineFinder( QTextDocument* pDoc, const QString& markup )
: CAbstractFinder(pDoc, markup, "")
{
}

//--------------------------------------------------------------------------
CLineFinder::CLineFinder( QTextDocument* pDoc, const QString& startMarkup, const QString& endMarkup )
: CAbstractFinder(pDoc, startMarkup, endMarkup)
{
}

//--------------------------------------------------------------------------
CLineFinder::CLineFinder(QTextDocument* pDoc, QRegExp& startRegExp)
: CAbstractFinder(pDoc, startRegExp, "")
{
}

//--------------------------------------------------------------------------
bool CLineFinder::FindInLine(QTextCursor& cursor, const QString& strFind, QRegExp& regExp)
{
	bool	bFound = false;

	while (! bFound)
	{
		QString		strCurLine;

		cursor.select(QTextCursor::LineUnderCursor);
		strCurLine = cursor.selectedText();

		if (strFind.isEmpty())
		{
			// If strFind is empty, use the regular expression
			int		foundPos = regExp.indexIn(strCurLine);

			if (foundPos != -1)
			{
				bFound = true;

				// Ensure that only the text that matches is selected
				cursor.movePosition(QTextCursor::StartOfLine);
				m_foundText = m_startRegExp.cap(0);			// Get the entire matched string
				m_capturedText = m_startRegExp.cap(1);		// Get the captured string, if any

				cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, m_foundText.length());
				break;
			}
		}
		else
		{
			if (strFind == strCurLine)
			{
				bFound = true;
				break;
			}
		}

		if (! bFound)
		{
			// Go to next line
			if (! cursor.movePosition(QTextCursor::Down))
			{
				break;		// At end of document
			}
		}
	}

	return bFound;
}

//--------------------------------------------------------------------------
bool CLineFinder::Find()
{
	bool	bFound = false;

	QTextCursor		startPos(m_pDocument), endPos(m_pDocument);

	if (FindInLine(m_curPosition, m_startMarkup, m_startRegExp))
	{
		// The found text is selected at this point
		m_curPosition.deleteChar();		// Delete the markup text
		m_selectionCursor = m_curPosition;

		if (! m_endMarkup.isEmpty())
		{
			startPos = m_curPosition;
			
			int startPosInt = startPos.position();		// DEBUG

			// Search for end markup
			QRegExp			rx;
			if (FindInLine(m_curPosition, m_endMarkup, rx))
			{
				// The found text is selected at this point
				bFound = true;

				endPos = m_curPosition;

				int endPosInt = endPos.position();

				m_curPosition.deleteChar();		// Delete the markup text

				// Select the text delimited by the markup text
				m_selectionCursor.setPosition(startPos.position(), QTextCursor::MoveAnchor);
				m_selectionCursor.setPosition(endPos.position(), QTextCursor::KeepAnchor);

				// DEBUG
				QString		selectedText;
				selectedText = m_selectionCursor.selectedText();
				// END DEBUG
			}
		}
		else
		{
			// No end text was requested
			bFound = true;

			m_selectionCursor.select(QTextCursor::LineUnderCursor);
		}
	}

#if 0
	while (! bFound)
	{
		QString		strCurLine;

		m_curPosition.select(QTextCursor::LineUnderCursor);
		strCurLine = m_curPosition.selectedText();

		if (m_startMarkup.isEmpty())
		{
			// If m_startMarkup is empty, use the regular expression
			int		foundPos = m_startRegExp.indexIn(strCurLine);

			if (foundPos != -1)
			{
				bFound = true;

				// Ensure only the text that matches is selected
				m_curPosition.movePosition(QTextCursor::StartOfLine);
				m_foundText = m_startRegExp.cap(0);		// Get the entire matched string
				m_capturedText = m_startRegExp.cap(1);

				m_curPosition.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, m_foundText.length());
			}
		}
		else
		{
			if (m_startMarkup == strCurLine)
				bFound = true;
		}

		// Does the line consist of only the markup string?
		if (bFound)
		{
			m_curPosition.deleteChar();		// Delete the markup text
			m_selectionCursor = m_curPosition;
		}
		else
		{
			// Go to next line
			if (! m_curPosition.movePosition(QTextCursor::Down))
			{
				break;		// At end of document
			}
		}
	}
#endif

	return bFound;
}


/************************************************************************/
/* CLooper                                                              */
/************************************************************************/

CLooper::CLooper(CAbstractFinder* pFinder, CAbstractConverter* pConverter)
: m_pFinder(pFinder), m_pConverter(pConverter)
{
}

//--------------------------------------------------------------------------
void CLooper::TraverseDocument()
{
	if (m_pFinder == NULL || m_pConverter == NULL)
		return;

	while (m_pFinder->Find())
	{
		if (m_pFinder->RequiresSelection())
		{
			if (! m_pFinder->GetSelectionCursor().hasSelection())
				continue;	// No selection - skip it
		}

		QTextCharFormat  selectionFormat;

		selectionFormat = m_pFinder->GetSelectionCursor().charFormat();
		QTextCursor			selCursor(m_pFinder->GetSelectionCursor());
		QString				capturedText(m_pFinder->CapturedText(1));

		if (m_pConverter->Convert(selectionFormat, selCursor, capturedText))
		{
			m_pFinder->GetSelectionCursor().setCharFormat(selectionFormat);
		}
	}
}
