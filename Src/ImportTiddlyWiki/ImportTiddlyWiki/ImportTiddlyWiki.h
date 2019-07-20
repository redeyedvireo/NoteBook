#ifndef IMPORTTIDDLYBLOG_H
#define IMPORTTIDDLYBLOG_H

#include <QDialog>
#if (QT_VERSION < 0x050000)
#include <QWebPage>
#else
#include <QWebEnginePage>
#endif
#include "PreviewImportDialog.h"


class CImportTiddlyWiki : public QDialog
{
	Q_OBJECT

public:
	CImportTiddlyWiki(int defaultFontSize, QWidget *parent = 0);
	~CImportTiddlyWiki();

	/*
	 *	Entry point for performing the import of a Tiddly Wiki.
	 */
	PAGE_LIST Import(const QString& filePath);

private slots:
	void OnWebFrameLayoutDone();

private:
	QDateTime ParseWikiDate(const QString& wikiDate);

	/*
	 *	Renders wiki text as HTML.
	 */
	QString RenderWikiText(const QString& plainText);

	void DuplicateEntryQuery(const QString& pageTitle, DecisionType& outUserDecision, bool& outDoAlways);
	void DoImport(PAGE_LIST& logEntryList);
	bool IsSystemTiddler(const QString& tagsString);

signals:
	void ITW_BeginDatabaseTransaction();
	void ITW_EndDatabaseTransaction();

private:
    QWebEnginePage*			m_pWebPage;
	int						m_initialFontSize;
};

#endif // IMPORTTIDDLYBLOG_H
