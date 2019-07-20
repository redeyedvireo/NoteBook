#ifndef FAVORITESWIDGET_H
#define FAVORITESWIDGET_H

#include <QWidget>
#include "PageData.h"
#include "ui_FavoritesWidget.h"

class CNoteBook;
class CDatabase;

class CFavoritesWidget : public QWidget
{
	Q_OBJECT

public:
    CFavoritesWidget(QWidget *parent = 0);
	~CFavoritesWidget();

    void Initialize(CNoteBook *parent , CDatabase *db);

	/**
	 *	Removes all items from the GUI.  Does not update the database.
	 */
	void Clear();

	void AddPage(ENTITY_ID pageId);
	void RemovePage(ENTITY_ID pageId);

	void AddPages(ENTITY_LIST& pageIds);

public slots:
	void on_clearButton_clicked();
	void on_removeSelectedButton_clicked();

	void OnPageTitleChanged( CPageData pageData );
	void OnPageDeleted(ENTITY_ID pageId);
	void OnPageImported( CPageData pageData );
	void OnPageUpdatedByImport( CPageData pageData );
	void OnItemClicked( QListWidgetItem* item );

signals:
	void FW_PageSelected(ENTITY_ID pageId);
	void FW_PageDefavorited(ENTITY_ID pageId);

protected:
	void SetConnections(CNoteBook *parent);
	QListWidgetItem* NewItem(ENTITY_ID pageId, const QString& pageTitle);
	void AddItem(ENTITY_ID pageId, const QString& pageTitle);
	QListWidgetItem* FindItem( ENTITY_ID pageId );

private:
	Ui::CFavoritesWidget ui;

    CDatabase       *m_db;
};

#endif // FAVORITESWIDGET_H
