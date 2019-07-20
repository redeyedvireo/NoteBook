#ifndef PAGETITLELIST_H
#define PAGETITLELIST_H

#include "PageData.h"
#include <QListWidget>

class CNoteBook;

class CPageTitleList : public QListWidget
{
	Q_OBJECT

public:
	CPageTitleList(QWidget *parent);
	virtual ~CPageTitleList();

	void Initialize(CNoteBook *parent);

	void AddItem(ENTITY_ID pageId, const QString& pageTitle);

	/*
	 *	Adds multiple items to the tree.  The tree is cleared first.
	 *	This function is used when adding items from a newly-opened NoteBook file.
	 */
	void AddItems(const PAGE_HASH& pageHash);

protected:
	void SetConnections(CNoteBook *parent);
	QListWidgetItem* FindItem(ENTITY_ID pageId);

public slots:
	void OnNewBlankPageCreated(CPageData pageData);
	void OnItemClicked(QListWidgetItem *item);
	void OnPageTitleChanged(CPageData pageData);
	void OnPageDeleted(ENTITY_ID pageId);

	void OnPageImported(CPageData pageData);
	void OnPageUpdatedByImport(CPageData pageData);

signals:
	void PTL_PageSelected(ENTITY_ID pageId);
};

#endif // PAGETITLELIST_H
