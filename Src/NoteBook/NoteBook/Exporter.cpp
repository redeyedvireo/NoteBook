#include "stdafx.h"
#include "Exporter.h"
#include "ExportDlg.h"
#include "Database.h"
#include <QTextCharFormat>

#define kPageTitlePointSize		18


/************************************************************************/
/* CAbstractTextOutput                                                  */
/************************************************************************/
CAbstractTextOutput::CAbstractTextOutput()
{
}

//--------------------------------------------------------------------------
bool CAbstractTextOutput::OpenFile(const QString& pathName)
{
	m_pOutputFile = new (std::nothrow) QFile(pathName);

	return m_pOutputFile->open(QFile::WriteOnly | QFile::Truncate);
}

//--------------------------------------------------------------------------
void CAbstractTextOutput::Done()
{
	m_pOutputFile->close();
	delete m_pOutputFile;
}


/************************************************************************/
/* CPlainTextOutput                                                     */
/************************************************************************/
CPlainTextOutput::CPlainTextOutput()
: CAbstractTextOutput()
{
}

//--------------------------------------------------------------------------
bool CPlainTextOutput::OpenFile( const QString& pathName )
{
	bool bResult = CAbstractTextOutput::OpenFile(pathName);

	if (bResult)
	{
		m_outputStream.setDevice(m_pOutputFile);
	}

	return bResult;
}

//--------------------------------------------------------------------------
void CPlainTextOutput::OutputPage( QTextDocument& pageDoc, const QString& pageTitle )
{
	m_outputStream << pageTitle;
	endl(m_outputStream);

	m_outputStream << "----------------------------------------------------";
	endl(m_outputStream);

	m_outputStream << pageDoc.toPlainText();
	endl(m_outputStream);

	endl(m_outputStream);		// Blank line between pages
}

//--------------------------------------------------------------------------
void CPlainTextOutput::Done()
{
	CAbstractTextOutput::Done();
}


/************************************************************************/
/* CRichTextOutput                                                      */
/************************************************************************/
CRichTextOutput::CRichTextOutput()
: CAbstractTextOutput()
{
	m_pCursor = new (std::nothrow) QTextCursor(&m_outputDoc);
}

//--------------------------------------------------------------------------
bool CRichTextOutput::OpenFile( const QString& pathName )
{
	return CAbstractTextOutput::OpenFile(pathName);
}

//--------------------------------------------------------------------------
QTextDocumentFragment CRichTextOutput::GetDocumentAsFragment(QTextDocument& doc)
{
	QTextCursor*	pCursor = new (std::nothrow) QTextCursor(&doc);

	// Select the entire page
	pCursor->movePosition(QTextCursor::Start);
	pCursor->movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

	QTextDocumentFragment	pageFrag = pCursor->selection();

	delete pCursor;

	return pageFrag;
}

//--------------------------------------------------------------------------
void CRichTextOutput::OutputPage( QTextDocument& pageDoc, const QString& pageTitle )
{
	// Create new document with the page title
	QTextDocument	titleDoc;
	QTextCursor		titleDocCursor(&titleDoc);

	// Use special font for the page title
	QTextCharFormat	charFormat = titleDocCursor.charFormat();
	QFont			theFont = charFormat.font();

	theFont.setPointSize(kPageTitlePointSize);
	theFont.setUnderline(true);
	charFormat.setFont(theFont);
	titleDocCursor.setCharFormat(charFormat);

	// Add title to output document
	titleDocCursor.insertText(pageTitle);
	titleDocCursor.insertText("\n");


	QTextCursor	pageCursor(&pageDoc);

	QTextDocumentFragment	titleFrag = GetDocumentAsFragment(titleDoc);

	// Insert page title into output document
	m_pCursor->insertFragment(titleFrag);

	QTextDocumentFragment	pageFrag = GetDocumentAsFragment(pageDoc);

	// Insert page text to output document
	m_pCursor->insertFragment(pageFrag);

	// Insert a few blank lines
	m_pCursor->insertText("\n\n\n");
}

//--------------------------------------------------------------------------
void CRichTextOutput::Done()
{
	// Write document to the file
	QTextStream		outStream(m_pOutputFile);

	outStream << m_outputDoc.toHtml();

	CAbstractTextOutput::Done();
}


/************************************************************************/
/* CExporter                                                            */
/************************************************************************/
CExporter::CExporter(CDatabase *db, QString& outputPath, ENTITY_LIST& pageIdList)
    : m_db(db), m_outputPath(outputPath), m_pageIdList(pageIdList)
{
}

//--------------------------------------------------------------------------
CExporter::~CExporter()
{
}

//--------------------------------------------------------------------------
CAbstractTextOutput* CExporter::CreateOutputObject(int exportFormatType)
{
	switch (exportFormatType)
	{
	case CExportDlg::kPlainText:
		return new (std::nothrow) CPlainTextOutput();

	case CExportDlg::kRichText:
		return new (std::nothrow) CRichTextOutput();

	default:
		return NULL;
	}
}

//--------------------------------------------------------------------------
bool CExporter::DoExport( int exportFormatType )
{
	bool	bReturn = false;

	CAbstractTextOutput*	pOutput = CreateOutputObject(exportFormatType);

	if (pOutput == NULL)
	{
		return false;
	}

	if (pOutput->OpenFile(m_outputPath))
	{
		foreach(ENTITY_ID pageId, m_pageIdList)
		{
			CPageData	pageData;

            if (m_db->GetPage(pageId, pageData))
			{
				QTextDocument		theDoc;

				theDoc.setHtml(pageData.m_contentString);

				pOutput->OutputPage(theDoc, pageData.m_title);
			}
		}

		pOutput->Done();
	}

	delete pOutput;

	return bReturn;
}
