#ifndef DATETREE_H
#define DATETREE_H

#include "PageData.h"
#include "DateWidgetItem.h"
#include <QTreeWidget>
#include <QMenu>


class CNoteBook;


class CDateTree : public QTreeWidget
{
	Q_OBJECT

public:
	CDateTree(QWidget *parent);
	~CDateTree();

	void Initialize( CNoteBook *parent );

	/*
	 *	Adds a new page item.  Creates a new date item, if necessary.
	 */
	void AddItem(const CPageData& pageData);

	/*
	 *	Adds multiple items to the tree.  The tree is cleared first.
	 *	This function is used when adding items from a newly-opened NoteBook file.
	 */
	void AddItems(const PAGE_HASH& pageHash);

protected:
	void InitMenus();
	void SetConnections(CNoteBook *parent);
	ENTITY_ID GetItemPageId(QTreeWidgetItem* pItem);

	/*
	 *	Finds a page item in the tree.
	 */
	QTreeWidgetItem* FindPageItem(ENTITY_ID pageId);
	CDateWidgetItem* PageItemParent(QTreeWidgetItem* pPageItem);

	CDateWidgetItem* FindDate(const QDate& inDate);
	CDateWidgetItem* AddDate(const QDate& inDate);

	void SortPageTitles();
	void SortDateItems();
	void SortTitlesWithinDate(QTreeWidgetItem* pItem);

public slots:
	void OnContextMenu(const QPoint& pos);
	void OnItemChanged(QTreeWidgetItem* pItem, int column);
	void OnItemClicked(QTreeWidgetItem* item, int column);
	void OnNewBlankPageCreated( CPageData pageData );
	void OnPageImported(CPageData pageData);
	void OnPageUpdatedByImport(CPageData pageData);

	/*
	 *	This slot is called when an existing page is saved.
	 */
	void OnPageSaved(CPageData pageData);

	void OnPageTitleChanged(CPageData pageData);
	void OnPageDeleted(ENTITY_ID pageId);

signals:
	void PageSelected(ENTITY_ID pageId);

private:
	QMenu				m_contextMenu;
};

#endif // DATETREE_H
