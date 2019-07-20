#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include "PageData.h"
#include "ui_SearchWidget.h"


class CNoteBook;
class CDatabase;

class CSearchWidget : public QWidget
{
	Q_OBJECT

public:
	CSearchWidget(QWidget *parent = 0);
	~CSearchWidget();

    void Initialize( CNoteBook *parent, CDatabase *db );

	void Clear();

	/**
	 *	Conduct a search over all pages (content and page titles).
	 *	@param searchText Text to search for.
	 */
	void DoSearch(const QString& searchText);

public slots:
	void on_searchButton_clicked();

	void OnPageTitleChanged( CPageData pageData );
	void OnPageDeleted(ENTITY_ID pageId);
	void OnPageImported( CPageData pageData );
	void OnPageUpdatedByImport( CPageData pageData );
	void OnItemClicked( QListWidgetItem* item );

signals:
	void SW_PageSelected(ENTITY_ID pageId);

protected:
	void SetConnections(CNoteBook *parent);
	QListWidgetItem* NewItem(ENTITY_ID pageId, const QString& pageTitle);
	void AddItem(ENTITY_ID pageId, const QString& pageTitle);
	QListWidgetItem* FindItem( ENTITY_ID pageId );

private:
	Ui::CSearchWidget ui;

    CDatabase           *m_db;
};

#endif // SEARCHWIDGET_H
