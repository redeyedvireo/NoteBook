#include "stdafx.h"
#include "PageTree.h"
#include "Database.h"
#include "PageCache.h"
#include <QApplication>
#include <QMessageBox>
#include <QMutableListIterator>



/************************************************************************/
/* CPageWidgetItem                                                      */
/************************************************************************/

const int kPageWidgetType = QTreeWidgetItem::UserType + 2;

CPageWidgetItem::CPageWidgetItem(QTreeWidgetItem *parent, const int pageId, PageWidgetItemType itemType)
: QTreeWidgetItem(parent, kPageWidgetType)
{
	m_pageId = pageId;
	m_itemType = itemType;

	setText(0, "Untitled Page");
	setFlags(flags() | Qt::ItemIsEditable);

	// Set icon based on m_itemType
    switch (m_itemType)
    {
    case eItemPage:
        //QIcon	pageIcon(":/NoteBook/Resources/Page.png");
        setIcon(0, QIcon(":/NoteBook/Resources/Page.png"));
        break;

    case eItemToDoList:
        setIcon(0, QIcon(":/NoteBook/Resources/ToDoList.png"));
        break;

    case eItemFolder:
        setIcon(0, QIcon(":/NoteBook/Resources/Folder Closed.png"));
        break;
    }
}

//--------------------------------------------------------------------------
void CPageWidgetItem::UpdateIcon()
{
	if (isExpanded())
	{
		setIcon(0, QIcon(":/NoteBook/Resources/Folder Open.png"));
	}
	else
	{
		setIcon(0, QIcon(":/NoteBook/Resources/Folder Closed.png"));
	}
}


/************************************************************************/
/* CPageTree                                                            */
/************************************************************************/

CPageTree::CPageTree(QWidget *parent)
	: QTreeWidget(parent)
{
	m_lastClickedPage = NULL;

	setContextMenuPolicy(Qt::CustomContextMenu);

	setAcceptDrops(true);
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::InternalMove);

	InitMenus();
}

//--------------------------------------------------------------------------
CPageTree::~CPageTree()
{

}

//--------------------------------------------------------------------------
void CPageTree::InitMenus()
{
    m_pRenamePageAction = m_pageContextMenu.addAction("Rename Page",
												this, SLOT(OnRenamePageActionTriggered()));
    m_pDeletePageAction = m_pageContextMenu.addAction("Delete Page", this, SLOT(OnDeletePageActionTriggered()));
    m_pageContextMenu.addSeparator();

	m_folderListSubmenu.setTitle(tr("Move to folder"));
    m_pageContextMenu.addMenu(&m_folderListSubmenu);
    m_pageContextMenu.addAction("Move to top-level", this, SLOT(OnMoveToTopLevel()));

    // Folder context menu
	m_pNewPageAction = m_folderContextMenu.addAction("New Page", this, SLOT(OnNewPageActionTriggered()));
    m_pNewToDoListAction = m_folderContextMenu.addAction(tr("New To Do List"), this, SLOT(OnNewToDoListActionTriggered()));
	m_pNewFolderAction = m_folderContextMenu.addAction("New Folder", this, SLOT(OnNewFolderActionTriggered()));
	m_folderContextMenu.addAction("Rename Folder", this, SLOT(OnRenamePageActionTriggered()));

    // For now, deleting non-empty folders is not supported.  This could be a very involved operation, as child pages and folders
    // will have to be deleted also.  A recursive function will be needed to delete a folder.  The UI should not be
    // updated until all children of the folder have been deleted.
    m_pDeleteEmptyFolderAction = m_folderContextMenu.addAction("Delete Empty Folder", this, SLOT(OnDeleteFolderActionTriggered()));
	m_folderContextMenu.addSeparator();

	m_pExpandAllAction = m_folderContextMenu.addAction("Expand All", this, SLOT(expandAll()));
	m_pCollapseAllAction = m_folderContextMenu.addAction("Collapse All", this, SLOT(collapseAll()));

    // Blank Area context menu (context menu that appears when user right-clicks on blank area in the pane)
    m_pNewTopLevelPageAction = m_blankAreaContextMenu.addAction("New Page", this, SLOT(OnNewTopLevelPageActionTriggered()));
    m_pNewTopLevelFolderAction = m_blankAreaContextMenu.addAction("New Folder", this, SLOT(OnNewTopLevelFolderActionTriggered()));
}

//--------------------------------------------------------------------------
void CPageTree::Initialize(QWidget *parent, CDatabase *db)
{
    m_db = db;
	SetConnections(parent);
}

//--------------------------------------------------------------------------
void CPageTree::SetConnections(QWidget *parent)
{
	connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(OnItemClicked(QTreeWidgetItem*, int)));
	connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(OnItemChanged(QTreeWidgetItem*, int)));
	connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(OnItemExpanded(QTreeWidgetItem*)));
	connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(OnItemCollapsed(QTreeWidgetItem*)));

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));

	connect(&m_folderItemMapper, SIGNAL(mapped(int)), this, SLOT(OnMoveFolder(int)));

	connect(parent, SIGNAL(MW_PageImported(CPageData)), this, SLOT(OnPageImported(CPageData)));
	connect(parent, SIGNAL(MW_PageUpdatedByImport(CPageData)), this, SLOT(OnPageUpdatedByImport(CPageData)));

	connect(parent, SIGNAL(MW_PageSelected(ENTITY_ID)), this, SLOT(OnPageSelected(ENTITY_ID)));
    connect(parent, SIGNAL(MW_PageDeleted(ENTITY_ID)), this, SLOT(OnPageDeleted(ENTITY_ID)));
}

//--------------------------------------------------------------------------
void CPageTree::AddTopLevelItem( const QString& pageTitle, ENTITY_ID pageId, PageWidgetItemType itemType )
{
	CPageWidgetItem*	pNewItem = new (std::nothrow) CPageWidgetItem(0, pageId, itemType);

	if (pNewItem != NULL)
	{
		pNewItem->setText(0, pageTitle);
		addTopLevelItem(pNewItem);
	}
}

//--------------------------------------------------------------------------
void CPageTree::NewItem(PAGE_ADD pageAdd, PAGE_ADD_WHERE pageAddWhere, QString title)
{
	ENTITY_ID	newPageId = kInvalidPageId;

    PageWidgetItemType	newItemType;

    switch (pageAdd)
    {
    case eNewPage:
        newItemType = eItemPage;
        break;

    case eNewFolder:
        newItemType = eItemFolder;
        break;

    case eNewToDoListPage:
        newItemType = eItemToDoList;
        break;

    default:
        newItemType = eItemPage;
        break;
    }

	CPageWidgetItem*	pNewItem = new (std::nothrow) CPageWidgetItem(0, newPageId, newItemType);

	if (pNewItem != NULL)
	{
		CPageWidgetItem*	pCurrentItem = dynamic_cast<CPageWidgetItem*>(currentItem());

		//  - if there is no current item in the page tree, then add a new top-level item
		//  - if the current item is a page (not a folder), then add a sibling item
		//  - if the current item is a folder, then add a child

		if (pCurrentItem == NULL || pageAddWhere == ePageAddTopLevel)
		{
			// There is no current item, so add a top-level item
			addTopLevelItem(pNewItem);
		}
		else
		{
			if (pCurrentItem->PageItemType() == eItemPage)
			{
				// The current item is a page, so add a sibling page
				QTreeWidgetItem*	pParent = pCurrentItem->parent();

				if (pParent != NULL)
				{
					pParent->addChild(pNewItem);
				}
				else
				{
					// pCurrentItem must be top-level (ie, no parent)
					int		curItemIndex = indexOfTopLevelItem(pCurrentItem);

					if (curItemIndex != -1)
					{
						insertTopLevelItem(curItemIndex + 1, pNewItem);
					}
				}
			}
			else if (pCurrentItem->PageItemType() == eItemFolder)
			{
				// The current item is a folder, so add a child item
				pCurrentItem->addChild(pNewItem);
				pCurrentItem->setExpanded(true);
			}
		}

		bool	bResult = false;
		if (newItemType == eItemFolder)
		{
			bResult = CreateNewFolder(pNewItem);
		}
		else
		{
            bResult = CreateNewPage(pNewItem, pageAdd);
		}

		if (bResult)
		{
			scrollToItem(pNewItem);

            if (title.isEmpty())
            {
                // The title parameter was empty, so make the item editable
                // so the user can enter the page title.
                editItem(pNewItem, 0);
            }
            else
            {
                // Use title as the item's title.
                pNewItem->setText(0, title);
            }
		}
	}
}


//--------------------------------------------------------------------------
bool CPageTree::AddItem( ENTITY_ID pageId, ENTITY_ID parentId, PageWidgetItemType itemType )
{
	Q_ASSERT(pageId != kInvalidPageId);

	CPageWidgetItem*	pParent;

	if (pageId == 0)
		return false;			// Invalid page ID

	// If the item has a parent, make sure the parent exists in the tree
	if (parentId != kInvalidPageId)
	{
		pParent = FindItem(parentId);

		if (pParent == NULL)
		{
			// The parent was not found
			return false;
		}
	}

	CPageWidgetItem*	pNewItem = new (std::nothrow) CPageWidgetItem(0, pageId, itemType);

	if (pNewItem != NULL)
	{
		pNewItem->setText(0, CPageCache::Instance()->PageTitle(pageId));

		if (parentId == kInvalidPageId)
		{
			// No parent - add as top-level item
			addTopLevelItem(pNewItem);
		}
		else
		{
			// Add as child
			pParent->addChild(pNewItem);
		}
	}

	return true;
}


//--------------------------------------------------------------------------
void CPageTree::AddItems( const PAGE_HASH& pageHash )
{
	clear();

	PAGE_LIST	folderList;

	// First pull out all the folders from the pageHash
	QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	it(pageHash);

	while (it.hasNext())
	{
		it.next();

		SHARED_CPAGEDATA	pageData = it.value();

		if (pageData.data()->m_pageType == kPageFolder)
		{
			// Found a folder - add to a temporary list
			folderList.append(*pageData.data());
		}
	}

	// Iterate through the folder list and add to the Page Tree.
	// For any item whose parent has not yet been added to the Page Tree,
	// skip it, and repeat this loop.
	// In the worst case, this loop will need to be executed once for each item in folderList.
	// In most cases, though, it will only likely be executed a small number of times.
	for (int iterateNum = folderList.size(); iterateNum > 0 && folderList.size() > 0; iterateNum--)
	{
		// Traverse through folderList, and add items to the Page Tree
		QMutableListIterator<CPageData>		folderListIt(folderList);

		while (folderListIt.hasNext())
		{
			folderListIt.next();

			CPageData	pageData = folderListIt.value();

			// Attempt to add the folder.  If the page has no parent
			// it will be added successfully.  If it has a parent and the
			// parent exists in the Page Tree, it will also be added successfully.
			// If it has a parent and the parent doesn't exist (because the
			// parent hasn't been added yet), skip it for now.  A subsequent
			// iteration of this loop will add it.
			if (AddItem(pageData.m_pageId, pageData.m_parentId, eItemFolder))
			{
				// Added successfully, so remove the item from the list
				folderListIt.remove();
			}
		}
	}

	// There should be no folders remaining in the folder list - they should
	// all have been added to the tree.
	Q_ASSERT(folderList.size() == 0);

	// Add the rest of the pages (these should all be regular pages; no folders).
	QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	pageIt(pageHash);

	while (pageIt.hasNext())
	{
		pageIt.next();

		SHARED_CPAGEDATA	pageData = pageIt.value();

		if (pageData.data()->m_pageType != kPageFolder)
		{
			AddItem(pageIt.key(), pageData.data()->m_parentId, eItemPage);
		}
	}
}

//--------------------------------------------------------------------------
void CPageTree::AddItemsNew(const PAGE_HASH& pageHash, const QString& pageOrderStr)
{
	if (pageOrderStr.length() > 0)
	{
		AddItemsUsingPageOrderString(pageHash, pageOrderStr);
		return;
	}

	// ****************************************************************************
	// This is the old (legacy) way to add items.  Going forward, notebooks should
	// use the page order string (above) to add their pages to the page tree.
	// ****************************************************************************
	PAGE_LIST	tryAgainList;

	// Add the rest of the pages (these should all be regular pages; no folders).
	QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	pageIt(pageHash);

	while (pageIt.hasNext())
	{
		pageIt.next();

		SHARED_CPAGEDATA	pageData = pageIt.value();

		// Attempt to add the page.  If the page has no parent
		// it will be added successfully.  If it has a parent and the
		// parent exists in the Page Tree, it will also be added successfully.
		// If it has a parent and the parent doesn't exist (because the
		// parent hasn't been added yet), skip it for now.  A subsequent
		// iteration of this loop will add it.
        PageWidgetItemType	pageType;

        switch (pageData.data()->m_pageType)
        {
        case kPageTypeUserText:
        case kPageTypeToDoList:
            pageType = eItemPage;
            break;

        case kPageFolder:
            pageType = eItemFolder;
            break;

        default:
            pageType = eItemPage;
        }

		if (! AddItem(pageData.data()->m_pageId, pageData.data()->m_parentId, pageType))
		{
			tryAgainList.append(*pageData.data());
		}
	}

	// Add the items in the "try again" list, to add pages or folders that weren't able to be added
	// in the above loop, because their parents hadn't yet been added.
	for (int iterateNum = tryAgainList.size(); iterateNum > 0 && tryAgainList.size() > 0; iterateNum--)
	{
		// Traverse through folderList, and add items to the Page Tree
		QMutableListIterator<CPageData>		tryAgainListIt(tryAgainList);

		while (tryAgainListIt.hasNext())
		{
			tryAgainListIt.next();

			CPageData	pageData = tryAgainListIt.value();

			// Attempt to add the page.  If the page has no parent
			// it will be added successfully.  If it has a parent and the
			// parent exists in the Page Tree, it will also be added successfully.
			// If it has a parent and the parent doesn't exist (because the
			// parent hasn't been added yet), skip it for now.  A subsequent
			// iteration of this loop will add it.

			PageWidgetItemType	pageType = pageData.m_pageType == kPageTypeUserText ? eItemPage : eItemFolder;

			if (AddItem(pageData.m_pageId, pageData.m_parentId, pageType))
			{
				// Added successfully, so remove the item from the list
				tryAgainListIt.remove();
			}
		}
	}
}

//--------------------------------------------------------------------------
void CPageTree::AddItemsUsingPageOrderString(const PAGE_HASH& pageHash, const QString& pageOrderStr)
{
	QStringList		pageIdList = pageOrderStr.split(",");

	foreach (const QString& pageIdStr, pageIdList)
	{
		SHARED_CPAGEDATA	pageData;
		ENTITY_ID			pageId = pageIdStr.toInt();

		if (pageId == kInvalidPageId)
		{
			continue;
		}

		pageData = pageHash.value(pageId);

		// ** Must determine if the item is a regular page or a folder.
		if (!pageData.isNull())
		{
            PageWidgetItemType	pageType;

            switch (pageData.data()->m_pageType)
            {
            case kPageTypeUserText:
                pageType = eItemPage;
                break;

            case kPageTypeToDoList:
                pageType = eItemToDoList;
                break;

            case kPageFolder:
                pageType = eItemFolder;
                break;

            default:
                pageType = eItemPage;
            }

			if (! AddItem(pageData.data()->m_pageId, pageData.data()->m_parentId, pageType))
			{
				// Should never get here
				// Q_ASSERT(false);
				qDebug() << "CPageTree::AddItemsUsingPageOrderString: Error adding item "
						 << pageData.data()->m_pageId;
			}
		}
	}
}

//--------------------------------------------------------------------------
bool CPageTree::MoveItem(CPageWidgetItem *pItem, CPageWidgetItem *pNewParent)
{
	bool	bReturn = false;

	if (pItem == NULL)
	{
		return false;
	}

	// Get parent of page to be moved
	CPageWidgetItem		*pParentItem = dynamic_cast<CPageWidgetItem*>(pItem->parent());

	if (pParentItem != NULL)
	{
		int		childIndex = pParentItem->indexOfChild(pItem);
		if (childIndex != -1)
		{
			QTreeWidgetItem*	pTempItem = pParentItem->takeChild(childIndex);

			// pTempItem should be the same as pItem
			Q_ASSERT(pItem == pTempItem);

			// Insert at new location
			if (pNewParent != NULL)
			{
				pNewParent->addChild(pTempItem);
			}
			else
			{
				// If pNewParent is NULL, move the item to the top-level
				addTopLevelItem(pTempItem);
			}
			bReturn = true;
		}
	}
	else
	{
		// pItem is a top-level item.
		int		index = indexOfTopLevelItem(pItem);

		if (index != -1)
		{
			CPageWidgetItem		*pTempItem = dynamic_cast<CPageWidgetItem*>(takeTopLevelItem(index));

			if (pTempItem != NULL)
			{
				// Insert at new location
				if (pNewParent != NULL)
				{
					pNewParent->addChild(pTempItem);
				}
				else
				{
					// If pNewParent is NULL, move the item to the top-level
					addTopLevelItem(pTempItem);
				}
				bReturn = true;
			}
			else
			{
				// Should never get here
				Q_ASSERT(false);
			}
		}
	}

	return bReturn;
}

//--------------------------------------------------------------------------
CPageWidgetItem* CPageTree::FindItemInSubTree(QTreeWidgetItem *pItem, ENTITY_ID pageId)
{
	CPageWidgetItem*	pPageItem = dynamic_cast<CPageWidgetItem*>(pItem);
	if (pPageItem && pPageItem->PageId() == pageId)
	{
		return pPageItem;
	}

	for (int i = 0; i < pItem->childCount(); i++)
	{
		QTreeWidgetItem*	pTreeItem = pItem->child(i);
		CPageWidgetItem*	pSubPageItem = dynamic_cast<CPageWidgetItem*>(pTreeItem);

		if (pSubPageItem && pSubPageItem->PageId() == pageId)
		{
			return pSubPageItem;
		}

		// Search pTreeItem's children
		CPageWidgetItem*	pFoundItem = FindItemInSubTree(pTreeItem, pageId);
		if (pFoundItem)
		{
			return pFoundItem;
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------
CPageWidgetItem* CPageTree::FindItem(ENTITY_ID pageId)
{
	if (pageId == kInvalidPageId)
		return NULL;		// No item will have this page ID
	else
		return FindItemInSubTree(invisibleRootItem(), pageId);
}

//--------------------------------------------------------------------------
void CPageTree::GetFolderListFromSubTree(QTreeWidgetItem* pTreeWidgetItem, PageItemList& existingList)
{
	for (int i = 0; i < pTreeWidgetItem->childCount(); i++)
	{
		CPageWidgetItem*	pSubPageItem = dynamic_cast<CPageWidgetItem*>(pTreeWidgetItem->child(i));

		if (pSubPageItem && pSubPageItem->PageItemType() == eItemFolder)
		{
			existingList.append(pSubPageItem);

			// Search folder's children
			GetFolderListFromSubTree(pSubPageItem, existingList);
		}
	}
}

//--------------------------------------------------------------------------
PageItemList CPageTree::GetFolderList()
{
	PageItemList	pageItemList;

	GetFolderListFromSubTree(invisibleRootItem(), pageItemList);

	return pageItemList;
}

//--------------------------------------------------------------------------
bool CPageTree::CreateNewPage(CPageWidgetItem* pItem, PAGE_ADD pageAdd)
{
	bool		bReturn = false;
	CPageData	newPageData;

	// This is a new item
//	newPageData.m_pageId = pItem->PageId();	// TODO: I don't think this is necessary (pageId is not defined at this point)

	newPageData.m_title = pItem->text(0);

	CPageWidgetItem*	pParent = dynamic_cast<CPageWidgetItem*>(pItem->parent());

	if (pParent != NULL)
	{
		newPageData.m_parentId = pParent->PageId();
	}

    if (pageAdd == eNewToDoListPage)
    {
        newPageData.m_pageType = kPageTypeToDoList;
    }

    int	newPageId = m_db->AddNewBlankPage(newPageData);

	if (newPageId != kInvalidPageId)
	{
		pItem->SetPageId(newPageId);
		newPageData.m_pageId = newPageId;

		CPageCache::Instance()->AddPage(newPageId, pItem->text(0));

        WritePageOrderToDatabase();

		emit NewBlankPageCreated(newPageData);
		bReturn = true;
	}

	return bReturn;
}

//--------------------------------------------------------------------------
bool CPageTree::CreateNewFolder(CPageWidgetItem* pItem)
{
	bool		bReturn = false;
	CPageData	newPageData;

	newPageData.m_title = pItem->text(0);
	newPageData.m_pageType = kPageFolder;

	CPageWidgetItem*	pParent = dynamic_cast<CPageWidgetItem*>(pItem->parent());

	if (pParent != NULL)
	{
		newPageData.m_parentId = pParent->PageId();
	}

    int	newPageId = m_db->AddNewFolder(newPageData);

	if (newPageId != kInvalidPageId)
	{
		pItem->SetPageId(newPageId);
		newPageData.m_pageId = newPageId;

		CPageCache::Instance()->AddPage(newPageId, pItem->text(0));

        WritePageOrderToDatabase();

		emit NewFolderCreated(newPageData);
		bReturn = true;
	}

	return bReturn;
}

//--------------------------------------------------------------------------
void CPageTree::DeletePage()
{
    // Emit message to mainwindow that the page is being deleted.  The mainwindow will reflect
    // that signal, and the Page Title List and Date Tree will respond to the
    // reflected signal to change their names.  Also, this class, CPageTree, will handle
    // that signal, too, which is where the work is done to update this widget and the database.
    emit PT_PageDeleted(m_lastClickedPage->PageId());
}

//--------------------------------------------------------------------------
void CPageTree::DeleteFolder()
{
    if (m_lastClickedPage != NULL && m_lastClickedPage->PageItemType() == eItemFolder)
    {
        // For now, only empty folders can be deleted.

        // Eventually, deleting non-empty folders will be supported.  This will be a somewhat complicated
        // process, as all the pages that are contained within it will need to be deleted.  And if there
        // are any folders contained within it, those will have to be deleted as well,
        // plus any pages that those folders contain.

        if (IsFolderEmpty(m_lastClickedPage))
        {
            DeletePage();
        }
    }
}


/************************************************************************/
/* S L O T S                                                            */
/************************************************************************/

void CPageTree::OnItemClicked( QTreeWidgetItem* item, int column )
{
	CPageWidgetItem*	pPageWidgetItem = dynamic_cast<CPageWidgetItem*>(item);

	if (pPageWidgetItem != NULL)
	{
		ENTITY_ID		pageId = pPageWidgetItem->PageId();
		emit PageSelected(pageId);
	}
}

//--------------------------------------------------------------------------
void CPageTree::OnItemChanged( QTreeWidgetItem* item, int column )
{
	setCurrentItem(item);

	CPageData	newPageData;

	CPageWidgetItem*	pItem = dynamic_cast<CPageWidgetItem*>(item);

	if (pItem == NULL)
		return;

	if (pItem->PageId() != kInvalidPageId)
	{
		// If name changed:
		// - Update name in database
		// - emit message to mainwindow about the name change.  The mainwindow will reflect
		//    that signal, and the Page Title List and Date Tree will respond to the
		//    reflected signal to change their names.
		// - Add function in the CPageCache to change the title
        m_db->ChangePageTitle(pItem->PageId(), pItem->text(0), true);

		CPageData	pageData;

        bool bRet = m_db->GetPartialPage(pItem->PageId(), pageData,
			CDatabase::PageTitle | CDatabase::PageModification | CDatabase::PageNumModifications);

		Q_ASSERT(bRet);

		if (bRet)
		{
			pageData.m_title = pItem->text(0);

			emit PT_PageTitleChanged(pageData);
			CPageCache::Instance()->ChangeTitle(pageData.m_pageId, pageData.m_title);
		}
	}
	else
	{
		// Should never happen
		Q_ASSERT(false);
	}
}

//--------------------------------------------------------------------------
void CPageTree::OnItemExpanded(QTreeWidgetItem* pItem)
{
	CPageWidgetItem*	pPageItem = dynamic_cast<CPageWidgetItem*>(pItem);
	if (pPageItem != NULL)
	{
		pPageItem->UpdateIcon();
	}
}

//--------------------------------------------------------------------------
void CPageTree::OnItemCollapsed(QTreeWidgetItem* pItem)
{
	CPageWidgetItem*	pPageItem = dynamic_cast<CPageWidgetItem*>(pItem);
	if (pPageItem != NULL)
	{
		pPageItem->UpdateIcon();
	}
}

//--------------------------------------------------------------------------
void CPageTree::ConstructFolderSubmenu()
{
	m_folderListSubmenu.clear();

	PageItemList	pageList = GetFolderList();

	foreach(CPageWidgetItem* pItem, pageList)
	{
		QAction*	pNewAction = new (std::nothrow) QAction(pItem->text(0), 0);

		if (pNewAction != NULL)
		{
			pNewAction->setData(QVariant(pItem->PageId()));
			m_folderListSubmenu.addAction(pNewAction);
			m_folderItemMapper.setMapping(pNewAction, pItem->PageId());

			connect(pNewAction, SIGNAL(triggered()), &m_folderItemMapper, SLOT(map()));
		}
	}
}

//--------------------------------------------------------------------------
void CPageTree::OnContextMenu( const QPoint& pos )
{
	QTreeWidgetItem*	pItem = itemAt(pos);

	if (pItem != NULL)
	{
		m_lastClickedPage = dynamic_cast<CPageWidgetItem*>(pItem);

		ConstructFolderSubmenu();
        if (IsPointOnPage(pos))
        {
            m_pageContextMenu.popup(mapToGlobal(pos));
        }
        else if (IsPointOnFolder(pos))
        {
            // For now, deleting non-empty folders is not supported.  So,
            // check if the folder is empty, and if not, hide the "Delete Empty Folder"
            // menu item.
            m_pDeleteEmptyFolderAction->setVisible(IsFolderEmpty(pItem));
            m_folderContextMenu.popup(mapToGlobal(pos));
        }
	}
    else
    {
        // User clicked in white space
        m_blankAreaContextMenu.popup(mapToGlobal(pos));
    }
}

//--------------------------------------------------------------------------
void CPageTree::OnRenamePageActionTriggered()
{
	QTreeWidgetItem		*pItem = currentItem();

	if (pItem != NULL)
	{
		editItem(pItem, 0);
	}
}

//--------------------------------------------------------------------------
void CPageTree::OnDeletePageActionTriggered()
{
    if (m_lastClickedPage != NULL)
    {
        QString		message = QString("Do you want to delete the page '%1'").arg(m_lastClickedPage->text(0));

        if (QMessageBox::question(this, "NoteBook - Delete Page", message, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            DeletePage();
        }
    }
}

//--------------------------------------------------------------------------
void CPageTree::OnMoveFolder(int toFolderPageId)
{
	if (m_lastClickedPage)
	{
		CPageWidgetItem	*pFolderItem = FindItem(toFolderPageId);

		if (pFolderItem && pFolderItem->PageItemType() == eItemFolder)
		{
			if (MoveItem(m_lastClickedPage, pFolderItem))
			{
                UpdateParent(m_lastClickedPage->PageId(), pFolderItem->PageId());
			}
		}
	}
}

//--------------------------------------------------------------------------
void CPageTree::OnMoveToTopLevel()
{
	if (m_lastClickedPage)
	{
		if (MoveItem(m_lastClickedPage, NULL))
		{
			// Set the page's parent to 0, to indicate it is a top-level item
            UpdateParent(m_lastClickedPage->PageId(), kInvalidPageId);
		}
	}
}

//--------------------------------------------------------------------------
void CPageTree::OnPageSelected( ENTITY_ID pageId )
{
	// Scroll to the page in the tree
	CPageWidgetItem*	pSelectedItem =  FindItem(pageId);

	if (pSelectedItem != NULL)
	{
		scrollToItem(pSelectedItem);
		setCurrentItem(pSelectedItem);
	}
}

//--------------------------------------------------------------------------
void CPageTree::OnNewPageActionTriggered()
{
    emit PT_OnCreateNewPage();
}

//--------------------------------------------------------------------------
void CPageTree::OnNewToDoListActionTriggered()
{
    emit PT_OnCreateNewToDoList();
}

//--------------------------------------------------------------------------
void CPageTree::OnNewFolderActionTriggered()
{
    emit PT_OnCreateNewFolder();
}

//--------------------------------------------------------------------------
void CPageTree::OnDeleteFolderActionTriggered()
{
    if (m_lastClickedPage != NULL)
    {
        QString		message = QString("Do you want to delete empty folder '%1'?").arg(m_lastClickedPage->text(0));

        if (QMessageBox::question(this, "NoteBook - Delete Folder", message, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            DeleteFolder();
        }
    }
}

//--------------------------------------------------------------------------
void CPageTree::OnNewTopLevelPageActionTriggered()
{
    emit PT_OnCreateNewTopLevelPage();
}

//--------------------------------------------------------------------------
void CPageTree::OnNewTopLevelFolderActionTriggered()
{
    emit PT_OnCreateNewTopLevelFolder();
}

//--------------------------------------------------------------------------
bool CPageTree::IsPointOnPage(const QPoint& pt)
{
    CPageWidgetItem*	pItem = dynamic_cast<CPageWidgetItem*>(itemAt(pt));

    if (pItem && (pItem->PageItemType() == eItemPage || pItem->PageItemType() == eItemToDoList))
        return true;
    else
        return false;
}

//--------------------------------------------------------------------------
bool CPageTree::IsPointOnFolder(const QPoint& pt)
{
	CPageWidgetItem*	pItem = dynamic_cast<CPageWidgetItem*>(itemAt(pt));

	if (pItem && pItem->PageItemType() == eItemFolder)
		return true;
	else
		return false;
}

//--------------------------------------------------------------------------
bool CPageTree::IsPointOnWhiteSpace(const QPoint& pt)
{
	CPageWidgetItem*	pItem = dynamic_cast<CPageWidgetItem*>(itemAt(pt));

	// I'm going to assume that if pItem is NULL, then pt is within the
	// blank area of the pane.
	if (pItem == NULL)
		return true;
	else
		return false;
}

//--------------------------------------------------------------------------
bool CPageTree::IsFolderEmpty(QTreeWidgetItem* pItem)
{
    Q_ASSERT(pItem != NULL);

    if (pItem == NULL)
    {
        return true;
    }

    return pItem->childCount() == 0;
}

//--------------------------------------------------------------------------
void CPageTree::mouseDoubleClickEvent( QMouseEvent *event )
{
	// Double-clicking in blank space will create a new top-level item.
	if (itemAt(event->pos()) == NULL)
	{
		// TODO: May want to have a signal that requests that the
		//       edit controls be cleared.

        OnNewTopLevelPageActionTriggered();
	}
}

//--------------------------------------------------------------------------
void CPageTree::mousePressEvent( QMouseEvent *event )
{
	QTreeWidget::mousePressEvent(event);

	CPageWidgetItem*	pItem = dynamic_cast<CPageWidgetItem*>(itemAt(event->pos()));
	m_lastClickedPage = pItem;

#ifdef _DEBUG
	if (pItem)
	{
		qDebug() << "mousePressEvent: item: " << pItem->text(0);
	}
#endif
}

//--------------------------------------------------------------------------
void CPageTree::dragEnterEvent( QDragEnterEvent *event )
{
	// TODO: Need to allow only drops from within this widget
	if (m_lastClickedPage != NULL)
	{
		event->acceptProposedAction();
	}
}

//--------------------------------------------------------------------------
void CPageTree::dragMoveEvent( QDragMoveEvent *event )
{
	QTreeWidget::dragMoveEvent(event);

	if ( dropIndicatorPosition() != QAbstractItemView::OnItem ||
		(dropIndicatorPosition() == QAbstractItemView::OnItem && IsPointOnFolder(event->pos())))
	{
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

//--------------------------------------------------------------------------
void CPageTree::dropEvent( QDropEvent *event )
{
	QTreeWidget::dropEvent(event);

	// Item has been moved.  Now update its parent
	CPageWidgetItem		*pParent = dynamic_cast<CPageWidgetItem*>(m_lastClickedPage->parent());

	if (pParent != NULL)
	{
        UpdateParent(m_lastClickedPage->PageId(), pParent->PageId());
	}
	else
	{
		// The page was moved to the top level, so its parent ID will be kInvalidPageId
        UpdateParent(m_lastClickedPage->PageId(), kInvalidPageId);
	}
}

//--------------------------------------------------------------------------
void CPageTree::OnPageDeleted(ENTITY_ID pageId)
{
    CPageWidgetItem     *pPageItem = NULL;

    pPageItem = FindItem(pageId);

    if (pPageItem != NULL)
    {
        // - Delete all images contained by the page (if any).
        // - Delete the page in the database
        // - Delete the page from CPageCache
        // - Select the next page in the page tree, or if this was the last page in the page
        //		tree, then select the previous item
        // - Remove this item from the tree

        m_db->DeleteAllImagesForPage(pageId);

        m_db->DeletePage(pPageItem->PageId());

        CPageCache::Instance()->DeletePage(pPageItem->PageId());

        // Determine which item to select next
        //  - First try to select the item below.
        //  - If that item does not exist, try to select the item above
        //  - If that item does not exist, try to select the parent
        QTreeWidgetItem*	pNewItem = itemBelow(pPageItem);

        if (pNewItem == NULL)
        {
            // Try item above
            pNewItem = itemAbove(pPageItem);

            if (pNewItem == NULL)
            {
                // Try parent
                pNewItem = pPageItem->parent();
            }
        }

        if (pNewItem != NULL)
        {
            setCurrentItem(pNewItem);
        }

        // Remove the item from the tree
        if (pPageItem->parent() != NULL)
            pPageItem->parent()->removeChild(pPageItem);
        else
        {
            int index = indexOfTopLevelItem(pPageItem);
            takeTopLevelItem(index);
            delete pPageItem;
        }

        WritePageOrderToDatabase();

        CPageWidgetItem*	pNewPageItem = dynamic_cast<CPageWidgetItem*>(pNewItem);

        if (pNewPageItem != NULL)
        {
            emit PageSelected(pNewPageItem->PageId());
        }
    }
}

//--------------------------------------------------------------------------
void CPageTree::OnPageImported( CPageData pageData )
{
	// No page ID checking is done here, as that could slow things down greatly
	// during importing of a large number of pages.  This slot is only meant to
	// be called for new pages.
	AddTopLevelItem(pageData.m_title, pageData.m_pageId, eItemPage);
}

//--------------------------------------------------------------------------
void CPageTree::OnPageUpdatedByImport( CPageData pageData )
{
	CPageWidgetItem* pItem = FindItem(pageData.m_pageId);

	if (pItem != NULL)
	{
		pItem->setText(0, pageData.m_title);
	}
}

//--------------------------------------------------------------------------
QString CPageTree::GetTreeIdList()
{
	ENTITY_LIST		idList;

	GetTreeListForSubfolder(invisibleRootItem(), idList);

	return CPageData::EntityListToCommaSeparatedList(idList);
}

//--------------------------------------------------------------------------
void CPageTree::GetTreeListForSubfolder(QTreeWidgetItem* pTreeWidgetItem, ENTITY_LIST& existingList)
{
	for (int i = 0; i < pTreeWidgetItem->childCount(); i++)
	{
		CPageWidgetItem*	pSubPageItem = dynamic_cast<CPageWidgetItem*>(pTreeWidgetItem->child(i));

		if (pSubPageItem)
		{
			existingList.append(pSubPageItem->PageId());

			if (pSubPageItem->PageItemType() == eItemFolder)
			{
				// Search folder's children
				GetTreeListForSubfolder(pSubPageItem, existingList);
			}
		}
	}
}

//--------------------------------------------------------------------------
void CPageTree::WritePageOrderToDatabase()
{
    QString		pageOrderStr;

    pageOrderStr = GetTreeIdList();
    m_db->SetPageOrder(pageOrderStr);
}

//--------------------------------------------------------------------------
void CPageTree::UpdateParent(ENTITY_ID pageId, ENTITY_ID newParentId)
{
    m_db->UpdatePageParent(pageId, newParentId);
    WritePageOrderToDatabase();
}
