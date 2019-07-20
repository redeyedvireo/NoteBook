//#include "stdafx.h"
#include "ImportTiddlyWiki.h"
#include "WikiTextRenderer.h"
#include "PageCache.h"
#include "importtiddlywikiplugin.h"
#include <QtGui>
#include <QList>
#include <QDate>
#include <QDateTime>
#include <QStringList>
#include <QMessageBox>
#include <QTextDocument>
#include <QtDebug>
#include <QProgressDialog>


// TODO: I think this can be done without using the jQuery files.
// #define kJQueryFile			"jquery-1.3.2.min.js"
// #define kMyStuffFile		"blogaccessor.js"

#define	kSearchString		"div[tags='journal']"


//--------------------------------------------------------------------------
CImportTiddlyWiki::CImportTiddlyWiki(int defaultFontSize, QWidget *parent)
	: m_initialFontSize(defaultFontSize), QDialog(parent)
{
	m_pWebPage = NULL;
}

//--------------------------------------------------------------------------
CImportTiddlyWiki::~CImportTiddlyWiki()
{
	if (m_pWebPage != NULL)
	{
		delete m_pWebPage;
	}
}

//--------------------------------------------------------------------------
PAGE_LIST CImportTiddlyWiki::Import(const QString& filePath)
{
    // TODO: This must be done with Javascript, as Qt's new Web Engine does not support accessing the
    //       HTML DOM.
#if 0
	// Get file from edit control, and load it
	QFileInfo	fileInfo(filePath);

	if (fileInfo.exists())
	{
		// Open and read first couple lines of file, for display in the edit control
		QFile	fileObj(filePath);

		if (fileObj.open(QIODevice::ReadOnly))
		{
			// Display wait cursor
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

			QByteArray	htmlData = fileObj.readAll();

			// TODO: Use QSharedPointer or some smart pointer.
			//			Actually, does this need to be a pointer?
			m_pWebPage = new QWebPage();

			if (m_pWebPage == NULL)
			{
				QApplication::restoreOverrideCursor();
				return PAGE_LIST();
			}

			connect(m_pWebPage, SIGNAL(initialLayoutCompleted()), this, SLOT(OnWebFrameLayoutDone()));

			IMPORT_PAGE_LIST	pageList;

			QWebFrame*	pFrame = m_pWebPage->mainFrame();
			pFrame->setHtml(htmlData);

			QWebElement document = pFrame->documentElement();

			// The user's tiddlers seem to be in a list of <div>'s under a <div>
			// with id="storeArea".
			QWebElement storeAreaDiv = document.findFirst("div[id='storeArea']");

			// Can filter out "system" tiddlers.  These will have id's with
			// values: systemConfig, systemPalette, systemServer, systemTheme.
			// Could possibly filter out any div with an id that begins with "system",
			// but this could potentially filter some of the user's tiddlers.

			QWebElementCollection allPages = storeAreaDiv.findAll("div");
			QList<QWebElement> allPagesList = allPages.toList();

			int numEntries = allPagesList.size();
			qDebug() << "Number of pages: " << numEntries;

			if (numEntries > 0)
			{
				for (int i = 0; i < numEntries; i++)
				{
					QWebElement		webElement = allPagesList.at(i);
					int				changeCount = 0;

					QWebElement		preElement = webElement.firstChild();
					QString			pageContentText = preElement.toPlainText();

					QString			createdDateString = webElement.attribute("created");
					QString			modifiedDateString = webElement.attribute("modified");
					QString			changeCountString = webElement.attribute("changecount");
					QString			titleString = webElement.attribute("title");
					QString			modifierString = webElement.attribute("modifier");
					QString			tagsString = webElement.attribute("tags");

					// Filter out "system" tiddlers
					if (IsSystemTiddler(tagsString))
						continue;

					if (changeCountString.length() > 0)
					{
						bool	bOk;
						changeCount = changeCountString.toInt(&bOk, 10);
						if (! bOk)
						{
							changeCount = 0;
						}
					}
					else
					{
						changeCount = 0;
					}

					CImportPageData		newPage;

					newPage.m_title = titleString;
					newPage.m_pageType = kPageTypeUserText;
					newPage.m_contentString = RenderWikiText(pageContentText);
					newPage.m_tags = tagsString;
					newPage.m_modifiedDateTime = ParseWikiDate(modifiedDateString);
					newPage.m_createdDateTime = ParseWikiDate(createdDateString);
					newPage.m_numModifications = changeCount;
					newPage.m_pageModifier = modifierString;

					pageList.append(newPage);
				}
			}

			// TODO: If a QSharedPointer or some smart pointer is used, it
			//		won't be necessary to delete it
			delete m_pWebPage;
			m_pWebPage = NULL;

			QApplication::restoreOverrideCursor();

			if (pageList.size() > 0)
			{
				// Sort the list
				qSort(pageList.begin(), pageList.end(), dateLessThan);

				// Show preview dialog and add to database if user is OK with it.
				CPreviewImportDialog	previewDialog(pageList);
				previewDialog.exec();

				if (previewDialog.result() == QDialog::Accepted)
				{
					return previewDialog.GetPages();
				}
			}
		}
	}
#endif
	return PAGE_LIST();		// Return an empty page list if there are errors
}

//--------------------------------------------------------------------------
QDateTime CImportTiddlyWiki::ParseWikiDate( const QString& wikiDate )
{
	QDateTime	tempDate;

	// I think the time stored is a UTC time
	tempDate = QDateTime::fromString(wikiDate, "yyyyMMddHHmm");

	return tempDate.toLocalTime();
}

//--------------------------------------------------------------------------
void CImportTiddlyWiki::OnWebFrameLayoutDone()
{
	// TODO: Is this function needed?
	QMessageBox::information(this, "NoteBook - Import TiddlyWiki", "Initial web layout completed.");
}

//--------------------------------------------------------------------------
QString CImportTiddlyWiki::RenderWikiText(const QString& plainText)
{
	CWikiTextRenderer	wikiRenderer;

	wikiRenderer.SetWikiText(plainText, m_initialFontSize);

	return wikiRenderer.GetRenderedHtml();
}

//--------------------------------------------------------------------------
bool CImportTiddlyWiki::IsSystemTiddler(const QString& tagsString)
{
	// TODO: Should probably create a list somewhere (possibly a user preference?)
	//		that contains all possible tags of system tiddlers

	QStringList		systemTiddlerList;
	
	systemTiddlerList << "systemConfig" << "systemPalette" << "systemTheme" << "systemServer";

	foreach (QString tagStr, systemTiddlerList)
	{
		if (tagsString.contains(tagStr, Qt::CaseInsensitive))
		{
			return true;
		}
	}

	// TODO: Could also possibly filter out tiddlers with "MPTW" as creator,
	//		or ask user to enter a creator, and only display tiddlers
	//		with that creator.

	return false;
}
