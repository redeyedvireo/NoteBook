#include <QString>
#include <QRegExp>
#include <QTextDocument>
#include <QTextTable>

/*
	This file is used to convert Tiddly Wiki markup text to HTML.  Although
	the conversion functions here work pretty well, the approach taken here
	is not the best way to do this.  The best way to parse Wiki text is to
	use a full-fledged syntax parser.
*/

/************************************************************************/
/* Finder Classes                                                       */
/************************************************************************/

class CAbstractFinder
{
public:
	CAbstractFinder(QTextDocument* pDoc, const QString& markup);
	CAbstractFinder(QTextDocument* pDoc, const QString& startMarkup, const QString& endMarkup);
	CAbstractFinder(QTextDocument* pDoc, QRegExp& regExp, const QString& endMarkup);
	virtual ~CAbstractFinder() {}

	virtual bool Find() = 0;
	QTextCursor GetSelectionCursor()	{ return m_selectionCursor; }
	virtual bool RequiresSelection()	{ return true; }

	/*
	 *	Return the captured text from the regular expression.  This is
	 *	text that that occurs within parentheses in the regular expression.
	 *	Note that, when used by a CMarkupFinder, only the first captured
	 *	text can be returned.  This could be changed by using a QStringList
	 *	for m_capturedText, instead of a QString.
	 */
	QString CapturedText(int captureIndex);

	/*
	 *	Return the entire text found by the regular expression.  This
	 *	is likely to be a superset of the captured text
	 */
	QString FoundText()					{ return m_foundText; }

protected:
	QTextDocument*	m_pDocument;
	QString			m_startMarkup;
	QString			m_endMarkup;
	QRegExp			m_startRegExp;

	QTextCursor		m_curPosition;
	QTextCursor		m_selectionCursor;

	// For regular expression searches only
	QString			m_capturedText;		// For now, only the first capture will be stored
	QString			m_foundText;		// The entire found text (might be more than just the captured text)
};

//--------------------------------------------------------------------------
class CMarkupFinder : public CAbstractFinder
{
public:
	CMarkupFinder(QTextDocument* pDoc, const QString& markup);
	CMarkupFinder(QTextDocument* pDoc, const QString& startMarkup, const QString& endMarkup);
	CMarkupFinder(QTextDocument* pDoc, QRegExp& startRegExp, const QString& endMarkup);
	virtual ~CMarkupFinder() {}

	virtual bool Find();
};

//--------------------------------------------------------------------------
class CLineFinder : public CAbstractFinder
{
public:
	CLineFinder(QTextDocument* pDoc, const QString& markup);
	CLineFinder(QTextDocument* pDoc, const QString& startMarkup, const QString& endMarkup);
	CLineFinder(QTextDocument* pDoc, QRegExp& startRegExp);
	virtual ~CLineFinder() {}

	virtual bool Find();
	virtual bool RequiresSelection()	{ return false; }

	/*
	 *	Searches for a line that equals (exactly) the strFind string, or that
	 *	matches the regular expression.  On return, if found, the found text
	 *	is selected in the document.
	 *
	 *	@param cursor Searching starts on the line contained by the cursor.  On
	 *			return, cursor contains the line on which the match occurred.
	 *	@param strFind The string to find.  If this is empty, regExp is used.
	 *	@param regExp Regular expression to find.  This is used only if
	 *			strFind is empty.  On return, (and if strFind is empty), this
	 *			will be initialized with captured text (if any).
	 *	@return Returns true if found, false otherwise.
	 */
	bool FindInLine(QTextCursor& cursor, const QString& strFind, QRegExp& regExp);
};


/************************************************************************/
/* Converter Classes                                                    */
/************************************************************************/

class CAbstractConverter
{
public:
	CAbstractConverter() {}
	virtual ~CAbstractConverter() {}

	/*
	 *	Converts a text selection (from a CFinder-derived class) to
	 *	HTML.
	 *	@param selectionFormat The char selection format of the selected text
	 *	@param curPosition The cursor position at the start of the selection
	 *	@param foundText Used only by a regular expression search, this
	 *			contains the contents of the text that was found.  For
	 *			non-regular expression searches, this will be an empty string.
	 *	@return If true, the calling function should apply the character format
	 *			to the document.  If false, the Convert function will have
	 *			done it.
	 */
	virtual bool Convert(	QTextCharFormat& selectionFormat,
							QTextCursor& curPosition,
							QString& foundText) = 0;
};

//--------------------------------------------------------------------------
class CBoldConverter : public CAbstractConverter
{
public:
	CBoldConverter() {}
	~CBoldConverter() {}

	virtual bool Convert(QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText)
	{
		selectionFormat.setFontWeight(QFont::Bold);
		return true;
	}
};

//--------------------------------------------------------------------------

class CItalicConverter : public CAbstractConverter
{
public:
	CItalicConverter() {}
	~CItalicConverter() {}

	virtual bool Convert(QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText)
	{
		selectionFormat.setFontItalic(true);
		return true;
	}
};

//--------------------------------------------------------------------------
class CStrikethroughConverter : public CAbstractConverter
{
public:
	CStrikethroughConverter() {}
	~CStrikethroughConverter() {}

	virtual bool Convert(QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText)
	{
		selectionFormat.setFontStrikeOut(true);
		return true;
	}
};

//--------------------------------------------------------------------------
class CUnderlineConverter : public CAbstractConverter
{
public:
	CUnderlineConverter() {}
	~CUnderlineConverter() {}

	virtual bool Convert(QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText)
	{
		selectionFormat.setFontUnderline(true);
		return true;
	}
};

//--------------------------------------------------------------------------
class CMonospaceConverter : public CAbstractConverter
{
public:
	CMonospaceConverter() {}
	~CMonospaceConverter() {}

	virtual bool Convert(QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText)
	{
		selectionFormat.setFontFamily("Courier New");
		selectionFormat.setFontFixedPitch(true);
		selectionFormat.setForeground(QBrush(Qt::darkYellow));
		return true;
	}
};

//--------------------------------------------------------------------------
class CMonospaceBlockConverter : public CAbstractConverter
{
public:
	CMonospaceBlockConverter() {}
	~CMonospaceBlockConverter() {}

	virtual bool Convert(QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText);
};

//--------------------------------------------------------------------------
class CBlockQuoteConverter : public CAbstractConverter
{
public:
	CBlockQuoteConverter() {}
	~CBlockQuoteConverter() {}

	virtual bool Convert(QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText);
};

//--------------------------------------------------------------------------
class CHighlightConverter : public CAbstractConverter
{
public:
	CHighlightConverter() {}
	~CHighlightConverter() {}

	virtual bool Convert(QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText)
	{
		selectionFormat.setBackground(QBrush(Qt::yellow));
		return true;
	}
};

//--------------------------------------------------------------------------
class CHorizLineConverter : public CAbstractConverter
{
public:
	CHorizLineConverter() {}
	~CHorizLineConverter() {}

	virtual bool Convert(QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText)
	{
		curPosition.insertHtml("<hr />");
		return false;
	}
};

//--------------------------------------------------------------------------
class CHeaderConverter : public CAbstractConverter
{
public:
	CHeaderConverter() {}
	~CHeaderConverter() {}

	virtual bool Convert(QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText);
};

//--------------------------------------------------------------------------
class CColorMarkupConverter : public CAbstractConverter
{
public:
	CColorMarkupConverter() {}
	~CColorMarkupConverter() {}

	virtual bool Convert(QTextCharFormat& selectionFormat, QTextCursor& curPosition, QString& foundText)
	{
		QColor	color(foundText);

		if (! color.isValid())
			return false;

		selectionFormat.setForeground(QBrush(color));

		return true;
	}
};


/************************************************************************/
/* Looper Class                                                         */
/************************************************************************/
class CLooper
{
public:
	CLooper(CAbstractFinder* pFinder, CAbstractConverter* pConverter);
	~CLooper() {}

	void TraverseDocument();

private:
	CAbstractFinder		*m_pFinder;
	CAbstractConverter	*m_pConverter;
};


/************************************************************************/
/* CWikiTextRenderer                                                    */
/************************************************************************/

class CWikiTextRenderer
{
public:
	CWikiTextRenderer(void);
	~CWikiTextRenderer(void);

	/*
	 *	Sets the wiki text to render.
	 *	@param wikiText Text to render.
	 *	@param defaultFontSize Font size at which the text should be rendered.
	 */
	void SetWikiText(const QString& wikiText, int defaultFontSize);

	QTextDocument*	GetDoc()	{ return m_pDoc; }
	QString GetRenderedHtml();

private:
	/*
	 *	Sets the font size of the document.
	 */
	void SetDocumentFontSize(int fontSize);

	/*
	 *	Conversion functions
	 */

	/*
	 *	Finds the specified markup text and selects the text that
	 *	is delimited by it.  The markup characters themselves are
	 *	removed.
	 *	@param startMarkupString The markup characters, as a string, indicating
	 *			the start of the sequence.
	 *	@param endMarkupString The markup characters, as a string, indicating
	 *			the end of the sequence.
	 *	@param selectionCursor If found, the cursor that indicates the selected text.
	 *	@return Returns true if the markup text was found, false otherwise
	 */
	bool FindAndSelectMarkupText(	const QString& startMarkupString,
									const QString& endMarkupString,
									QTextCursor& selectionCursor);

	/*
	 *	A convenience function that where the start markup string and end
	 *	markup string are the same.
	 *	@param markupString The markup characters, as a string.
	 *	@param selectionCursor If found, the cursor that indicates the selected text.
	 *	@return Returns true if the markup text was found, false otherwise
	 */
	bool FindAndSelectMarkupText(const QString& markupString, QTextCursor& selectionCursor);

	// TODO: Need a similar FindAndSelectMarkupText that has different
	// markup characters for the begin and the end (like the {{{ }}} combo,
	// and the @@(red)...@@ combo.)


	/*
	 *	 Convert {{{...}}} to Courier New, and remove the curly braces
	 */
	void ReplaceCurlyBraceTrio();

	/*
	 *	Convert {{{ and }}} that are used to specify a monospace block.  Note
	 *	that this is different that the ReplaceCurlyBraceTrio.  This must be
	 *	done before calling ReplaceCurlyBraceTrio().
	 */
	void ReplaceMonospaceBlock();

	/*
	 *	Convert <<< and <<< (block quote markup) to a block quote block.
	 */
	void ReplaceBlockQuoteBlock();

	/*
	 *	Convert //...// to Italic, and remove the slashes
	 */
	void ReplaceSlashItalics();

	/*
	 *	Convert ''...'' to bold, and remove the quotes.
	 */
	void ReplaceBoldQuotes();

	/*
	 *	Convert --...-- to strikeout, and remove the dashes.
	 */
	void ReplaceStrikethroughDashes();

	/*
	 *	Convert __...__ to underline, and remove the underline characters.
	 */
	void ReplaceUnderlineMarkup();

	/*
	 *	Convert @@...@@ to highlighted text, and remove the @@ characters.
	 */
	void ReplaceHighlightMarkup();

	/*
	 *	Convert !... to larger text, and remove the exclamation points.
	 */
	void ReplaceHeaders();

	/*
	 *	Replace color markup: 
	 */
	void ReplaceColorMarkup();

	/*
	 *	Convert # and * to lists
	 */
	void ReplaceListElements();

	/*
	 *	Returns the width of a cell, in pixels.
	 */
	int CellWidth(QTextTable* pTable, int row, int column);

	/*
	 *	Replace table formatting wiki text with a table structure.
	 */
	void ReplaceTableElements();

	/*
	 *	Replace horizontal line wiki text with an HTML horizontal line
	 */
	void ReplaceHorizLineElements();

	void DeleteDoc();
	void Render(int fontSize);

	QString			m_wikiText;
	QTextDocument	*m_pDoc;
};
