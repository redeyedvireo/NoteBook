#ifndef PAGEHISTORYWIDGET_H
#define PAGEHISTORYWIDGET_H

#include "PageData.h"
#include <QListWidget>

// Keeps track of pages that the user has viewed.

#define kDefaultMaxHistory	20

class CNoteBook;
class CDatabase;

class CPageHistoryWidget : public QListWidget
{
	Q_OBJECT

public:
	CPageHistoryWidget(QWidget *parent);
	~CPageHistoryWidget();

    void Initialize( CNoteBook *parent, CDatabase *db );

	void AddItem(ENTITY_ID pageId, const QString& pageTitle, bool bAddAtEnd=false);

	/**
	 *	Returns the page history as a string of comma-separated page IDs
	 */
	QString GetPageHistory();

	/**
	 *	Initializes the widget with a history.
	 *	@param pageHistoryStr A string containing a comma-separated
	 *			list of page IDs.
	 */
	void SetPageHistory(const QString& pageHistoryStr);

	ENTITY_ID GetMostRecentlyViewedPage();

public slots:
	void OnPageTitleChanged( CPageData pageData );
	void OnPageDeleted(ENTITY_ID pageId);
	void OnPageImported( CPageData pageData );
	void OnPageUpdatedByImport( CPageData pageData );
	void OnItemClicked( QListWidgetItem* item );

signals:
	void PHW_PageSelected(ENTITY_ID pageId);

protected:
	void SetConnections(CNoteBook *parent);
	QListWidgetItem* FindItem(ENTITY_ID pageId);
	QListWidgetItem* NewItem(ENTITY_ID pageId, const QString& pageTitle);

private:
    CDatabase       *m_db;
	int				m_maxHistory;		// Maximum number of history elements tracked
};

#endif // PAGEHISTORYWIDGET_H
