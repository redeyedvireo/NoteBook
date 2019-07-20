#pragma once
#include "PageData.h"
#include <QFile>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextStream>

class CDatabase;

//--------------------------------------------------------------------------
class CAbstractTextOutput
{
public:
	CAbstractTextOutput();
	virtual ~CAbstractTextOutput() {}

	virtual bool OpenFile(const QString& pathName);
	virtual void Done();

	virtual void OutputPage(QTextDocument& pageDoc, const QString& pageTitle) = 0;

protected:
	QFile*		m_pOutputFile;
};

//--------------------------------------------------------------------------
class CPlainTextOutput : public CAbstractTextOutput
{
public:
	CPlainTextOutput();
	virtual ~CPlainTextOutput() {}

	virtual bool OpenFile(const QString& pathName);
	virtual void Done();

	virtual void OutputPage(QTextDocument& pageDoc, const QString& pageTitle);

private:
	QTextStream		m_outputStream;
};

//--------------------------------------------------------------------------
class CRichTextOutput : public CAbstractTextOutput
{
public:
	CRichTextOutput();
	virtual ~CRichTextOutput() {}

	virtual bool OpenFile(const QString& pathName);
	virtual void Done();

	virtual void OutputPage(QTextDocument& pageDoc, const QString& pageTitle);

private:
	QTextDocumentFragment GetDocumentAsFragment(QTextDocument& doc);

	QTextDocument		m_outputDoc;
	QTextCursor*		m_pCursor;
};

//--------------------------------------------------------------------------
class CExporter
{
public:
    CExporter(CDatabase *db, QString& outputPath, ENTITY_LIST& pageIdList);
	~CExporter();

	/*
	 *	Exports selected pages to a file.
	 *	@param exportFormatType: 0 - Rich Text, 1 - Plain Text
	 */
	bool DoExport(int exportFormatType);

protected:
	CAbstractTextOutput* CreateOutputObject(int exportFormatType);

private:
    CDatabase           *m_db;
	QString				m_outputPath;
	ENTITY_LIST			m_pageIdList;
};
