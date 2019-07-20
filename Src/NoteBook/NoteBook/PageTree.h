#ifndef PAGETREE_H
#define PAGETREE_H

#include "PageData.h"
#include "uidef.h"
#include <QTreeWidget>
#include <QMouseEvent>
#include <QDropEvent>
#include <QMenu>
#include <QSignalMapper>


class CNoteBook;
class CDatabase;

typedef enum {
	eItemPage = 0,
    eItemFolder,
    eItemToDoList
} PageWidgetItemType;


/************************************************************************/
/* CPageWidgetItem                                                      */
/************************************************************************/

class CPageWidgetItem : public QTreeWidgetItem
{
public:
	CPageWidgetItem(QTreeWidgetItem *parent, const int pageId, PageWidgetItemType itemType);
	virtual ~CPageWidgetItem() {}

	PageWidgetItemType PageItemType()	{ return m_itemType; }

	ENTITY_ID PageId()		{ return m_pageId; }
	void SetPageId(ENTITY_ID pageId)	{ m_pageId = pageId; }

	/*
	 *	Updates the icon depending on whether it is expanded or collapsed.
	 */
	void UpdateIcon();

private:
	ENTITY_ID 			m_pageId;
	PageWidgetItemType	m_itemType;
};

typedef QList<CPageWidgetItem*>		PageItemList;


/************************************************************************/
/* CPageTree                                                            */
/************************************************************************/

class CPageTree : public QTreeWidget
{
	Q_OBJECT

public:
    CPageTree(QWidget *parent);
	~CPageTree();

    void Initialize(QWidget *parent, CDatabase *db);

	void AddTopLevelItem(const QString& pageTitle, ENTITY_ID pageId, PageWidgetItemType itemType);

	/*
	 *	Creates a new entry in the tree, and makes it editable
	 *	so the user can enter a title.
     *	@param pageAdd Indicates whether a page or a folder will be created.
     *	@param pageAddWhere Indicates whether the item will be created as a child
     *		of the current item in the tree, as a sibling of the current item,
     *		or as a top-level item.
     *  @param title If specified, will be used as the item's title.  In this case,
     *      the item will not be made editable for the user to enter a title.
	 */
    void NewItem(PAGE_ADD pageAdd, PAGE_ADD_WHERE pageAddWhere = ePageAddDefault, QString title=QString());

	/*
	 *	Adds an item to the tree, and does not make it editable.  The title
	 *	for the item is taken from the page cache.  This function is
	 *	used when adding items from a newly-opened NoteBook file.
	 *	@return Returns true if added, false otherwise.
	 */
	bool AddItem(ENTITY_ID pageId, ENTITY_ID parentId, PageWidgetItemType itemType);

	/*
	 *	Adds multiple items to the tree.  The tree is cleared first.
	 *	This function is used when adding items from a newly-opened NoteBook file.
	 *	THIS FUNCTION SEEMS TO BE OBSOLETE.  IT APPEARS AddItemsNew IS TO BE USED INSTEAD.
	 */
	void AddItems(const PAGE_HASH& pageHash);

	void AddItemsNew(const PAGE_HASH& pageHash, const QString& pageOrderStr);

	/*
	 *	Adds items using the page order string.  The page order string is simply a string
	 *	that contains a list of page IDs, which is the order in which to add them.
	 */
	void AddItemsUsingPageOrderString(const PAGE_HASH& pageHash, const QString& pageOrderStr);

	/*
	 *	Finds a page.
	 *	@return Returns a pointer to the item, or NULL if not found.
	 */
	CPageWidgetItem* FindItem(ENTITY_ID pageId);

	/*
	 *	Returns a comma-separated string that represents the entire tree of
	 *	pages.  Each folder is terminated by a 0.
	 */
	QString GetTreeIdList();

    /**
     * Writes the page order to the database.
     */
    void WritePageOrderToDatabase();

protected:
	/*
	 *	Moves a page widget item to a new parent.
	 */
	bool MoveItem(CPageWidgetItem *pItem, CPageWidgetItem *pNewParent);

	/*
	 *	Creates a new page.  This involves creating it in the database
	 *	and updating the page editor with a new blank page.
	 */
	bool CreateNewPage(CPageWidgetItem* pItem, PAGE_ADD pageAdd);

	/*
	 *	Creates a new folder.  This involves creating it in the database
	 *	and disabling the page editor.
	 */
	bool CreateNewFolder(CPageWidgetItem* pItem);

    /**
     * Updates the parent of a page entity (could be a page, folder, etc.)
     * @param pageId
     * @param newParentId
     */
    void UpdateParent(ENTITY_ID pageId, ENTITY_ID newParentId);

public slots:
	void OnItemClicked(QTreeWidgetItem* item, int column);
	void OnItemChanged(QTreeWidgetItem* item, int column);
	void OnItemExpanded(QTreeWidgetItem* pItem);
	void OnItemCollapsed(QTreeWidgetItem* pItem);
	void OnContextMenu(const QPoint& pos);

	void OnRenamePageActionTriggered();
	void OnDeletePageActionTriggered();

	void OnMoveFolder(int toFolderPageId);
	void OnMoveToTopLevel();
    void OnPageDeleted(ENTITY_ID pageId);
	void OnPageImported(CPageData pageData);
	void OnPageUpdatedByImport(CPageData pageData);

	/*
	 *	This slot is used when a page is selected by another means,
	 *	requiring the page tree to scroll to the selected page.
	 */
	void OnPageSelected(ENTITY_ID pageId);

    void OnNewPageActionTriggered();
    void OnNewToDoListActionTriggered();
    void OnNewFolderActionTriggered();
    void OnDeleteFolderActionTriggered();
    void OnNewTopLevelPageActionTriggered();
    void OnNewTopLevelFolderActionTriggered();

signals:
	void PageSelected(ENTITY_ID pageId);
	void NewBlankPageCreated(CPageData pageData);
	void NewFolderCreated(CPageData pageData);
	void PT_PageTitleChanged(CPageData pageData);
	void PT_PageDeleted(ENTITY_ID pageId);
    void PT_OnCreateNewPage();
    void PT_OnCreateNewToDoList();
    void PT_OnCreateNewFolder();
    void PT_OnCreateNewTopLevelPage();
    void PT_OnCreateNewTopLevelFolder();

protected:
    void SetConnections(QWidget *parent);

	/*
	 *	Performs a depth-first search of the tree looking for pageId
	 */
	CPageWidgetItem* FindItemInSubTree(QTreeWidgetItem *pItem, ENTITY_ID pageId);

	void GetFolderListFromSubTree(QTreeWidgetItem* pTreeWidgetItem, PageItemList& existingList);
	PageItemList GetFolderList();
	void InitMenus();

    void DeletePage();
    void DeleteFolder();

	void ConstructFolderSubmenu();
    bool IsPointOnPage(const QPoint &pt);
    bool IsPointOnFolder(const QPoint& pt);
	bool IsPointOnWhiteSpace(const QPoint& pt);
    bool IsFolderEmpty(QTreeWidgetItem *pItem);

	virtual void mouseDoubleClickEvent(QMouseEvent *event);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dragMoveEvent(QDragMoveEvent *event);
	virtual void dropEvent(QDropEvent *event);

	void GetTreeListForSubfolder(QTreeWidgetItem* pTreeWidgetItem, ENTITY_LIST& existingList);

private:
    CDatabase           *m_db;

    // Page actions
	QAction				*m_pRenamePageAction;
	QAction				*m_pDeletePageAction;
	QAction				*m_pExpandAllAction;
	QAction				*m_pCollapseAllAction;

    // Folder actions
    QAction				*m_pNewPageAction;
    QAction             *m_pNewToDoListAction;
    QAction				*m_pNewFolderAction;
    QAction				*m_pDeleteEmptyFolderAction;

    // Blank area actions
    QAction             *m_pNewTopLevelPageAction;
    QAction             *m_pNewTopLevelFolderAction;

    QMenu				m_pageContextMenu;				// Context menu for pages
    QMenu				m_folderContextMenu;		// Context menu for folders
    QMenu				m_blankAreaContextMenu;		// Context menu for blank area
	QMenu				m_folderListSubmenu;

	QSignalMapper		m_folderItemMapper;

	// The most-recently clicked item.  Used for drag & drop and context menus
	CPageWidgetItem		*m_lastClickedPage;
};

#endif // PAGETREE_H
