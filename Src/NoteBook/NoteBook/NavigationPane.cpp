#include "stdafx.h"
#include "NavigationPane.h"
#include "PageTree.h"
#include "DateTree.h"


//--------------------------------------------------------------------------
CNavigationPane::CNavigationPane()
{
}

//--------------------------------------------------------------------------
CNavigationPane::~CNavigationPane(void)
{
}

//--------------------------------------------------------------------------
void CNavigationPane::SetControlPointers( CPageTree* pPageTree )
{
	m_pPageTree = pPageTree;
}

//--------------------------------------------------------------------------
void CNavigationPane::AddItem(	ENTITY_ID pageId,
								ENTITY_ID parentId,
								const QString& pageTitle,
								const QStringList& tagList,
								const QDateTime& modifiedTime )
{
	// TODO: Add this item to:
	//		- pageTree
	//		- pageTitleList
	//		- dateTree
	//		- tagList

	// TODO: The item type should be set by the caller.
	m_pPageTree->AddTopLevelItem(pageTitle, pageId, eItemPage);
}

//--------------------------------------------------------------------------
// TODO: I can't see a reason to not just call into the page tree directly.
void CNavigationPane::NewItem(PAGE_ADD pageAdd, PAGE_ADD_WHERE pageAddWhere, QString pageTitle)
{
    m_pPageTree->NewItem(pageAdd, pageAddWhere, pageTitle);
}
